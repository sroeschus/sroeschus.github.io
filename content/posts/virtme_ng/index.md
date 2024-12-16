---
title: "How to use virtme-ng for Linux Kernel development"
description: "Using virtme-ng for Linux Kernel development"
date: 2024-12-14T21:50:04-07:00
featuredImage: "virtme-ng.png"
toc:
  enable: true
tags: ["linux", "kernel", "testing", "virtme", "virtme-ng", "qemu", "arch"]
draft: false
---

This article describes how to use virtme-ng for Linux kernel development.
<!--more-->

## Overview
Being able to quickly start a virtualized Linux Kernel on your development
machine is very valueable. It can speed up the development time considerably.

## Previous solutions
In the past there has been [virtme](https://github.com/amluto/virtme), which
has orginally been developed by Andrew Lutomirski. However this github repository
is no longer maintained and no longer works with current kernels.

The repository has been [forked](https://github.com/arighi/virtme) by Andea Righi.
However this fork is also no longer maintained. Andrea has created
[virtme-ng](https://github.com/arighi/virtme-ng) for all new development.

## Installation from binary
Until a few months ago only installation from source was possible. However Arch
Linux, Debian and Ubuntu now offer packages. This makes the installation easier.

On Arch Linux the package can now be installed with:
```shell
sudo pacman -S virtme-ng
```

## Installation from source
The github repository does a good job for explaining how to install the software.
However for Arch Linux a few additional steps are required. Additional packages
are needed

```shell
sudo pacman -S virtiofsd
sudo pacman -S busybox
sudo pacman -S mkinitcpio-busybox
```
Install the repository:
```shell
git clone --recurse-submodules https://github.com/arighi/virtme-ng.git
```

On modern python installations the software can't be installed as described. Instead
an additional parameter needs to be specified:


```shell
cd <virtme-ng repository>
BUILD_VIRTME_NG_INIT=1 pip3 install --break-system-packages --verbose -r requirements.txt .
```

This installs the binaries under $HOME/.local/bin. Its best to exit session and start
a new session so the new binaries are added to the path.

## Usage
The github page describes its usage very well.

### Build
Switch to your linux kernel tree and build a new kernel with:

```shell
vng --build
```

### Run
Now you are able to run it:

```shell
vng --run
```

The result is a booted kernel:

```shell

╭─shr@shr in repo: linux on  shr/rel-v6.9 [$?] took 1m52s
╰─λ vng
_      _
__   _(_)_ __| |_ _ __ ___   ___       _ __   __ _
\ \ / / |  __| __|  _   _ \ / _ \_____|  _ \ / _  |
 \ V /| | |  | |_| | | | | |  __/_____| | | | (_| |
  \_/ |_|_|   \__|_| |_| |_|\___|     |_| |_|\__  |
                                             |___/
kernel version: 6.9.0 x86_64
(CTRL+d to exit)

```

### Run with boot parameters
Kernel boot parameters can be specified with -a (append) flag. The following example
specifies log_level boot parameter

```shell
╭─shr@shr in repo: linux on  shr/rel-v6.9 [$] took 1ms
╰─λ vng -a loglevel=2 -v
virtme: waiting for virtiofsd to start
virtme: use 'microvm' QEMU architecture
early console in setup code
Probing EDD (edd=off to disable)... ok
No EFI environment detected.
early console in extract_kernel
input_data: 0x0000000003b192b9
input_len: 0x0000000000a36ce0
output: 0x0000000001000000
output_len: 0x00000000034fac08
kernel_total_size: 0x0000000003426000
needed_size: 0x0000000003600000
trampoline_32bit: 0x0000000000000000

Decompressing Linux... Parsing ELF... done.
Booting the kernel (entry_offset: 0x0000000000000000).
[    0.000000] Linux version 6.9.0 (shr@shr-linux) (gcc (GCC) 14.2.1 20240805, GNU ld (GNU Binutils) 2.43.0) #7 SMP PREEMPT_DYNAMIC Mon Sep  2 11:24:53 PDT 2024
[    0.000000] Command line: virtme_hostname=virtme-ng nr_open=1073741816 virtme_link_mods=/home/shr/linux/.virtme_mods/lib/modules/0.0.0 virtme_rw_overlay0=/etc virtme_rw_overlay1=/lib virtme_rw_overlay2=/home virtme_rw_overlay3=/opt virtme_rw_overlay4=/srv virtme_rw_overlay5=/usr virtme_rw_overlay6=/var virtme_rw_overlay7=/tmpconsole=hvc0 earlyprintk=serial,ttyS0,115200 virtme_console=ttyS0 psmouse.proto=exps "virtme_stty_con=rows 98cols 110 iutf8" TERM=xterm-256color virtme_chdir=home/shr/linux virtme_user=shr debug loglevel=2 init=/home/shr/.local/lib/python3.12/site-packages/virtme/guest/bin/virtme-ng-init
[    0.000000] BIOS-provided physical RAM map:
[    0.000000] BIOS-e820: [mem 0x0000000000000000-0x000000000009fbff] usable
[    0.000000] BIOS-e820: [mem 0x000000000009fc00-0x000000000009ffff] reserved
[    0.000000] BIOS-e820: [mem 0x00000000000f0000-0x00000000000fffff] reserved
[    0.000000] BIOS-e820: [mem 0x0000000000100000-0x000000003fffdfff] usable
[    0.000000] BIOS-e820: [mem 0x000000003fffe000-0x000000003fffffff] reserved
[    0.000000] BIOS-e820: [mem 0x00000000feffc000-0x00000000feffffff] reserved
[    0.000000] BIOS-e820: [mem 0x00000000fffc0000-0x00000000ffffffff] reserved
[    0.000000] printk: legacy bootconsole [earlyser0] enabled
_      _
__   _(_)_ __| |_ _ __ ___   ___       _ __   __ _
\ \ / / |  __| __|  _   _ \ / _ \_____|  _ \ / _  |
\ V /| | |  | |_| | | | | |  __/_____| | | | (_| |
\_/ |_|_|   \__|_| |_| |_|\___|     |_| |_|\__  |
|___/
kernel version: 6.9.0 x86_64
(CTRL+d to exit)


╭─shr@virtme in repo: linux on  shr/rel-v6.9 [$] as 🧙 took 15ms
╰─λ cat /proc/cmdline
virtme_hostname=virtme-ng nr_open=1073741816 virtme_link_mods=/home/shr/linux/.virtme_mods/lib/modules/0.0.0 virtme_rw_overlay0=/etc virtme_rw_overlay1=/lib virtme_rw_overlay2=/home virtme_rw_overlay3=/opt virtme_rw_overlay4=/srv virtme_rw_overlay5=/usr virtme_rw_overlay6=/var virtme_rw_overlay7=/tmp console=hvc0 earlyprintk=serial,ttyS0,115200 virtme_console=ttyS0 psmouse.proto=exps "virtme_stty_con=rows 98 cols 110 iutf8" TERM=xterm-256color virtme_chdir=home/shr/linux virtme_user=shr debug loglevel=2 init=/home/shr/.local/lib/python3.12/site-packages/virtme/guest/bin/virtme-ng-init
```
From the dump boot command line parameters it is clear that the log_level boot parameter
has been set to 2.

This can also be verified with the following command:

```shell
╭─shr@virtme in repo: linux on  shr/rel-v6.9 [$] as 🧙 took 19ms
╰─λ cat /proc/sys/kernel/printk
2       4       1       7
```

## Differences
Notable differences to virtme:
- Booting the kernel is very fast
- Kernel log is not displayed by default

To display the kernel log, the kernel can be started with:

```shell
vng -v
```
