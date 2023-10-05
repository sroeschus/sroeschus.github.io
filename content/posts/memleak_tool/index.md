---
title: "How to use the memleak tool"
description: "How to use the memleak tool to find memory bugs"
date: 2023-09-17T21:57:44-07:00
featuredImage: "leak.jpg?width=20pc"
toc:
  enable: true
tags: ["linux", "memory", "memleak", "bcc", "bcc-tools"]
draft: false
---

This article describes how to use the memleak tool to find memory leaks.
<!--more-->

## Overview
To debug memory leaks, the `memleak` tool from the [bcc-tools](https://github.com/iovisor/bcc)
suite can be very useful.
It does not require to change the application like a lot of heap profilers require.
If the bcc-tools are not yet installed, they can be installed on arch linux with

```shell
pacman -S bcc-tools
```

The program also requires the linux-headers package. If the package is not yet installed,
it can be installed with the following command:

```shell
pacman -S linux-headers
```
## Signs of memory leaks
Monitoring the RSS size and the virtual size of a process over time might give good hints
if there is a potential memory leak. If the virtual size grows continuously at a faster pace
as RSS, there is a high likelyhood that there is a memory leak. Another indiciation is if
the memory size of the process does not stabilize and continues to grow.

## Invoking the tool
The memleak tool can be invoked with the `-c` switch to start the to be analyzed program
at the same time as the memleak tool. The other way is to attach the memleak tool with
the `-p` switch to a running process.

## Monitoring individual memory allocations
With the `-t` switch individual allocations and de-allocations can be monitored. This is
very useful to see how much memory is allocated. These are the memory allocations that
are performed by the kernel on behalf of the process. The user space memory manager of
the process might process a lot more memory allocation and memory de-allocation requests.
The memleak tool does not report the user-space requests.

The tool can be invoked with:

```shell
sudo memleak -tp 11748
```
The `-t` flags specifies tracing of individual allocations and the `-p` flag specifies
which process to trace.

The following gives an impression of the typical output:

```shell
(b'top', 11748, 8, b'...11', 3393.638436, b'alloc entered, size = 7')
(b'top', 11748, 8, b'...11', 3393.638438, b'alloc exited, size = 7, result = 56140ebca620')
(b'top', 11748, 8, b'...11', 3393.638444, b'free entered, address = 56140ebc92b0, size = 7')
(b'top', 11748, 8, b'...11', 3393.63845, b'alloc entered, size = 5')
(b'top', 11748, 8, b'...11', 3393.638453, b'alloc exited, size = 5, result = 56140ebc92b0')
```

The format of the output is: task, pid, cpu#, flags, timestamp and allocation / de-allocation
message. For each allocation it shows the two lines: alloc entered and alloc exited. For the
de-allocation operation it only shows the free entered entry. For the allocations as well as
for the de-allocation it reports the address as well as the size of the request. This makes it
very easy to write scripts on top of the output to see which memory addresses do not get freed.

In addition it also shows the process id and the name of the process. If the process starts
new threads or child processes they are also traced.

This has already helped to find some memory leaks in server applications.

# Capturing call stacks

If the user specifies the `-a` option, the tool also captures the top call stacks of
the callers. Typical result is something like this:

```shell
addr = 560cdc84a080 size = 29
addr = 560cdc846610 size = 29
addr = 560cdc848260 size = 29
addr = 560cdc849a00 size = 30
addr = 560cdc847e10 size = 32
addr = 560cdc846820 size = 32
30 bytes in 1 allocations from stack
0x00007fc75f6a33ef      __strdup+0x1f [libc.so.6]
0x622d36313a383475      [unknown]
32 bytes in 4 allocations from stack
0x00007fc75f6a33ef      __strdup+0x1f [libc.so.6]
0x0000746174732f35      [unknown]
32 bytes in 2 allocations from stack
0x00007fc75f6a33ef      __strdup+0x1f [libc.so.6]
0x00726f702d706f74      [unknown]
32 bytes in 2 allocations from stack
0x00007fc75f6a33ef      __strdup+0x1f [libc.so.6]
0x0064617068736172      [unknown]
32 bytes in 1 allocations from stack
0x00007fc75f6a33ef      __strdup+0x1f [libc.so.6]
0x74622d353a383475      [unknown]
32 bytes in 1 allocations from stack
0x00007fc75f6a33ef      __strdup+0x1f [libc.so.6]
0x74622d323a383475      [unknown]
45 bytes in 3 allocations from stack
0x00007fc75f6a33ef      __strdup+0x1f [libc.so.6]
0x0000767264726561      [unknown]
54 bytes in 7 allocations from stack
0x00007fc75f6a33ef      __strdup+0x1f [libc.so.6]
0x000000746174732f      [unknown]
63 bytes in 7 allocations from stack
0x00007fc75f6a33ef      __strdup+0x1f [libc.so.6]
0x0000746174732f00      [unknown]
```
Once potential leaks have been identified, this makes it easier to identify what code is reponsible
for the allocation or de-allocation. There are additional flags to the memleak tool, which allow to
report only allocations / de-allocations of a certain size.
