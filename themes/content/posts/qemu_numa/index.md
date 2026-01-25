---
title: "Configure virtme/qemu with Numa support"
description: "How to start qemu/virtme with Numa support"
date: 2023-12-05T11:44:49-08:00
featuredImage: "numa.jpg"
draft: false
toc:
  enable: false
tags: ["linux", "memory", "numa", "qemu", "virtme"]
---

This article describes how to start qemu / virtme with Numa support
<!--more-->

## Overview
When developing a new kernel feature or debugging a kernel functionality, it
is sometimes necessary to have access to a NUMA machine. This is not always possible.
An alternative to a physical NUMA machine is using qemu or the combination of qemu and
virtme.

## Starting virtme with NUMA support
To start virtme with NUMA support enabled, the following command can be
used:

```shell
virtme-run --kimg arch/x86_64/boot/bzImage                                    \
   --qemu-opts -m 8G -smp 4 -object memory-backend-ram,size=4G,id=m0          \
   -object memory-backend-ram,size=4G,id=m1                                   \
   -numa node,memdev=m0,cpus=0-1,nodeid=0                                     \
   -numa node,memdev=m1,cpus=2-3,nodeid=1
```
This assumes that the virtual machines is started with 8GB of memory and 4
CPU's. It then creates 2 NUMA nodes each with 2 CPU's and 4GB of memory. Other
combinations are certainly possible by changing the corresponding parameters.
