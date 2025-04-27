---
title: "Introduction to Linux Kernel debugging with drgn"
date: 2025-04-26T22:02:25
featuredImage: "drgn.jpg"
toc:
  enable: true
tags: ["linux", "kernel", "debugging", "drgn", "gdb", "crash"]
draft: false 
---

This article describes how to use the drgn debugger for Linux Kernel debugging.
<!--more-->

## Overview
Debugging Linux kernel code can be challenging. There are existing solutions with
GDB for live kernel debugging and crash to debug core dumps. While these solutions
have been very helpful in the past, for some type of problems they are not
flexible enough. This is where drgn can be very useful. Drgn is a newer debugger
that has been introduced by Omar Sandoval, a former colleague at Meta.

Existing debuggers have provided the ability to extend them with scripts. Drgn
takes a different approach to debugging than existing solutions: it integrates
directly with the python interpreter by provding additional API's to python.

## Approach
The python interpreter is the command shell for drgn. This allows to use the 
interpreter for small interactive introspection, as well as writing scripts or
integrating it into analysis and debugging tools. The approach is to write
small scripts to report or query the information you are interested in. The
python interpreter is extended so core dumps can be loaded and introspected.

## Installation
Most Linux distributions already provide packages, so drgn is installed easily
and quickly. In addition the software hosted on [github](https://github.com/osandov/drgn)
and can also be built from source. In addition the software needs kernel
debugging symbols. Install the debugging symbols for your Linux distribution.
The [documentation](https://drgn.readthedocs.io/en/latest/getting_debugging_symbols.html)
has a good description of the different approaches.

For Arch Linux you will do:
```Linux
sudo pacman -S drgn
sudo pacman -S --needed libelf
source /etc/profile.d/debuginfod.sh
```

## Loading
The first step in analyzing a running kernel or a kernel dump is to start
drgn with the corresponding parameters. The default is to debug the running kernel.

### Load the running kernel
The running kernel can be loaded with:
```Linux
drgn

drgn 0.0.31 (using Python 3.13.3, elfutils 0.192, with debuginfod (dlopen), with libkdumpfile)
For help, type help(drgn).
>>> import drgn
>>> from drgn import FaultError, NULL, Object, alignof, cast, container_of, execscript, implicit_convert,offsetof, reinterpret, sizeof, stack_trace
>>> from drgn.helpers.common import *
>>> from drgn.helpers.linux import *
>>>
```

or

```Linux
drgn -k
```

### Loading a core dump
A core dump can be loaded with:
```Linux
drgn -c <core-dump-file>
```

### Debugging a process
Drgn can also debug user processes. With the following command an individual
process can be loaded:
```Linux
drgn -p <pid>
```

## Accessing State
drgn has very good [documentation](https://drgn.readthedocs.io/en/latest/user_guide.html).
I will not repeat the contents of the documentation here, but highlight a few key points.
drgn introduces a drgn.Program object. This object can be used to access the values, constants
and types of the Linux kernel. This can return references or values. Values can be
accessed directly, while for references, the .value_() needs to be called.

To get information for the definition of a type the following can be used:
```Linux
>>> prog.type("struct task_struct")
struct task_struct {
struct thread_info thread_info;
unsigned int __state;
unsigned int saved_state;
void *stack;

>>> prog.type("struct task_struct")
struct task_struct {
struct thread_info thread_info;
unsigned int __state;
unsigned int saved_state;
void *stack;
...
```

The value of variables can be accessed with:
```Linux
>>> prog["init_task"]
(struct task_struct){
    .thread_info = (struct thread_info){
        .flags = (unsigned long)16384,
        .syscall_work = (unsigned long)0,
        .status = (u32)0,
        .cpu = (u32)0,
    },
    .__state = (unsigned int)0,
    ...
```

Values in structures, are accessed with the . notation:
```Linux
>> prog["init_task"].mm
(struct mm_struct *)0x0
>>> prog["init_task"].active_mm
    *(struct mm_struct *)0xffff88810533b9c0 = {
        .mm_count = (atomic_t){
            .counter = (int)7,
    },
    .mm_mt = (struct maple_tree){
        .ma_lock = (spinlock_t){
            .rlock = (struct raw_spinlock){
                .raw_lock = (arch_spinlock_t){
                    .val = (atomic_t){
                    ...
```

## Accessing References
To print references, the .value_() function needs to be used. For interactive
usage this is not that important, but when writing scripts, it might be required
to only get the value and not also the dataytpe:
```Linux
>> prog["init_task"].active_mm.mm_count
(atomic_t){
    .counter = (int)7,
}
>>> prog["init_task"].active_mm.mm_count.counter
(int)7
>>> prog["init_task"].active_mm.mm_count.counter.value_()
7
```

Sometimes its necessary to get address of reference. The address of a reference
is returned with the address_of_() function:
```Linux
>> prog["init_task"].active_mm.mm_count.address_of_()
*(atomic_t *)0xffff88810533b9c0 = {
.counter = (int)7,
}

>>> prog["init_task"].active_mm.mm_count.address_of_().value_()
18446612686452275648

>>> hex(prog["init_task"].active_mm.mm_count.address_of_().value_())
'0xffff88810533b9c0'
```

## Constructing objects at an address
Sometimes it is necessary to construct an object at a specific address. This can be
achieved by constructing a drgn object at the requested address. The following example
creates a zone object at <zone_address>:

```Linux
my_zone = Object(prog, "struct zone *", int(zone_address, 0))
```

## Using helpers
Drgn has added so called helper API's to access and interate over key Linux kernel
data structures. Among them are:
- Double-linked lists
- Linked lists
- Lockless lists
- Maple Trees
- Priority-Sorted lists
- Readix trees
- Red-black trees
- XArrays

The API documentation covers all the helper functions. In addition there are addtional
helpers for other kernel structures and subsystems.

The following shows an example of how to iterate over the task list:
```Linux
>>> for p in for_each_task(prog):
...     print(p.pid)
...
(pid_t)1
(pid_t)2
...
```

## Getting the call stack
The call stack of a process can be obtained with the stack_trace command. The pid
of the process has to be specified as the first parameter.

### Getting the call stack for an individual process
The call stack of a process can be reported with the following command:
```Linux
>>> stack_trace(304)
#0  context_switch (kernel/sched/core.c:5369:2)
#1  __schedule (kernel/sched/core.c:6756:8)
#2  __schedule_loop (kernel/sched/core.c:6833:3)
#3  schedule (kernel/sched/core.c:6848:2)
#4  worker_thread (kernel/workqueue.c:3406:2)
#5  kthread (kernel/kthread.c:389:9)
#6  ret_from_fork (arch/x86/kernel/process.c:147:3)
#7  ret_from_fork_asm+0x1a/0x1f (arch/x86/entry/entry_64.S:244)
```

Its also possible to print a so-called annotated stack trace:
```Linux
>>> print_annotated_stack(stack_trace(304))
STACK POINTER     VALUE
[stack frame #0 at 0xffffffff820ca31a (__schedule+0x52a/0x1082) in context_switch at kernel/sched/core.c:5369:2 (inlined)]
[stack frame #1 at 0xffffffff820ca31a (__schedule+0x52a/0x1082) in __schedule at kernel/sched/core.c:6756:8]
ffffc90000427e20: ffff888100078c00 [slab object: kmalloc-1k+0x0]
ffffc90000427e28: ffff888106a23300 [slab object: task_struct+0x0]
ffffc90000427e30: 0100000000000282
ffffc90000427e38: 0000040200000000
ffffc90000427e40: 0000000000000000
ffffc90000427e48: 0000000000000004
ffffc90000427e50: ffffffff8242fad0 [object symbol: cpu_bit_bitmap+0x30]
ffffc90000427e58: 0000000000000000
ffffc90000427e60: 0000000000000002
ffffc90000427e68: 39d5cfeb65f0c600
ffffc90000427e70: 0000000000000000
ffffc90000427e78: ffff888104667ac0 [slab object: kmalloc-192+0x40]
ffffc90000427e80: ffff888100078c28 [slab object: kmalloc-1k+0x28]
ffffc90000427e88: ffff888106a23300 [slab object: task_struct+0x0]
ffffc90000427e90: 0000000000000000
ffffc90000427e98: ffff888106a23300 [slab object: task_struct+0x0]
ffffc90000427ea0: ffffffff820caebb [function symbol: schedule+0x2b]
[stack frame #2 at 0xffffffff820caebb (schedule+0x2b/0x1c9) in __schedule_loop at kernel/sched/core.c:6833:3 (inlined)]
[stack frame #3 at 0xffffffff820caebb (schedule+0x2b/0x1c9) in schedule at kernel/sched/core.c:6848:2]
ffffc90000427ea8: ffff888104667a80 [slab object: kmalloc-192+0x0]
ffffc90000427eb0: ffff888100078c00 [slab object: kmalloc-1k+0x0]
ffffc90000427eb8: ffffffff8115e190 [function symbol: worker_thread+0x90]
[stack frame #4 at 0xffffffff8115e190 (worker_thread+0x90/0x31b) in worker_thread at kernel/workqueue.c:3406:2]
ffffc90000427ec0: ffff888104667ac0 [slab object: kmalloc-192+0x40]
ffffc90000427ec8: 0000000080000000
ffffc90000427ed0: ffff888100ecb100 [slab object: kmalloc-128+0x0]
ffffc90000427ed8: 0000000000000000
ffffc90000427ee0: ffff888106a23300 [slab object: task_struct+0x0]
ffffc90000427ee8: ffffffff8115e100 [function symbol: worker_thread+0x0]
ffffc90000427ef0: ffff888104667a80 [slab object: kmalloc-192+0x0]
ffffc90000427ef8: ffffffff81167de5 [function symbol: kthread+0x105]
[stack frame #5 at 0xffffffff81167de5 (kthread+0x105/0x12a) in kthread at kernel/kthread.c:389:9]
ffffc90000427f00: ffffffff81167ce0 [function symbol: kthread+0x0]
ffffc90000427f08: ffffc90000427f58 [vmap stack: 304 (kworker/u32:4) +0x3f58]
ffffc90000427f10: ffff888102131b40 [free slab object: kmalloc-64+0x0]
ffffc90000427f18: 0000000000000000
ffffc90000427f20: 0000000000000000
ffffc90000427f28: 0000000000000000
ffffc90000427f30: ffffffff810be056 [function symbol: ret_from_fork+0x46]
[stack frame #6 at 0xffffffff810be056 (ret_from_fork+0x46/0x5d) in ret_from_fork at arch/x86/kernel/process.c:147:3]
ffffc90000427f38: ffffffff81167ce0 [function symbol: kthread+0x0]
ffffc90000427f40: 0000000000000000
ffffc90000427f48: ffff888102131b40 [free slab object: kmalloc-64+0x0]
ffffc90000427f50: ffffffff8107d3fa [symbol: ret_from_fork_asm+0x1a]
[stack frame #7 at 0xffffffff8107d3fa (ret_from_fork_asm+0x1a/0x1f) at arch/x86/entry/entry_64.S:244]
ffffc90000427f58: 0000000000000000
```
The annotated call stack does not only contain the function information, but also the local
variables and their addresses.

### Get the callstack for all processes.
This is where drgn shines. Based on the earlier information, we use the task helper to
iterate over all the tasks, obtain the pid of each task and then report the stack trace
for each task:

```Linux
>>> for p in for_each_task(prog):
...     pid = p.pid.value_()
...     print("\nCall stack for PID {0}".format(pid))
...     try:
...         stack_trace(pid)
...     except ValueError:
...         print("\tCannot unwind current task")
...

Call stack for PID 1
#0  context_switch (kernel/sched/core.c:5369:2)
#1  __schedule (kernel/sched/core.c:6756:8)
#2  __schedule_loop (kernel/sched/core.c:6833:3)
#3  schedule (kernel/sched/core.c:6848:2)
#4  do_wait (kernel/exit.c:1696:3)
#5  kernel_wait4 (kernel/exit.c:1850:8)
#6  do_syscall_x64 (arch/x86/entry/common.c:52:14)
#7  do_syscall_64 (arch/x86/entry/common.c:83:7)
#8  entry_SYSCALL_64+0xab/0x148 (arch/x86/entry/entry_64.S:121)
#9  0x7f2ddcb89be2

Call stack for PID 2
#0  context_switch (kernel/sched/core.c:5369:2)
#1  __schedule (kernel/sched/core.c:6756:8)
#2  __schedule_loop (kernel/sched/core.c:6833:3)
#3  schedule (kernel/sched/core.c:6848:2)
#4  kthreadd (kernel/kthread.c:755:4)
#5  ret_from_fork (arch/x86/kernel/process.c:147:3)
#6  ret_from_fork_asm+0x1a/0x1f (arch/x86/entry/entry_64.S:244)
...
```
The code contains a try-except block. This is required to guard against a possible
exception: we can't get the current stack of the running process. This can happen
if we debug the live kernel.

## Summary

This article has given a short overview on the drgn debugger and how to get started with
using drgn. The article should help with getting you over some of the initial challenges
on using drgn.

If one wants to experiment with drgn using a virtual machine like qemu and virtme-ng
is a great way to do this. 

Future articles will describe some utilities that use the drgn API.


