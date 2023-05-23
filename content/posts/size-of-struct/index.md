---
title: "How to query the size of a struct"
description: How to query the size of a struct and get good recommendation on packing 
date: 2023-05-22T21:29:42-07:00
featuredImage: "size.jpg"
toc:
  enable: true
tags: ["linux", "memory", "struct", "size", "packing"]
draft: false
---

This article describes how to query the size of c structs and how to optimize 
their size by changing the field order.
<!--more-->

## Overview
For debugging purposes it can be useful to determine how much space a c struct needs.
In addition that information can also be very valuable in optimizing the size of c structs.
Understanding the storage allocation of the struct can also be important for cache
alignment and to understand false sharing scenarios. There are two different ways to
determine the data structure layout.

## GDB ptype command
With the [ptype](https://sourceware.org/gdb/onlinedocs/gdb/Symbols.html) command GDB can show the storage layout of a c struct. The following output
shows the layout of the file struct.

```c
(gdb) ptype struct file
type = struct file {
  union {
    struct llist_node f_llist;
    struct callback_head f_rcuhead;
    unsigned int f_iocb_flags;
  };
  struct path f_path;
  struct inode *f_inode;
  const struct file_operations *f_op;
  spinlock_t f_lock;
  atomic_long_t f_count;
  unsigned int f_flags;
  fmode_t f_mode;
  struct mutex f_pos_lock;
  loff_t f_pos;
  struct fown_struct f_owner;
  const struct cred *f_cred;
  struct file_ra_state f_ra;
  u64 f_version;
  void *f_security;
  void *private_data;
  struct hlist_head *f_ep;
  struct address_space *f_mapping;
  errseq_t f_wb_err;
  errseq_t f_sb_err;
}
```

The `ptype` command can also print the struct offsets.
```c
(gdb) ptype /o struct file
/* offset      |    size */  type = struct file {
/*      0      |      16 */    union {
/*                     8 */        struct llist_node {
/*      0      |       8 */            struct llist_node *next;

/* total size (bytes):    8 */
} f_llist;
/*                    16 */        struct callback_head {
/*      0      |       8 */            struct callback_head *next;
/*      8      |       8 */            void (*func)(struct callback_head *);

/* total size (bytes):   16 */
} f_rcuhead;
/*                     4 */        unsigned int f_iocb_flags;

/* total size (bytes):   16 */
};
/*     16      |      16 */    struct path {
/*     16      |       8 */        struct vfsmount *mnt;
/*     24      |       8 */        struct dentry *dentry;

/* total size (bytes):   16 */
} f_path;
/*     32      |       8 */    struct inode *f_inode;
/*     40      |       8 */    const struct file_operations *f_op;
/*     48      |       4 */    spinlock_t f_lock;
/* XXX  4-byte hole      */
/*     56      |       8 */    atomic_long_t f_count;
/*     64      |       4 */    unsigned int f_flags;
/*     68      |       4 */    fmode_t f_mode;
/*     72      |      32 */    struct mutex {
/*     72      |       8 */        atomic_long_t owner;
/*     80      |       4 */        raw_spinlock_t wait_lock;
/*     84      |       4 */        struct optimistic_spin_queue {
/*     84      |       4 */            atomic_t tail;

/* total size (bytes):    4 */
} osq;
/*     88      |      16 */        struct list_head {
/*     88      |       8 */            struct list_head *next;
/*     96      |       8 */            struct list_head *prev;

/* total size (bytes):   16 */
} wait_list;

/* total size (bytes):   32 */
} f_pos_lock;
/*    104      |       8 */    loff_t f_pos;
/*    112      |      32 */    struct fown_struct {
/*    112      |       8 */        rwlock_t lock;
/*    120      |       8 */        struct pid *pid;
/*    128      |       4 */        enum pid_type pid_type;
/*    132      |       4 */        kuid_t uid;
/*    136      |       4 */        kuid_t euid;
/*    140      |       4 */        int signum;

/* total size (bytes):   32 */
} f_owner;
/*    144      |       8 */    const struct cred *f_cred;
/*    152      |      32 */    struct file_ra_state {
/*    152      |       8 */        unsigned long start;
/*    160      |       4 */        unsigned int size;
/*    164      |       4 */        unsigned int async_size;
/*    168      |       4 */        unsigned int ra_pages;
/*    172      |       4 */        unsigned int mmap_miss;
/*    176      |       8 */        loff_t prev_pos;

/* total size (bytes):   32 */
} f_ra;
/*    184      |       8 */    u64 f_version;
/*    192      |       8 */    void *f_security;
/*    200      |       8 */    void *private_data;
/*    208      |       8 */    struct hlist_head *f_ep;
/*    216      |       8 */    struct address_space *f_mapping;
/*    224      |       4 */    errseq_t f_wb_err;
/*    228      |       4 */    errseq_t f_sb_err;

/* total size (bytes):  232 */
}
```


## pahole command
The [pahole](https://man.archlinux.org/man/extra/pahole/pahole.1.en) command can be used from the command line. It displays the
data layout of c structs. It also shows the holes in the storage layout.

```c
> pahole file

struct file {
  union {
    struct llist_node  f_llist;              /*     0     8 */
    struct callback_head f_rcuhead;          /*     0    16 */
    unsigned int       f_iocb_flags;         /*     0     4 */
  };                                               /*     0    16 */
  struct path                f_path;               /*    16    16 */
  struct inode *             f_inode;              /*    32     8 */
  const struct file_operations  * f_op;            /*    40     8 */
  spinlock_t                 f_lock;               /*    48     4 */

  /* XXX 4 bytes hole, try to pack */

  atomic_long_t              f_count;              /*    56     8 */
  /* --- cacheline 1 boundary (64 bytes) --- */
  unsigned int               f_flags;              /*    64     4 */
  fmode_t                    f_mode;               /*    68     4 */
  struct mutex               f_pos_lock;           /*    72    32 */
  loff_t                     f_pos;                /*   104     8 */
  struct fown_struct         f_owner;              /*   112    32 */
  /* --- cacheline 2 boundary (128 bytes) was 16 bytes ago --- */
  const struct cred  *       f_cred;               /*   144     8 */
  struct file_ra_state       f_ra;                 /*   152    32 */
  u64                        f_version;            /*   184     8 */
  /* --- cacheline 3 boundary (192 bytes) --- */
  void *                     f_security;           /*   192     8 */
  void *                     private_data;         /*   200     8 */
  struct hlist_head *        f_ep;                 /*   208     8 */
  struct address_space *     f_mapping;            /*   216     8 */
  errseq_t                   f_wb_err;             /*   224     4 */
  errseq_t                   f_sb_err;             /*   228     4 */

  /* size: 232, cachelines: 4, members: 20 */
  /* sum members: 228, holes: 1, sum holes: 4 */
  /* last cacheline: 40 bytes */
};
```

With the `-R` switch pahole gives recommendations on how to reorganize the c struct to be
more space efficient.
