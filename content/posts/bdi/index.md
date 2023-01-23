---
title: "Limiting dirty writeback cache size for nbd volumes"
date: 2022-12-26T20:23:05-08:00
featuredImage: "bdi.jpg"
math: true
draft: false
toc:
  enable: true
tags: ["block", "fs", "mm", "bdi", "writeback", "btrfs"]
---

## Overview

Network block devices ([nbd](https://en.wikipedia.org/wiki/Network_block_device)) are often used to provide
remote block storage. It is convenient to then create a filesystem on top of the nbd device. For this article
the BTRFS filesystem is used. This choice has implications on how the backing device info gets allocated and is
explained later.

During testing the following problems have been identified:
- it takes a long time to write back dirty blocks
- under high memory pressure, the nbd network connections can be closed due to high memory pressure.

## Investigation

While investigating the high memory pressure, it was discovered that with the default settings
for dirty_ratio (20) and dirty_background_ratio (10), the writeback dirty cache can consume
considerable amounts of writeback cache. The dirty settings can be queried from the proc filesystem:
- [/proc/sys/vm/dirty_ratio](https://docs.kernel.org/admin-guide/sysctl/vm.html?highlight=dirty_ratio#dirty-ratio),
- [/proc/sys/vm/dirty_background_ratio](https://docs.kernel.org/admin-guide/sysctl/vm.html?highlight=dirty_ratio#dirty-background-ratio).


The amount of the dirty writeback cache can be queried with:
``` shell
  > grep Dirty /proc/meminfo
```

On a machine with 64GB main memory, up to 8GB dirty writeback memory have been observed when testing with block devices. On a machine with 256GB of main memory more than 20GB of dirty cache have been observed. To create the dirty memory, the following command was used:

``` shell
> dd if=/dev/zero of=/dev/nbd1 bs=10G seek=0
```

To also evaluate how this effects the writeback cache when using the BTRFS filesystem, the [fio](https://github.com/axboe/fio) program with 
the following command has been used:

``` shell
  > fio --rw=randwrite --bs=1m --ioengine=libaio --iodepth=64    \
        --runtime=300 --numjobs=4 --time_based --group_reporting \
        --name=throughput-test-job --size=1gb
```

When using filesystems up to 3GB of dirty writeback cache have been observed.

## Limiting the size of the writeback cache

To be able to make network block device volumes sustain memory pressure situations better,
the amount of writeback memory needs to be limited. The size of the writeback cache can
be limited by setting limits on the backing device info (BDI) of the device or filesystem.
The backing device info currently supports two knobs:
- [min_ratio](https://docs.kernel.org/admin-guide/abi-testing.html?highlight=sysfs+class+bdi#abi-sys-class-bdi-bdi-min-ratio) and
- [max_ratio](https://docs.kernel.org/admin-guide/abi-testing.html?highlight=sysfs+class+bdi#abi-sys-class-bdi-bdi-max-ratio)

The min_ratio allows assigning a minimum percentage of the writeback cache to a device. The max_ratio allows limiting a particular device to use not more than the given percentage of the writeback cache.
The two above ratio's are only applied once the dirty writeback cache size has reached
$$ \frac{dirty\text{\textunderscore}ratio + background\text{\textunderscore}dirty\text{\textunderscore}ratio}{2} $$

The existing [knobs](https://www.kernel.org/doc/Documentation/ABI/testing/sysfs-class-bdi) are documented.
As dirty dirty_ratio and dirty_background_ratio are global settings, changing these settings has bigger implications.
What is needed is something more fine grained.

## Patch with more controls

A new [patch](https://lore.kernel.org/all/20221119005215.3052436-1-shr@devkernel.io/T/#u) has been accepted upstream. The patch implements four changes:
1) Introduce strictlimit knob.
Currently the max_ratio knob exists to limit the dirty_memory. However this knob only applies once
$$ \frac{dirty\text{\textunderscore}ratio + background\text{\textunderscore}dirty\text{\textunderscore}ratio}{2} $$
has been reached.
With the [BDI_CAP_STRICTLIMIT](https://elixir.bootlin.com/linux/v6.1/source/include/linux/backing-dev.h#L118)
flag, the max_ratio can be applied without reaching that limit. This change exposes that knob.
This knob can also be useful for NFS, fuse filesystems and USB devices.
2) Use part of 1000000 internal calculation.
  The max_ratio is based on percentage. With the current machine sizes
  percentage values can be very high (1% of a 256GB main memory is already
  2.5GB). This change uses part of 1000000 instead of percentages for the
  internal calculations.
3) Introduce two new sysfs knobs: min_bytes and max_bytes.
  Currently all calculations are based on ratio, but for a user it often
  more convenient to specify a limit in bytes. The new knobs will not
  store bytes values, instead they will translate the byte value to a
  corresponding ratio. As the internal values are now part of 1000, the
  ratio is closer to the specified value. However the value should be more
  seen as an approximation as it can fluctuate over time.
4) Introduce two new sysfs knobs: min_ratio_fine and max_ratio_fine.
  The granularity for the existing sysfs BDI knobs min_ratio and max_ratio
  is based on percentage values. The new sysfs BDI knobs min_ratio_fine
  and max_ratio_fine allow to specify the ratio as part of 1 million.

The strictlimit knob is exposing exisitng kernel functionality. The knob is already used by the
fuse filesystem to always enable the strictlimit flag.

## How to apply the BDI settings

There are two different ways to apply BDI settings: to the block device and to the BTRFS filesystem.
For block devices the settings can be directly set in /sys/block/<device>/bdi. The BTRFS filesystem
creates its own BDI, which is different from the BDI of the block device. Each BTRFS BDI gets the
name "btrfs-<number>". The number is incremented for each filesystem. To get to the BDI of the
filesystem is more complicated. The following process needs to be followed (it is assumed that the
name of the device is known, which is the case for network block devices):
``` shell
  > blkid -s UUID -o value /dev/<nbd-device>
  This returns the uuid of the BTRFS filesystem

  > echo 1 > /sys/class/fs/btrfs/<uuid>/bdi/strict_limit
  Sets the strict_limit with the uuid from the previous command
```
It is important to understand that BTRFS does not apply the block device BDI settings, it only
applies its own settings. If a device can be used as a block device and as a filesystem (at different times),
it might make sense to specify BDI settings for the block device and the BTRFS filesystem.
The BDI settings are only stored in memory. After a filesystem has been mounted, or the device
has been loaded (in case it is a module), the settings have to be set again.

## Using the new sysctl knobs

Tests have shown that by specifying the new sysctl knobs, the size of the dirty writeback
cache can be limited accordingly. The following sysctl values have been used for some of the testing:
``` shell
  > echo 1 > /sys/class/bdi/btrfs-2/strict_limit
  > echo 1000000000 > /sys/block/nbd1/bdi/max_bytes
```
It should not be expected to reach max_ratio or max_bytes of dirty writeback cache as the
writeback will start before that limit is reached. By limiting the dirty writeback cache
size also the time to write back dirty blocks to the storage device has considerably decreased
and the write throughput has no more spikes and is more consistent throughput.

## Kernel internals

A BDI object gets allocated for all block devices. In addition some filesystems allocate their
own BDI object. One of these examples is the BTRFS filesystem.

### BTRFS BDI code path
When the BTRFS filesystem gets mounted, the [btrfs_mount_root()](https://elixir.bootlin.com/linux/v6.1/source/fs/btrfs/super.c#L1739)
function calls the [btrfs_fill_super()](https://elixir.bootlin.com/linux/v6.1/source/fs/btrfs/super.c#L1431) function.

{{< highlight c "linenos=table,linenostart=1431,hl_lines=25-29" >}}
static int btrfs_fill_super(struct super_block *sb,
			    struct btrfs_fs_devices *fs_devices,
			    void *data)
{
	struct inode *inode;
	struct btrfs_fs_info *fs_info = btrfs_sb(sb);
	int err;

	sb->s_maxbytes = MAX_LFS_FILESIZE;
	sb->s_magic = BTRFS_SUPER_MAGIC;
	sb->s_op = &btrfs_super_ops;
	sb->s_d_op = &btrfs_dentry_operations;
	sb->s_export_op = &btrfs_export_ops;
#ifdef CONFIG_FS_VERITY
	sb->s_vop = &btrfs_verityops;
#endif
	sb->s_xattr = btrfs_xattr_handlers;
	sb->s_time_gran = 1;
#ifdef CONFIG_BTRFS_FS_POSIX_ACL
	sb->s_flags |= SB_POSIXACL;
#endif
	sb->s_flags |= SB_I_VERSION;
	sb->s_iflags |= SB_I_CGROUPWB;

	err = super_setup_bdi(sb);
	if (err) {
		btrfs_err(fs_info, "super_setup_bdi failed");
		return err;
	}

	err = open_ctree(sb, fs_devices, (char *)data);
	if (err) {
		btrfs_err(fs_info, "open_ctree failed");
		return err;
	}

	inode = btrfs_iget(sb, BTRFS_FIRST_FREE_OBJECTID, fs_info->fs_root);
	if (IS_ERR(inode)) {
		err = PTR_ERR(inode);
{{< /highlight >}}

This function in turns calls [super_setup_bdi()](https://elixir.bootlin.com/linux/latest/source/fs/super.c#L1608).
The [super_setup_bdi()](https://elixir.bootlin.com/linux/latest/source/fs/super.c#L1608) function
allocates a new BDI, which sets up a new BDI directory with the name "bdi-\<number>".


{{< highlight c "linenos=table,linenostart=1604,hl_lines=9-10" >}}
/*
 * Setup private BDI for given superblock. I gets automatically cleaned up
 * in generic_shutdown_super().
 */
int super_setup_bdi(struct super_block *sb)
{
	static atomic_long_t bdi_seq = ATOMIC_LONG_INIT(0);

	return super_setup_bdi_name(sb, "%.28s-%ld", sb->s_type->name,
				    atomic_long_inc_return(&bdi_seq));
}
EXPORT_SYMBOL(super_setup_bdi);
{{< /highlight >}}

The BDI name is setup in the function [super_setup_bdi_name()](https://elixir.bootlin.com/linux/v6.1/source/fs/super.c#L1579)
which consists of the filename and a running number.
At the end of [super_setup_bdi_name()](https://elixir.bootlin.com/linux/v6.1/source/fs/super.c#L1579)
assigns the newly allocated BDI to the superblock.

{{< highlight c "linenos=table,linenostart=1575,hl_lines=11-21" >}}
/*
 * Setup private BDI for given superblock. It gets automatically cleaned up
 * in generic_shutdown_super().
 */
int super_setup_bdi_name(struct super_block *sb, char *fmt, ...)
{
	struct backing_dev_info *bdi;
	int err;
	va_list args;

	bdi = bdi_alloc(NUMA_NO_NODE);
	if (!bdi)
		return -ENOMEM;

	va_start(args, fmt);
	err = bdi_register_va(bdi, fmt, args);
	va_end(args);
	if (err) {
		bdi_put(bdi);
		return err;
	}
	WARN_ON(sb->s_bdi != &noop_backing_dev_info);
	sb->s_bdi = bdi;
	sb->s_iflags |= SB_I_PERSB_BDI;

	return 0;
}
EXPORT_SYMBOL(super_setup_bdi_name);
{{< /highlight >}}

In the write code path of the BTRFS filesystem, the [btrfs_buffered_write()](https://elixir.bootlin.com/linux/v6.1/source/fs/btrfs/file.c#L1483)
function gets eventually called to perform the buffered writes. This function invokes
[btrfs_write_check()](https://elixir.bootlin.com/linux/v6.1/source/fs/btrfs/file.c#L1433).
In [btrfs_write_check()](https://elixir.bootlin.com/linux/v6.1/source/fs/btrfs/file.c#L1433),
it sets the backing_dev_info pointer of the task struct.

{{< highlight c "linenos=table,linenostart=1575,hl_lines=22" >}}
static int btrfs_write_check(struct kiocb *iocb, struct iov_iter *from,
			     size_t count)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	loff_t pos = iocb->ki_pos;
	int ret;
	loff_t oldsize;
	loff_t start_pos;

	/*
	 * Quickly bail out on NOWAIT writes if we don't have the nodatacow or
	 * prealloc flags, as without those flags we always have to COW. We will
	 * later check if we can really COW into the target range (using
	 * can_nocow_extent() at btrfs_get_blocks_direct_write()).
	 */
	if ((iocb->ki_flags & IOCB_NOWAIT) &&
	    !(BTRFS_I(inode)->flags & (BTRFS_INODE_NODATACOW | BTRFS_INODE_PREALLOC)))
		return -EAGAIN;

	current->backing_dev_info = inode_to_bdi(inode);
	ret = file_remove_privs(file);
	if (ret)
		return ret;
{{< /highlight >}}


The write first goes to the page cache. Eventually the dirty buffers will get written
from the page cache with the function [balance_dirty_pages()](https://elixir.bootlin.com/linux/v6.1/source/mm/page-writeback.c#L1557).
When the function gets invoked it gets among other parameters a [bdi_writeback struct](https://elixir.bootlin.com/linux/v6.1/source/include/linux/backing-dev-defs.h#L105).
This contains a pointer to the above BDI object. To determine how much to write out
in one batch, [balance_dirty_pages()](https://elixir.bootlin.com/linux/v6.1/source/mm/page-writeback.c#L1557)
uses the information of the bdi object.

### Block device code path

For block devices the BDI structure gets allocated when [blk_alloc_disk()](https://elixir.bootlin.com/linux/v6.1/source/include/linux/blkdev.h#L814) is called.

{{< highlight c "linenos=table,linenostart=814,hl_lines=14" >}}
/**
 * blk_alloc_disk - allocate a gendisk structure
 * @node_id: numa node to allocate on
 *
 * Allocate and pre-initialize a gendisk structure for use with BIO based
 * drivers.
 *
 * Context: can sleep
 */
#define blk_alloc_disk(node_id)						\
({													\
	static struct lock_class_key __key;				\
													\
	__blk_alloc_disk(node_id, &__key);				\
})
{{< /highlight >}}

This function calls down [__blk_alloc_disk()](https://elixir.bootlin.com/linux/v6.1/source/block/genhd.c#L1410)

{{< highlight c "linenos=table,linenostart=1410,hl_lines=10-14" >}}
struct gendisk *__blk_alloc_disk(int node, struct lock_class_key *lkclass)
{
	struct request_queue *q;
	struct gendisk *disk;

	q = blk_alloc_queue(node, false);
	if (!q)
		return NULL;

	disk = __alloc_disk_node(q, node, lkclass);
	if (!disk) {
		blk_put_queue(q);
		return NULL;
	}
	set_bit(GD_OWNS_QUEUE, &disk->state);
	return disk;
}
EXPORT_SYMBOL(__blk_alloc_disk);
{{< /highlight >}}

and [alloc_disk_node()](https://elixir.bootlin.com/linux/v6.1/source/block/genhd.c#L1351)
to invoke [bdi_alloc()](https://elixir.bootlin.com/linux/v6.1/source/mm/backing-dev.c#L792).

{{< highlight c "linenos=table,linenostart=1351,hl_lines=13-15" >}}
struct gendisk *__alloc_disk_node(struct request_queue *q, int node_id,
		struct lock_class_key *lkclass)
{
	struct gendisk *disk;

	disk = kzalloc_node(sizeof(struct gendisk), GFP_KERNEL, node_id);
	if (!disk)
		return NULL;

	if (bioset_init(&disk->bio_split, BIO_POOL_SIZE, 0, 0))
		goto out_free_disk;

	disk->bdi = bdi_alloc(node_id);
	if (!disk->bdi)
		goto out_free_bioset;

	/* bdev_alloc() might need the queue, set before the first call */
	disk->queue = q;
...
{{< /highlight >}}

## Kernel tracing

When the writeback to the storage device has to wait, it can be traced. The function 
[balance_dirty_pages()](https://elixir.bootlin.com/linux/v6.1/source/mm/page-writeback.c#L1557) has
a tracepoint defined, that can be used for this purpose.

{{< highlight c "linenos=table,linenostart=1758,hl_lines=9-20" >}}
		/*
		 * For less than 1s think time (ext3/4 may block the dirtier
		 * for up to 800ms from time to time on 1-HDD; so does xfs,
		 * however at much less frequency), try to compensate it in
		 * future periods by updating the virtual time; otherwise just
		 * do a reset, as it may be a light dirtier.
		 */
		if (pause < min_pause) {
			trace_balance_dirty_pages(wb,
						  sdtc->thresh,
						  sdtc->bg_thresh,
						  sdtc->dirty,
						  sdtc->wb_thresh,
						  sdtc->wb_dirty,
						  dirty_ratelimit,
						  task_ratelimit,
						  pages_dirtied,
						  period,
						  min(pause, 0L),
						  start_time);
{{< /highlight >}}

It contains information about which BDI is used to do the write back.
The tracepoint is defined [here](https://elixir.bootlin.com/linux/v6.1/source/include/trace/events/writeback.h#L621).
``` shell
  > echo 1 > /sys/kernel/debug/tracing/tracing_on
  > echo 1 > /sys/kernel/debug/tracing/events/writeback/balance_dirty_pages/enable
  > cat /sys/kernel/debug/tracing/trace_pipe
```

Typical output for the tracepoint looks like this (the ouput has been reformated to make it
easier to read):

```Shell
  
fio-3093904 [044] ..... 1545451.315895: balance_dirty_pages: bdi 259:0: limit=245 setpoint=214 dirty=621
                                                             bdi_setpoint=0 bdi_dirty=2 dirty_ratelimit=249060
                                                             task_ratelimit=0 dirtied=0 dirtied_pause=0 paused=0
                                                             pause=3 period=3 think=2 cgroup_ino=7947
```
