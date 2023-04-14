---
title: "Compiling older Linux Kernels can fail with load BTF from vmlinux: invalid argument"
description: "How to fix the load BTF from vmlinux: invalid argument when compiling older Linux kernels"
date: 2023-04-13T22:20:33-07:00
featuredImage: "hole.jpg"
toc:
  enable: false
tags: ["linux", "kernel", "compile", "pahole", "vmlinux", "BTF"]
draft: false
---

This article describes how to solve the error load BTF from vmlinux: invalid argument
when compiling older kernels.
<!--more-->

## Problem
As a kernel developer it is not uncommon to bisect in which kernel release and
with which git commit a problem first occured. However when compiling 5.x kernels
with the current version of the build environment, the following error can happen:
```shell
  KSYMS   .tmp_vmlinux.kallsyms1.S
  AS      .tmp_vmlinux.kallsyms1.S
  LD      .tmp_vmlinux.kallsyms2
  KSYMS   .tmp_vmlinux.kallsyms2.S
  AS      .tmp_vmlinux.kallsyms2.S
  LD      vmlinux
  BTFIDS  vmlinux
FAILED: load BTF from vmlinux: Invalid argument
make: *** [Makefile:1187: vmlinux] Error 255
make: *** Deleting file 'vmlinux'
```
It turns out the problem is caused `CONFIG_DEBUG_INFO_BTF` configuration option and
when the pahole utility has been updated to 1.24. If the version of the pahole
utility is less than 1.24, the build is successful.
The new pahole (version 1.24) generates by default the new `BTF_KIND_ENUM64` BTF tag,
which is not supported by stable kernel.

## Solution
There are three ways to address the problem.

### Disable the configuration setting
By disabling the `CONFIG_DEBUG_INFO_BTF` configuration option, the kernel will build
successfully. However this has a downside, as BPF programs cannot be run.

### Downgrade the pahole package
The `pahole` program is part of the [pahole](https://archlinux.org/packages/extra/x86_64/pahole/)
package. If your Linux distribution still provides an earlier version, it is the easiest
to downgrade to the previous version.

### Patch pahole-flags.sh
The new pahole provides the `--skip_encoding_btf_enum64` option to skip `BTF_KIND_ENUM64`
generation. By specifying this option, a stable kernel will be created.

Applying the following patch to [scripts/pahole-flags.sh](https://elixir.bootlin.com/linux/v5.19/source/scripts/pahole-flags.sh)
will achieve enabling the new flag.
  
  
```patch
--- a/scripts/pahole-flags.sh
+++ b/scripts/pahole-flags.sh
@@ -14,4 +14,8 @@ if [ "${pahole_ver}" -ge "118" ] && [ "$
 	extra_paholeopt="${extra_paholeopt} --skip_encoding_btf_vars"
 fi
 
+if [ "${pahole_ver}" -ge "124" ]; then
+	extra_paholeopt="${extra_paholeopt} --skip_encoding_btf_enum64"
+fi
+
 echo ${extra_paholeopt}
```
