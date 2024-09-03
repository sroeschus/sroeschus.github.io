---
title: "How to use virtme-ng for Linux Kernel development"
description: "Using virtme-ng for Linux Kernel development"
date: 2024-09-02T21:50:04-07:00
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

## Installation
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

## Differences
Notable differences to virtme:
- Booting the kernel is very fast
- Kernel log is not displayed by default

To display the kernel log, the kernel can be started with:

```shell
vng -v
```
