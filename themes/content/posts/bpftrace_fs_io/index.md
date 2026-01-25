---
title: "Tracing filesystem reads and writes with bpftrace"
description: "Using Linux Kernel Samepage Merging"
date: 2023-10-04T22:36:40-07:00
featuredImage: "io.jpg"
toc:
  enable: true
tags: ["linux", "I/O", "bpftrace", "ebpf", "tracing"]
draft: false
---

This article describes how to get a breakdown of reads and writes by filesystem.
<!--more-->

## Overview
To analyze I/O behavior it can be useful to get a breakdown of the I/O activity
by filesystem. This allows to focus performance engagements on the filesystems
which have the most I/O or are the slowest.

## Breakdown by reads
To have a look at the number of reads per filesystem, the
[reads.bt](https://github.com/sroeschus/bpf-scripts/blob/main/vfs/reads.bt) script
can be used. Typical output of the script is shown below:

```shell
sudo ./reads.bt
Attaching 3 probes...
^C

@reads[tracefs]: 8192
@reads[pipefs]: 15765
@reads[anon_inodefs]: 20520
@reads[sockfs]: 42016
@reads[btrfs]: 319488
@reads[proc]: 359389
@reads[devtmpfs]: 520456
@reads[cgroup2]: 1474560
@reads[tmpfs]: 3538883

```
This shows there a considerable number of reads for the pseudo filesystems and
for the btrfs filesystem.

## Breakdown by writes
To have a look at the number of writes per filesystem, the
[writes.bt](https://github.com/sroeschus/bpf-scripts/blob/main/vfs/writes.bt) script
can be used. Typical output of the script is shown below:

```shell
sudo ./writes.bt
Attaching 3 probes...

@writes[sockfs]: 5528
@writes[pipefs]: 7821
@writes[anon_inodefs]: 24656
@writes[devtmpfs]: 49643
@writes[btrfs]: 469158
@writes[tmpfs]: 836356
```
This shows there a considerable number of writes for the pseudo filesystems and in
addition some writes for the btrfs filesystem.
