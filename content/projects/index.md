---
title: "Projects"
date: 2022-12-24T22:29:52-08:00
tags: ["stefan", "roesch", "projects"]
draft: false
---

For the reader who wants to get an overview over the patches that I have submitted
[upstream](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/log/?qt=grep&q=stefan+roesch) (be patient, loading git.kernel.org takes a while).

# List of patch series
The following is a list of the more important patch series that I developed and that have been upstreamed.
The list is ordered in chronological order. After the title it contains the linux release when the most current version
of the patch series was submitted and which subsystems were part of the patch series. The subsystem names are
abbreviated with the commmon short names from the kernel directory names:
- block = block layer
- btrfs
- fs = filesystem layer
- iomap
- io-uring
- mm = memory management
- nvme
- xfs

## Backing device information changes
{{< version 6.2 >}} `mm`

This patch provides new knobs to influence the behavior of the size and management of the
dirty pagecache. In addition it also changes the internal calculation to use part of 1000000
instead of percentage values. 

[Cover letter](https://lore.kernel.org/all/20221119005215.3052436-1-shr@devkernel.io/T/#u)

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=8e9d5ead865a1a7af74a444d2f00f1ef4539bfba), [P2](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=27bbe9d48d4e298864e18b39f091342c68b81637), [P3](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=16b837eb84e6948f92411eb32e97a05f89733ddc), [P4](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=ae82291e9ca47c3d6da6b77a00f427754aca413e), [P5](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=00df7d51263b46ed93f7572e2d09579746f7b1eb), [P6](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=efc3e6ad53ea14225b434fddca261c9a1c56c707), [P7](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=1bf27e98d26d1e62166a456ef17460be085cbe0b), [P8](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=c56e049a5e401a177c7c9b39a3bcc973ff5cec0b), [P9](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=c354d9268d7825eb8643f658c5091079d4f11a4a), [P10](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=712c00d66a342a3ed375df41c3df7d3d2abad2c0), [P11](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=8021fb3232f265b81c7e4e7aba15bc3a04ff1fd3), [P12](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=803c98050569850be5fd51a2025c67622de887d9), [P13](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=9c84819bd64ec15cb15d041c45ebe4725e9d4f3b), [P14](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=9c832a8d571784c998d0f9f5df480c62f7f3064c), [P15](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=4e230b406eda9bdf7f8a71e2cc3df18a824abcb0), [P16](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=bca52dcbadc583f4db6435599c44a79f97293f06), [P17](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=54790f30fea74247e2f38b4a632ee3dc2fe42d86), [P18](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=2c44af4f2aaa260199f218f11920c406e688693c), [P19](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=ad3e6dabf6f7d9ffd68eb711191ef16cdbdd25f0), [P20](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=eba39236f18da7a50b6c51df5d902ee72c43e760)

## Enable batched completions of passthrough I/O
{{< version 6.2 >}} `mm` `block` 

The filesystem IO path can take advantage of allocating batches of
requests, if the underlying submitter tells the block layer about it
through the blk_plug. For passthrough IO, the exported API is the
blk_mq_alloc_request() helper, and that one does not allow for
request caching.

I developed and implemented the prototype of the feature and worked with
Jens Axboe on upstreaming this patch series.

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=ab3e1d3bbab9e973aeb4dd4603251578658a47ff), [P2](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=c0a7ba77e81b8440d10f38559a5e1d219ff7e87c), [P3](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=851eb780decb7180bcf09fad0035cba9aae669df)

## Support async buffered writes for io-uring on btrfs
{{< version 6.1 >}} `mm` `btrfs` 

This patch series adds support for async buffered writes when using both
btrfs and io-uring. Currently io-uring only supports buffered writes (for btrfs)
in the slow path, by processing them in the io workers. With this patch series
it is now possible to support buffered writes in the fast path. To be able to use
the fast path, the required pages must be in the page cache, the required locks
in btrfs can be granted immediately and no additional blocks need to be read
form disk.

This patch series makes use of the changes that have been introduced by a
previous patch series: "io-uring/xfs: support async buffered writes"

#### Performance results

The new patch improves throughput by over two times (compared to the exiting
behavior, where buffered writes are processed by an io-worker process) and also
the latency is considerably reduced. Detailled results are part of the changelog
of the first commit.

This patch series is based on the earlier patch series, that provided async buffered writes for XFS.

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=611df5d6616d80a22906c352ccd80c395982fbd9), [P2](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=857bc13f857aea957ae038b2b43c28560976024a), [P3](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=26ce91144631a402ff82c93358d8880326a7caa3), [P4](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=1daedb1d6bf24c7185e00cd341404f262f8de7c8), [P5](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=d2c7a19f5c82ace6ea0ec00ae53c6dd97ee8e274), [P6](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=80f9d24130e45b01984a918d6b2006c10687b138), [P7](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=fc2260001232766c1836d5a6053913194ce23f88), [P8](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=2fcab928ccc2bac0c22e3b3b04f5acf0dc8de96b), [P9](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=304e45acdb8f68a974e8a64a6296803530bb851f), [P10](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=965f47aeb5deddc59a1ace28e99b2a578df57305), [P11](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=c922b016f353eafb69997d8c0f06efdf945315ce), [P12](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=926078b21db91b72b444277fdc2166914cf113fc)

The patch series has been disccussed on [phoronix](https://www.phoronix.com/news/Btrfs-Async-Buffered-Writes-6.1), 
[zetabizu](https://zetabizu.com/btrfs-brings-some-great-performance-improvements-with-linux-6-1/), 
[cosfone](https://www.cosfone.com/linux-6-1-welcomes-btrfs-asynchronous-buffer-write-patch-double-throughput/).

## btrfs: sysfs: set / query btrfs chunk size
{{< version 6.0 >}} `btrfs` 

The btrfs allocator is currently not ideal for all workloads. It tends
to suffer from overallocating data block groups and underallocating
metadata block groups. This results in filesystems becoming read-only
even though there is plenty of "free" space.

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=f6fca3917b4d99d8c13901738afec35f570a3c2f), [P2](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=19fc516a516f624fa3b0c329929561186247537e), [P3](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=22c55e3bbb20c60846812ea2b8ea0f3153c0df73)

## Support async buffered writes on XFS for io-uring
{{< version 6.0 >}} `mm` `iomap` `fs` `xfs` `io-uring`

This patch series adds support for async buffered writes when using both
xfs and io-uring. Currently io-uring only supports buffered writes in the
slow path, by processing them in the io workers. With this patch series it is
now possible to support buffered writes in the fast path. To be able to use
the fast path the required pages must be in the page cache, the required locks
in xfs can be granted immediately and no additional blocks need to be read
form disk.

Updating the inode can take time. An optimization has been implemented for
the time update. Time updates will be processed in the slow path. While there
is already a time update in process, other write requests for the same file,
can skip the update of the modification time.

#### Performance results
For fio the following results have been obtained with a queue depth of
1 and 4k block size for sequential writes (runtime 600 secs):

| Metric       | without patch       |   with patch   |  libaio   |  psync     |
| :---         |             ---:    |         ---:   |      ---: |       ---: |
| iops:        |     77k             |      209k      |   195K    |  233K      |
| bw:          |    314MB/s          |      854MB/s   |   790MB/s |  953MB/s   |
| clat:        |   9600ns            |      120ns     |   540ns   | 3000ns     |


For an io depth of 1, the new patch improves throughput by over three times
(compared to the exiting behavior, where buffered writes are processed by an
io-worker process) and also the latency is considerably reduced. To achieve the
same or better performance with the exisiting code an io depth of 4 is required.
Increasing the iodepth further does not lead to improvements.

In addition the latency of buffered write operations is reduced considerably.

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=ea6813be07dcdc072aa9ad18099115a74cecb5e1), [P2](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=e92eebbb09218e128e559cf12b65317721309324), [P3](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=fe6c9c6e3e3e332b998393d214fba9d09ab0acb0), [P4](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=9753b868fda48330ce358df203c0069ac0788ac0), [P5](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=cae2de6978915991a564e3c5c69b66b629c031af), [P6](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=18e419f6e80a6d3c8aaab94abd55c3b41741d8df), [P7](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=8017553980d0bbfef3e66c583363828565afd6da), [P8](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=faf99b563558f74188b7ca34faae1c1da49a7261), [P9](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=6a2aa5d85de534471dd023773236f113eaef26f0), [P10](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=66fa3cedf16abc82d19b943e3289c82e685419d5), [P11](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=4e17aaab54359fa2cdeb0080c822a08f2980f979), [P12](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=1c849b481b3e4f8c36f297cd3aa88ef52a19cee9), [P13](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=9641506b2deed1bb6be7464a95d62c472eca0e8e), [P14](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=1aa91d9c993397858a50c433933ea119903fdea2)

The patch series has been disccussed on [phoronix](https://www.phoronix.com/news/Linux-520-XFS-uring-Async-Buff), 
[twitter](https://twitter.com/axboe/status/1539739564691947520), [Kernel Recipes presentation](https://kernel.dk/axboe-kr2022.pdf).

## Add large CQE support for io-uring (April 2022, io-uring)
{{< version 5.19 >}} `io-uring`

This adds the large CQE support for io-uring. Large CQE's are 16 bytes longer.
To support the longer CQE's the allocation part is changed and when the CQE is
accessed.
The allocation of the large CQE's is twice as big, so the allocation size is
doubled. The ring size calculation needs to take this into account.

All accesses to the large CQE's need to be shifted by 1 to take the bigger size
of each CQE into account. The existing index manipulation does not need to be
changed and can stay the same.

The setup and the completion processing needs to take the new fields into
account and initialize them. For the completion processing these fields need
to be passed through.

The flush completion processing needs to fill the additional CQE32 fields.

The code for overflows needs to be adapted accordingly: the allocation needs to
take large CQE's into account. This means that the order of the fields in the io
overflow structure needs to be changed and the allocation needs to be enlarged
for big CQE's.
In addition the two new fields need to be copied for large CQE's.

The new fields are added to the tracing statements, so the extra1 and extra2
fields are exposed in tracing. The new fields are also exposed in the /proc
filesystem entry.

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=7a51e5b44b92686eebd3e1b46b86e1eb4db975db), [P2](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=4e5bc0a9a1d0be5b20a0366fbfbe5a99d61c6003), [P3](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=baf9cb643b485d57c404b0ea9c1865036dde9eb7), [P4](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=916587984facd01a2f4a2e327d721601a94ed1ed), [P5](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=effcf8bdeb03aa726e9db834325c650e1700b041), [P6](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=2fee6bc6407848043798698116b8fd81d1fe470a), [P7](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=0e2e5c47fed68ce203f2c6978188cc49a2a96e26), [P8](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=e45a3e05008d52c6db63a3a01a9cdc7d89cd133a), [P9](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=c4bb964fa092fb68645a852365dfe9855fef178a), [P10](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=f9b3dfcc68a502ef82e50274e2e7e9e91f6bf4e2), [P11](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=76c68fbf1a1f97afed0c8f680ee4e3f4da3e720d), [P12](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=2bb04df7c2af9dad5d28771c723bc39b01cf7df4)

## Add xattr support to io-uring
{{< version 5.18 >}} `fs` `io-uring`

This adds the xattr support to io_uring. The intent is to have a more
complete support for file operations in io_uring.

This change adds support for the following functions to io_uring:
- fgetxattr
- fsetxattr
- getxattr
- setxattr

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=1a91794ce8481a293c5ef432feb440aee1455619), [P2](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=c975cad931570004b5f51248424a2a696fb65630), [P3](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=e9621e2bec80fe63f677a759066a5089b292f43a), [P4](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=a56834e0fafe0adf7f22a28a5dbec3e8c3031a0e)

## Make statx api for io-uring stable (Feb 2022, fs, io-uring)
{{< version 5.18 >}} `fs` `io-uring`

One of the key architectual tenets of io-uring is to keep the
parameters for io-uring stable. After the call has been submitted,
its value can be changed.  Unfortunaltely this is not the case for
the current statx implementation.

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=1b6fe6e0dfecf8c82a64fb87148ad9333fa2f62e)

## Make io-uring tracepoints consistent (Feb 2022, io-uring)
{{< version 5.17 >}} `io-uring`

This makes the io-uring tracepoints consistent. Where it makes sense
the tracepoints start with the following four fields:
- context (ring)
- request
- user_data
- opcode.

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=502c87d65564cbfd65b1621309bcd900390eca81)

