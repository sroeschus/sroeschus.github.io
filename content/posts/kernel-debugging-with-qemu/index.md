---
title: "Kernel Debugging With Qemu and virtme"
description: "How to debug the Linux Kernel with qemu and virtme"
date: 2023-04-16T21:43:57-07:00
featuredImage: "bug.jpg"
draft: false
toc:
  enable: true
tags: ["kernel", "gdb", "virtme", "qemu", "vmlinux-gdb.py", "vmlinux-gdb", "nokaslr", "lx-ps", "lx-dmesg"]
---
This article describes how to debug the Linux kernel with qemu, virtme and gdb.
<!--more-->

## Overview
Being able to debug the Linux kernel helps the developer to gain a better understanding
of the code, data structures and also defects. Using the kernel debugger directly with
the Linux debugger has its own challenges. A better solution is to start the kernel in
a virtual machine and attach the debugger to this virtual machine.

## Setting up the virtual machine
If you followed the earlier series about how to setup your Linux Kernel development
environment, then you already setup qemu and virtme. If you haven't done so, now would
be a good time to catch up. Once qemu and virtme have been setup and are working, the
preconditions are met.

## Installing gdb
In most Linux distributions gdb is installed by default. If gdb hasn't been installed yet,
this can be done with the following command (assuming an arch-based distribution).
```shell
sudo pacman -S -y gdb
```

## Cloning my kernel gdb scripts
It is a good idea to clone the [kernel-gdb](https://github.com/sroeschus/kernel-gdb)
github repository. The repository contains two scripts to invoke the debugger `kernel.gdb` 
and 'blk.gdb'. As a starting point kernel.gdb can be used. The `add-auto-load-safe-path`
for the `vmlinux-gdb.py` script needs to be updated for your kernel build environment.

## Defining an alias for virtme
It makes sense to define an alias to start virtme with the necessary parameters so gdb
can be attached.
```shell
alias vdbg 'virtme-run --kimg arch/x86_64/boot/bzImage  -a "nokaslr" 
  --qemu-opts -m 16384 -smp 4 -qmp tcp:localhost:4444,server,nowait -s -S'
```
The command disables kaslr (kernel address space randomization), uses 16GB of memory, 4 CPU's,
and defines the port on how to communicate with qemu. It also specifies to wait for the
debugger to attach.

## Building the kernel
To be able to see the symbols the kernel needs to be built with symbols. The easiest is
to clone the [kernel-configs](https://github.com/sroeschus/kernel-configs) repo and use
the kernel config from the `debug` directory. Then rebuild your kernel.

## Debugging the kernel
To debug the kernel it is the easiest to use two windows: one to start the virtual machine
with vdbg and the other one to start the debugger. It is assumed that the following is
executed from the Linux source kernel base directory.

In window 1:
```shell
vdbg
```
This will only show an empty prompt.

In the second window, the following command is entered:
```shell
gdb -command=kernel.dbg

╭─shr@stefan in repo: linux on  ksm.v8:mm-unstable [?⇡3] via  v3.10.10 took 25s
╰─λ gdb --command=~/repo/shr/kernel-gdb/kernel.gdb
GNU gdb (GDB) 13.1
Copyright (C) 2023 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-pc-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word".
0x000000000000fff0 in exception_stacks ()
Hardware assisted breakpoint 1 at 0xffffffff83046940: file init/main.c, line 881.

Thread 1 hit Breakpoint 1, start_kernel () at init/main.c:881
881     {
```
The [kernel.gdb](https://github.com/sroeschus/kernel-gdb/blob/main/kernel.gdb) script
defines a breakpoint at the [start_kernel()](https://elixir.bootlin.com/linux/latest/source/init/main.c#L940)
function and stops the execution of the kernel at that function. This is very early in the boot sequence of the
kernel and allows to define additional breakpoints.

With the `c` command (continue), the startup sequence can be resumed. In the first window
one can see how the startup log statements are printed until the command prompt is
available.

## Linux kernel debug commands
If the vmlinux-gdb.py script has been successfully loaded with the kernel.gdb script, additional
gdb commands and functions get defined. The list of additional commands can be queried with the following
command:
```gdb
apropos lx
function lx_clk_core_lookup -- Find struct clk_core by name
function lx_current -- Return current task.
function lx_device_find_by_bus_name -- Find struct device by bus and name (both strings)
function lx_device_find_by_class_name -- Find struct device by class and name (both strings)
function lx_module -- Find module by name and return the module variable.
function lx_per_cpu -- Return per-cpu variable.
function lx_rb_first -- Lookup and return a node from an RBTree
function lx_rb_last -- Lookup and return a node from an RBTree.
function lx_rb_next -- Lookup and return a node from an RBTree.
function lx_rb_prev -- Lookup and return a node from an RBTree.
function lx_task_by_pid -- Find Linux task by PID and return the task_struct variable.
function lx_thread_info -- Calculate Linux thread_info from task variable.
function lx_thread_info_by_pid -- Calculate Linux thread_info from task variable found by pid
lx-clk-summary -- Print clk tree summary
lx-cmdline -- Report the Linux Commandline used in the current kernel.
lx-configdump -- Output kernel config to the filename specified as the command
lx-cpus -- List CPU status arrays
lx-device-list-bus -- Print devices on a bus (or all buses if not specified)
lx-device-list-class -- Print devices in a class (or all classes if not specified)
lx-device-list-tree -- Print a device and its children recursively
lx-dmesg -- Print Linux kernel log buffer.
lx-fdtdump -- Output Flattened Device Tree header and dump FDT blob to the filename
lx-genpd-summary -- Print genpd summary
lx-iomem -- Identify the IO memory resource locations defined by the kernel
lx-ioports -- Identify the IO port resource locations defined by the kernel
lx-list-check -- Verify a list consistency
lx-lsmod -- List currently loaded modules.
lx-mounts -- Report the VFS mounts of the current process namespace.
lx-ps -- Dump Linux tasks.
lx-symbols -- (Re-)load symbols of Linux kernel and currently loaded modules.
lx-timerlist -- Print /proc/timer_list
lx-version -- Report the Linux Version of the current kernel.
```

The commands itself should be pretty self explanatory. Especially useful are the
`lx-ps` and the `lx-dmesg` commands. But this depends on the use case. Additional
documentation to the scripts and functions is available on [kernel.org](https://www.kernel.org/doc/html/v4.10/dev-tools/gdb-kernel-debugging.html).

## Extending GDB
This only provides the standard gdb experience. Most of the time you are also
interested to see the source code. In the same gdb repository there is also a
`gdbinit` script. This script needs to be renamed to `.gdbinit`. This script gets
automatically loaded when gdb gets started.

```gdb
─── Output/messages ────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

Thread 1 hit Breakpoint 1, start_kernel () at init/main.c:881
881     {
─── Source ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
861      unknown_options = memblock_alloc(len, SMP_CACHE_BYTES);
862      if (!unknown_options) {
863          pr_err("%s: Failed to allocate %zu bytes\n",
864              __func__, len);
865          return;
866      }
867      end = unknown_options;
868
869      for (p = &argv_init[1]; *p; p++)
870          end += sprintf(end, " %s", *p);
871      for (p = &envp_init[2]; *p; p++)
872          end += sprintf(end, " %s", *p);
873
874      /* Start at unknown_options[1] to skip the initial space */
875      pr_notice("Unknown kernel command line parameters \"%s\", will be passed to user space.\n",
876          &unknown_options[1]);
877      memblock_free(unknown_options, len);
878  }
879
880  asmlinkage __visible void __init __no_sanitize_address start_kernel(void)
881  {
882      char *command_line;
883      char *after_dashes;
884
885      set_task_stack_end_magic(&init_task);
886      smp_setup_processor_id();
887      debug_objects_early_init();
888      init_vmlinux_build_id();
889
890      cgroup_init_early();
891
892      local_irq_disable();
893      early_boot_irqs_disabled = true;
894
895      /*
896       * Interrupts are still disabled. Do necessary setups, then
897       * enable them.
898       */
899      boot_cpu_init();
900      page_address_init();
─── Variables ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
loc after_dashes = <optimized out>
loc command_line = 0x13cd0 <exception_stacks+31952> <error: Cannot access memory at address 0x13cd0>: Can…
─── Stack ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
[0] from 0xffffffff83046940 in start_kernel+0 at init/main.c:881
[1] from 0xffffffff81000145 in secondary_startup_64 at arch/x86/kernel/head_64.S:358
[2] from 0x0000000000000000
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
>>>
```
This shows the thread, source, variables and the stack. This makes debugging easier.
There are several gdb extensions which provide an interface like this. The provided gdb
script is a modified version of one these.

