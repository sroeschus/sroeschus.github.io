---
title: "Link error: undefined reference to __divdi3"
description: "How to solve link error: undefined reference to __divdi3"
date: 2023-12-31T13:57:11-08:00
featuredImage: "error.jpg"
toc:
  enable: false
tags: ["linux", "linker", "error", "__divdi3", "__udivdi3", "32bit"]
draft: false 
---

This article describes how to solve the linker error "undefined referenc to __divdi3".
<!--more-->

## Overview
Once a patch or a patch series has been upstreamed, it is run on the Intel test farm. The
test farm runs tests with the new patch. This also includes running the new kernel on different
platforms. This is generally a mix of 32 and 64 bit platforms. Before running the patch
on the different platforms, it is build. During the build it is possible that errors like the
one below is raised:

```
All errors (new ones prefixed by >>):

   ld: mm/ksm.o: in function `scan_get_next_rmap_item':
ksm.c:(.text+0x3e45): undefined reference to `__divdi3'
ld: ksm.c:(.text+0x3e88): undefined reference to `__divdi3'
   ld: ksm.c:(.text+0x3ed9): undefined reference to `__divdi3'
```
The error is caused by a division of the long long data type on 32 bit platforms. 32 bit platforms
do not support the division of the long long datatype.

By replacing the division with the `div_s64` function, the problem can be adressed. Similarly link
errors that reference the `__udivdi3` function can be handled.
