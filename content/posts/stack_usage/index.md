---
title: "Measure stack usage of kernel functions"
description: "How to measure stack usage and how to approach the stack frame exceeds limit warning"
date: 2024-12-20T17:17:37-08:00
featuredImage: "stack.jpg"
toc:
  enable: true
tags: ["linux", "kernel", "stack", "stackusage", "stackdelta", "gcc", "CONFIG_FRAME_WARN", "CONFIG_DEBUG_STACK_USAGE", "checkstack.pl", "stack_tracer_enabled", "frame-larger-than"]
draft: false
---

This article describes how to measure the stack usage of kernel functions and how to
apprach the dreaded "Stack frame exceeds limit message".
<!--more-->

## Overview
In Linux kernel development the stack size available is limited. It is important to verify
that the stack size requirement for a function stay within an acceptable range. The linux
kernel development directory contains several tools that help with analyzing the requirements.

In addition when you submit a patch the testing infrastructure will verify that your
stack frame requirements do not pass certain limits. These checks are not perfect and can
report false positives.

## "Stack frame exceeds limit warning"
After you submitted a patch, it is possible that you get an email from the CI infrastructure
that your patch passed certain limits. The following is an example of such an 
[email](https://marc.info/?l=linux-mm&m=169893983328519&w=4).
At this point it is necessary to analyze the stack growth of the reported functions and
compare the stack frame size before and after applying your patch.

It is important to consider that this is not necessarily a problem with your patch. There
are other factor that can contribute to the warning like

- You made a small change and this just caused the limit to be reached
- The code layout has changed with your change and this caused the increase in the
  stack frame size. This is generally especially a problem with functions that are
  defined inline or that get inlined.

## General Approach
To find out if this is a real problem or a false positive its best to compare before and
after applying the patch. Sometimes it is necessary to dump the object file with objdump
to compare the differences.

## stackusage 
In the scripts directory there is the [stackusage](https://elixir.bootlin.com/linux/v6.12/source/scripts/stackusage) script.
The stackusage script allows to recompile the kernel and create an output file. The
output file contains a line for each function with its stack size.

The script can be invoked like this. The `-o` option specifies the output file.
```shell
> scripts/stackusage -o ~/su.txt
```

Let's assume we are interested in the functions in the file
[ksm.c](https://elixir.bootlin.com/linux/v6.12/source/mm/ksm.c) in the memory
management layer, we can run the following command. It will search for all the functions
in the file [ksm.c](https://elixir.bootlin.com/linux/v6.12/source/mm/ksm.c), sort them
in ascending order by the third column (the stack size) and then format them in tabular
format. The functions with the biggest stack sizes are reported last.
```shell
> grep "ksm.c" ~/su.txt | sort -k 3 -n | expand -t 32
...
./mm/mm/ksm.c:884               ksm_get_folio                   64                              static
./mm/mm/ksm.c:3070              collect_procs_ksm               80                              static
./mm/mm/ksm.c:1626              stable_node_dup                 96                              static
./mm/mm/ksm.c:2996              rmap_walk_ksm                   96                              static
./mm/mm/ksm.c:623               break_ksm                       112                             static
./mm/mm/ksm.c:2741              ksm_del_vmas                    120                             static
./mm/mm/ksm.c:2732              ksm_add_vmas                    128                             static
./mm/mm/ksm.c:3801              ksm_init                        128                             static
./mm/mm/ksm.c:3338              run_store                       152                             static
./mm/mm/ksm.c:1442              try_to_merge_one_page           160                             static
./mm/mm/ksm.c:1246              write_protect_page.constprop    176                             static
./mm/mm/ksm.c:2665              ksm_scan_thread                 256                             static
```

This helps in finding the biggest stack consumers. The disadvantage of this utility is
that needs to recompile the kernel to be able to produce the stack usage report. This can
run for a while

## stackdelta
The [stackdelta](https://elixir.bootlin.com/linux/v6.12/source/scripts/stackdelta) utility
allows to compare to stackusage reports. This allows to analyze which functions have changed
in terms of stack frame size. Assuming I have two different stackusage report su.txt and su2.txt,
a diff report of the two can be created. The report only includes functions that are part of
both kernels and whose stack size has changed.

```shell
╭─shr@shr in repo: linux on  master [$?] took 0s
╰─λ scripts/stackdelta ~/su.txt ~/su2.txt | expand -t 10
...
./kernel/kernel/cpu.c         cpuhp_thread_fun              32        40        +8
./kernel/kernel/fork.c        mm_release                    40        32        -8
./kernel/kernel/irq_work.c    irq_work_single               32        40        +8
./kernel/kernel/padata.c      invoke_padata_reorder         16        24        +8
./kernel/kernel/padata.c      padata_parallel_worker        24        32        +8
./kernel/kernel/smp.c         generic_exec_single           32        40        +8
./kernel/kernel/softirq.c     __local_bh_enable_ip          8         16        +8
./kernel/kernel/softirq.c     _local_bh_enable              8         16        +8
./kernel/kernel/softirq.c     irq_enter_rcu                 8         16        +8
...
 ```

## checkstack.pl
The [checkstack.pl](https://elixir.bootlin.com/linux/v6.12/source/scripts/checkstack.pl)
script is a quicker way to produce a stack usage report. It uses the
[objdump](https://man7.org/linux/man-pages/man1/objdump.1.html) tool to create the stack
usage breakdown. Here is an example:
```shell

╭─shr@shr in repo: linux on  master [$?] took 4s
╰─λ objdump -d vmlinux | scripts/checkstack.pl | expand -t 20

...
0xffffffff81bcdca0 mlx5e_stats_eth_ctrl_get [vmlinux]:      528
0xffffffff81bcdb90 mlx5e_stats_eth_mac_get [vmlinux]:       528
0xffffffff81bcdb10 mlx5e_stats_eth_phy_get [vmlinux]:       528
0xffffffff81bcb410 mlx5e_stats_get_ieee [vmlinux]:          528
...
```

## gcc options
### frame-larger-than
The gcc compiler provides the option [-Wframe-larger-than=<len>](https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Warning-Options.html#index-Wframe-larger-than_003d). This can be used to warn if a function uses more than len bytes stack space.

### stack-usage
The gcc compiler now also contains [-fstack-usage](https://gcc.gnu.org/onlinedocs/gcc-14.2.0/gcc/Developer-Options.html#index-fstack-usage) option. If files get compiled with this
option and additional file per translation unit gets created. The file has the extension
`.su`. It contains the stack usage per file. The output is pretty similar to the above
output of the stackusage command.

## Analyzing the stack usage in a live system
The ftrace kernel tracing framework provides the ability to trace the stack size of a 
live system. The stack tracing functionality can be enabled with the following commands:

```shell
sudo echo 1 > /proc/sys/kernel/stack_tracer_enabled
cd /sys/kernel/tracing
[root@me tracing]# cat stack_trace
Depth    Size   Location    (11 entries)
-----    ----   --------
  0)     3040      48   rcu_segcblist_enqueue+0x5/0x40
  1)     2992      56   __call_rcu_common.constprop.0+0x9b/0x230
  2)     2936     560   mas_topiary_replace+0x769/0xe10
  3)     2376    1048   mas_wr_bnode+0x9d7/0x1a30
  4)     1328     112   mas_store_prealloc+0x1a6/0x3a0
  5)     1216     680   __mmap_region+0x8f7/0xd10
  6)      536     112   do_mmap+0x473/0x650
  7)      424     144   vm_mmap_pgoff+0xdf/0x1b0
  8)      280      64   ksys_mmap_pgoff+0x149/0x1f0
  9)      216      40   do_syscall_64+0x5b/0x130
 10)      176     176   entry_SYSCALL_64_after_hwframe+0x76/0x7e
```

The stack tracer has one configuration option: `stack_trace_filter`. If `stack_trace_filter`
is specified it will only include these functions that have been stored in the
stack_trace_filter file.

The file `stack_max_size` in the same directory reports the maximum stack size.

## Specifying CONFIG_FRAME_WARN
The kernel can be configured with the `CONFIG_FRAME_WARN` option. In that case it will create
warnings for all functions that use use more than the specified allocation size.
```
 Symbol: FRAME_WARN [=2048]
 Type  : integer
 Range : [0 8192]
 Defined at lib/Kconfig.debug:441
   Prompt: Warn for stack frames larger than
   Location:
     -> Kernel hacking
       -> Compile-time checks and compiler options
 (1)     -> Warn for stack frames larger than (FRAME_WARN [=2048])

```
Under the covers it uses the above described gcc option `-Wframe-larger-than`.

## Stack size debugging with /proc/vmstat
The Linux kernel 6.12 introduces a new configuration option called `CONFIG_DEBUG_STACK_USAGE`.
The new configuration option needs to be enabled when building a kernel. It can be found
under:
```
 Symbol: DEBUG_STACK_USAGE [=n]
 Type  : bool
 Defined at lib/Kconfig.debug:782
   Prompt: Stack utilization instrumentation
   Depends on: DEBUG_KERNEL [=y]
   Location:
     -> Kernel hacking
       -> Memory Debugging
 (1)     -> Stack utilization instrumentation (DEBUG_STACK_USAGE [=n])

```
Once the kernel is built with the stack usage option, we can query the `vmstat` file for the 
size of the kernel stacks.
```shell

╭─shr@virtme in repo: linux on  master [$] as 🧙 took 0s
╰─λ grep kstack /proc/vmstat
kstack_1k  0
kstack_2k  39
kstack_4k  170
kstack_8k  0
kstack_16k 0
```
It doesn't give the exact numbers, but we can see in which size buckets the kernel allocation
generally falls.
