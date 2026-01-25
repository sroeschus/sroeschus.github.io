---
title: "Setting up the kernel development environment - Basic"
description: "How to setup the basic Linux kernel development environment"
date: 2023-01-24T20:23:05-08:00
featuredImage: "setup.jpg"
draft: false
toc:
  enable: true
tags: ["kernel", "setup", "pacman"]
series: [kernel-development-setup]
series_weight: 1
---

This article describes how to do the basic setup for your kernel development
environment.
<!--more-->

## Overview
Kernel development requires a number of Linux packages to be installed. This series documents
the setup process for an arch-based linux system. The general requirements are defined in
[minimal requirements to compile the kernel](https://www.kernel.org/doc/html/v4.13/process/changes.html).

## Installing the base packages
To install the base packages execute the following commands:
``` shell
sudo pacman -y -S neovim
sudo pacman -y -S git
sudo pacman -y -S gcc
sudo pacman -y -S clang
sudo pacman -y -S make
sudo pacman -y -S binutils
sudo pacman -y -S util-linux
sudo pacman -y -S module-init-tools
sudo pacman -y -S e2fsprogs
sudo pacman -y -S jfsutils
sudo pacman -y -S xfsprogs
sudo pacman -y -S btrfs-progs
sudo pacman -y -S procps
sudo pacman -y -S bc
sudo pacman -y -S bison
sudo pacman -y -S flex
sudo pacman -y -S quota-tools
sudo pacman -y -S grub
sudo pacman -y -S ncurses
sudo pacman -y -S openssl
sudo pacman -y -S cscope
sudo pacman -y -S ctags
sudo pacman -y -S gdb
```

## Cloning the kernel source
The next step is to get the current kernel source to the development machine. There
are several ways to do this, but the best way is to use git clone. Using git clone
we can fetch the next tree or one of the other development trees. In this example the
next tree is cloned. Cloning the next tree is described [here](https://kernel.org/doc/man-pages/linux-next.html).

```shell
git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
cd linux
```
Add the remote so we can fetch the changes and the new tags.
```shell
git remote add linux-next https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git
git fetch linux-next
git fetch --tags linux-next
git checkout master
git remote update
```
Query which branches are available to be checked out.
```shell
git tag -l "next-*" | tail
```

## Checkout a working branch
To work on some changes it is best to checkout a new branch. The following commands
achieve this. The two commands are only examples on how to checkout a branch. Both
commands will check out a branch.
```shell
git checkout -b first-patch next-.....
git checkout -b feature v6.2-rc1
```
## Creating a kernel configuration
The first step in compiling a kernel is to create a kernel configuration. There are
various ways to create a kernel configuration. In this guide we use the machine
configuration as the starting point for our config:
```shell
cp /boot/config-$(uname -r) .config
zcat /proc.config.gz > .config
```
Both of the above commands create a kernel config file. It is possible that one of
the commands does not work. It depends which features are configured in the current
kernel. The kernel config file is called .config.

Afterwards invoke menuconfig to see that we have a valid configuration. In addition
it is possible that new configuration options have been added and you will get
query prompts for them. If unsure answer the question with 'n' (no).
```shell
make menuconfig
```

## Kernel compilation
The last step is to compile the kernel. The kernel can be compiled with the
following command. The command invokes the make command with the -j option.
The -j option specifies the parallelism (how many workers work in parallel
on the compilation).
```shell
make -j$(nproc) 
```
or if you want to compile with clang, you can use:
```shell
make CC=clang -j$(nproc) 
```

## Kernel symbol lookup
To make development easier tools like cscope and tags are used to query where
source code symbols are used or defined. cscope and tags are the older tools.
They require a symbol database. The symbol database for these commands can
be created with the make command.
```shell
make cscope
make ctags
```

The cscope command has the advantage that it also has a terminal program to
run queries against the database. Otherwise the queries can be run from an
editor like vim or emacs. cscope is described [here](https://cscope.sourceforge.net/)
and ctags is described [here](https://ctags.sourceforge.net/).

A more modern way to query source code symbols is by using the compiler
databases. Instead of parsing the source code, they use the
[AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree) of the compiler.
Most editors have the ability to integrate and query that information.
The disadvantage to cscope is that only the symbols that are currently compiled
in can be queried. Kernel features that are not compiled in will not report
source code symbols. This can be a problem when kernel code changes are
made. However compiler databases have the advantage that they are more accurate.
