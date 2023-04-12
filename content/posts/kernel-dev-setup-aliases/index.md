---
title: "Setting up the kernel development environment - Shell Aliases"
description: How to setup shell aliases for your Linux kernel development environment
date: 2023-04-11T20:58:11-07:00
featuredImage: "alias.jpg"
draft: false
toc:
  enable: true
tags: ["kernel", "setup", "shell", "alias"]
series: [kernel-development-setup]
series_weight: 5
---

This article describes how to define useful shell aliases for your kernel development
environment.
<!--more-->

## Overview
Shell aliases can make the life of a developer easier. In kernel development this
is even more important as a lot of the workflows are driven by the command line.

## General Aliases
Generally aliases make the life easier when navigating the source tree.

```shell
alias ..="cd .."
alias sb="source ~/.bash_profile"

alias io="cd ~/liburing"
alias lio="cd ~/liburing.upstream"

alias l="cd ~/linux"
alias lu="cd ~/linux.upstream"

alias r="cd ~/rocksdb"
alias s="cd ~/systemd"
```

I generally define aliases for the important directories. This makes navigating pretty
quick. I work mostly on three repositories: linux upstream repo, linux company repo and
io-uring. For each of them I have an alias defined. In addition I have an alias to switch
to the parent directory.

## Git Aliases
Some git commands are repeated pretty reguarly. I have defined aliases for some of them.
I often need to see the current list of branches, create a patch for for the last commit,
get a list of current commits and get the status of the current transaction.
```shell
alias gb="git branch"
alias gf='git format-patch -1'
alias glo="git log --oneline"
alias gs="git status"
```

## Editor Aliases
I also defined a few aliases to invoke emacs faster. One is for text mode emacs, the other
is to open emacs in graphics mode. In addition there are shortcuts to update the doom emacs
configuration and to invoke neovim.
```shell
alias ds='~/.emacs.d/bin/doom sync'

alias e="emacs -nw"
alias et="emacs -nw"
alias ew="emacs"

alias n="nvim"
```

## Email Aliases
To quickly fetch new emails, I have defined the `mb` alias.
```shell
alias mb="mbsync -a"
```

## Kernel Aliases
For kernel development I have defined a few additional aliases. `mc` invokes make clean and
`m` starts a new parallel build. It takes into accout how many processors are available on the
system. 

In case you decided to use LSP (Language server protocol), the `gen` alias updates the LSP
server. Especially after swithing branches this is necessary. If you use cscope instead,
the `cs` alias invokes cscope.
```shell
alias cpl="scripts/checkpatch.pl *.patch"
alias cs='cscope -k -d'
alias gen="scripts/clang-tools/gen_compile_commands.py"
alias m="make -j$(nproc) tar-pkg"
alias mc="make clean"
```

## Virtualization Aliases
To make virtualization easier to use, a couple of aliases have been defined. The standard
one is `vt`. `vt` starts a virtual kernel with 4 processors and 16MB of memory. `vio` starts a
kernel that also makes a local liburing and fio directory available. This is very useful for
running io-uring tests. `vdbg` is an alias that allows to attach a gdb session to the kernel
during startup (I will write an additional post that describes how to do this). The last
alias `vnvme` attaches a nvme device to the virtual machine.
```shell
alias vt='virtme-run --kimg arch/x86_64/boot/bzImage --rwdir=/home/shr/linux.upstream   -a "nokaslr"  --qemu-opts -m 16384 -smp 4'
alias vio='virtme-run --kimg arch/x86_64/boot/bzImage --rwdir=/home/shr/liburing.master --rwdir=/home/shr/fio  -a "nokaslr"  --qemu-opts -m 16384 -smp 4 -qmp tcp:localhost:4444,server,nowait'
alias vdbg='virtme-run --kimg arch/x86_64/boot/bzImage --rwdir=/home/shr/liburing.master --rwdir=/home/shr/fio  -a "nokaslr"  --qemu-opts -m 16384 -smp 4 -qmp tcp:localhost:4444,server,nowait -s -S'
alias vnvme='virtme-run --kimg arch/x86_64/boot/bzImage --rwdir=/home/shr/liburing.master --rwdir=/home/shr/fio  -a "nokaslr"  --qemu-opts -m 16384 -smp 4 -drive file=/home/shr/nvme.img,format=raw,if=none,id=NVME1 -device nvme,drive=NVME1,serial=1234'
```

