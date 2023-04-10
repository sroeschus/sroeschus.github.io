---
title: "Setting up the kernel development environment - VM"
description: How to setup email for your Linux kernel development environment
date: 2023-04-09T23:05:20-07:00
featuredImage: "vm.jpg"
draft: true
toc:
  enable: true
tags: ["kernel", "setup", "virtme", "qemu"]
series: [kernel-development-setup]
series_weight: 4
---

This article describes how to setup a virtual machine for your kernel development
environment.
<!--more-->

## Overview
For testing and debugging new kernel features a virtual machine is a very quick
way to evaluate and debug new kernel functionality. Compared to installing a new
Linux kernel on a test machine, booting the new kernel in the test machine, a virtual
machine is a lot quicker. Once the new kernel has been compiled, the kernel can
be started in a matter of seconds.

## BIOS Settings
The development machine needs to be setup to be able to run the virtualiztion software.
This setting is generally made in the BIOS of the computer. When the computer gets
started, the BIOS settings can be invoked by pressing the hotkey. The hotkey depends
on the manufacturer of the machine / BIOS. Generally its a key like F7 or F10. Its
best to consult your computer manual.

You can check on the machine if virtualization has been enabled with the following
command:

``` shell
$ cat /proc/cpuinfo

...
flags           : fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmxfxsr sse sse2 ss ht tm pbe
                  syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf
                  tsc_known_freq pni pclmulqdq dtes64 monitor ds_cpl vmx smx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2
                  x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single
                  ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust bmi1 avx2 smep bmi2
                  erms invpcid rdseed adx smap clflushopt clwb intel_pt sha_ni xsaveopt xsavec xgetbv1 xsaves split_lock_detect avx_vnni
                  dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp hwp_pkg_req hfi umip pku ospke waitpkg gfni vaes vpclmulqdq
                  tme rdpid movdiri movdir64b fsrm md_clear serialize pconfig arch_lbr ibt flush_l1d arch_capabilities
vmx flags       : vnmi preemption_timer posted_intr invvpid ept_x_only ept_ad ept_1gb flexpriority apicv tsc_offset vtpr mtf vapic ept vpid
                  unrestricted_guest vapic_reg vid ple shadow_vmcs ept_mode_based_exec tsc_scalingusr_wait_pause
...
```

If virtualization is enabled for the CPU, it should contain the value vmx or svm in the flags display.

## Installation
This guide uses QEMU to start a kernel. The software can virtualize different architectures. In this guide
we assume we want to execute on a virtualize x86_64 architecture. The software can be installed with the
following command:

```shell
sudo pacman -S qemu-system-x86
sudo pacman -S python-setuptools
```

This will install the exeutable qemu-system-x86_64 and some additional python libraries which are required
in the next step. qemu can be used directly, but this requires to setup a complete system. An easier way is
to use the development system, but start it virtualized with a new kernel. To do this we need to install the
virtme software.

```shell
git clone https://github.com/arighi/virtme.git
sudo ./setup.py install
```

The software was originally developed by https://github.com/amluto/virtme.git. However the software doesn't
seem to get updated anymore. This causes problems with newer versions of the qemu software as some flags /
options have been renamed. A fork that gets updated is maintained here https://github.com/arighi/virtme.git.
This fork also works with newer versions. This is especially important if qemu is newer than 7.2

## Preparing the kernel
To be able to start the kernel in a virtualized environment, it needs to be configured for it. This is
described in the above github repository. The easiest way is to execute the following command in the
kernel repository:

```shell
virtme-configkernel --arch=x86 --defconfig
```

Afterwards recompile the kernel with the new configuration.

## Start the kernel in the virtualized environment
From the kernel repository can now be started with a command like the following:

```shell
virtme-run --kimg arch/x86_64/boot/bzImage

...
[    1.248605] clocksource: tsc: mask: 0xffffffffffffffff max_cycles: 0x1e7161bdfcf, max_idle_ns: 440795233580ns
[    1.249049] clocksource: Switched to clocksource tsc
[    2.139882] virtme-init: udev is done
[    2.213672] ip (129) used greatest stack depth: 12424 bytes left
virtme-init: console is ttyS0
[root@(none) /]#
```

After a couple of seconds this greets you with root prompt in the virtualized kernel. The virtualized kernel
environment can be stopped by pressing Ctrl-A X.

