---
title: "Introduction to kernel tracing with ftrace"
description: "Using ftrace for Linux Kernel tracing"
date: 2024-12-18T21:13:23-08:00
featuredImage: "ftrace.jpg"
toc:
  enable: true
tags: ["linux", "kernel", "tracing", "ftrace"]
draft: false
---

This article describes how to use the ftrace kernel framework for tracing.
<!--more-->

## Overview
The Linux kernel also contains a very powerful tracing framework. It is able to
trace various aspects of kernel code execution. The tracing framework captures
every execution. This is different from profilers, which only sample executionns.
Among other things this can be very valuable in root cause analysis of problems
and to understand the code flow.

## Kernel Configuration
The linux kernel must be configured to include the ftrace tracing framework. Most
Linux distributions include some support for ftrace in their kernels. However not
all ftrace features might be enabled by default. It can be necessary to determine
which ftrace features are supported by the current kernel.

| Configuration name           | Explanation                          |
|:---------------------------- |:------------------------------------ |
| CONFIG_DYNAMIC_FTRACE        |  Enable function tracing dynamically |
| CONFIG_FTRACE                |  Enable ftrace                       |

| Tracer                       |  Explanation                      |
|:---------------------------- |:--------------------------------- |
| CONFIG_FUNCTION_TRACER       |  Enable function tracer           |
| CONFIG_FUNCTION_GRAPH_TRACER |  Enable function graph tracer     |
| CONFIG_BRANCH_TRACER         |  Enable branch tracer             |
| CONFIG_IRQSOFF_TRACER        |  Enable irqs-off trace            |
| CONFIG_PREEMPT_TRACER        |  Enable preemption-off tracer     |
| CONFIG_SCHED_TRACER          |  Enable scheduling tracer         |
| CONFIG_HWLAT_TRACER          |  Enable hardware latency tracer   |
| CONFIG_OSNOISE_TRACER        |  Enable osnoise tracer            |
| CONFIG_TIMERLAT_TRACER       |  Enable timerlat tracer           |
| CONFIG_MMIOTRACE             |  Enable memory mapped IO access   |
| CONFIG_FTRACE_SYSCALLS       |  Enable syscall tracer            |
| CONFIG_BLK_DEV_IO_TRACE      |  Enable block IO tracer           |

| Probes                |  Explanation                             |
|:--------------------- |:---------------------------------------- |
| CONFIG_FPROBE         |  Enable function probe                   |
| CONFIG_FPROBE_EVENTS  |  Enable tracing events on function entry |
| CONFIG KPROBE_EVENTS  |  Enable kernel probes                    |
| CONFIG_UPROBE_EVENTS  |  Enable userspace dynamic events         |
| CONFIG_USER_EVENTS    |  Enable user defined events              |

| Probes                   |  Explanation                      |
|:------------------------ |:--------------------------------- |
| CONFIG_FUNCTION_PROFILER |  Enable function profiler         |
| CONFIG_DEBUG_FS_DISALLOW |  Hide /sys/kernel/debug/tracing   |

The following kernel options should be enabled:
- CONFIG_FTRACE
- CONFIG_DYNAMIC_FTRACE
- CONFIG_FUNCTION_TRACER
- CONFIG_FUNCTION_GRAPH_TRACER
- CONFIG_FTRACE_SYSCALLS
- CONFIG_FPROBE
- CONFIG_FPROBE_EVENTS
- CONFIG_KPROBE_EVENTS
- CONFIG_UPROBE_EVENTS
- CONFIG_USER_EVENTS

Additional kernel configuration options can be enabled as needed.

## Location
The configuration files for the ftrace framework are located in /sys/kernel/tracing.
Most Linux distributions install ftrace by default (its generally disabled with
the nop tracer). Here is the typical contents of the directory:

```shell
available_events                  function_profile_enabled  set_event_notrace_pid   trace
available_filter_functions        hwlat_detector            set_event_pid           trace_clock
available_filter_functions_addrs  instances                 set_ftrace_filter       trace_marker
available_tracers                 kprobe_events             set_ftrace_notrace      trace_marker_raw
buffer_percent                    kprobe_profile            set_ftrace_notrace_pid  trace_options
buffer_size_kb                    max_graph_depth           set_ftrace_pid          trace_pipe
buffer_subbuf_size_kb             options                   set_graph_function      trace_stat
buffer_total_size_kb              osnoise                   set_graph_notrace       tracing_cpumask
current_tracer                    per_cpu                   snapshot                tracing_max_latency
dynamic_events                    printk_formats            stack_max_size          tracing_on
dyn_ftrace_total_info             README                    stack_trace             tracing_thresh
enabled_functions                 saved_cmdlines            stack_trace_filter      uprobe_events
error_log                         saved_cmdlines_size       synthetic_events        uprobe_profile
events                            saved_tgids               timestamp_mode          user_events_data
free_buffer                       set_event                 touched_functions       user_events_status
```
## Kernel configuration for ftrace
If you work on an older Linux kernel or a Linux kernel that hasn't ftrace included as
part of the kernel build, it is necessary to configure the kernel build with make config, enable
the required options, build a new kernel and then install the new kernel.

If the above directory /sys/kernel/tracing exists, the kernel has been built with ftrace support.
{{< admonition type=tip title="" open=true >}}
If you are running an older kernel (before 4.1), the configuration directory is at 
/sys/kernel/debug/tracing.
{{< /admonition >}}

## Tracers
The ftrace framework consists of serveral tracers. They can trace different aspects of the system.
The following is an overview of the available tracers, it also depends which kernel options have
been enabled when the system was configured.

| Tracer | Explanation |
|:------------------ |:------------------------------------------- |
| blk                | Block I/O tracer                            |
| branch             | Trace likely/unlikely calls                 |
| function           | Function tracer                             |
| function\_graph    | Function graph tracer                       |
| hwlat              | System firmware and hardware latency tracer |
| irqsoff            | Trace time interrupts disabled              |
| mmiotrace          | Trace module calls to hardware              |
| nop                | No tracing                                  |
| osnoise            | Trace interference from OS like SMI's       |
| preemptirqsoff     | Trace time preemption and IRQS disabled     |
| preemptoff         | Trace time preemption is disabled           |
| timerlat           | Trace wakeup latencies of real-time threads |
| wakeup\_dl         | Trace wakeup time for SCHED_DEADLINE task   |
| wakeup\_rt         | Trace wakeup time of highest prio RT task   |
| wakeup             | Trace wakeup time of highest prio task      |

The installed tracers can be queried with
```shell
[root@shr-linux tracing] cat /sys/kernel/tracing/available_tracers 
timerlat osnoise hwlat blk mmiotrace branch function_graph wakeup_dl wakeup_rt wakeup
preemptirqsoff preemptoffirqsoff function nop
```
The list of tracers is described [here](https://kernel.org/doc/html/v6.12/trace/ftrace.html#the-tracers).
In addition to to the tracers also events can be traced.

## Clients
There are two ways to configure and use the ftrace framework:
- the original way by directly manipulating the files in the /sys/kernel/tracing directory
- the new way by using the [trace-cmd](https://trace-cmd.org/).

This article will mostly focus on using the first one. The second doesn't support all the
options of the first one. If the first option is used, it is recommended to create a script with
the file modifications in /sys/kernel/tracing and then execute that script.

{{< admonition type=warning title="" open=true >}}
Tracing can create a lot of data and depending on the options that are enabled, it can
slow down the system. It is recommended to trace the system first only for a couple of
seconds to reduce the risk.
{{< /admonition >}}

## Implementation
Ftrace works by using function hooks by using compiler instrumentation. For better
performance there is also a dynamic ftrace option, which enables the hooks on the fly.
The kernel configuration option is called CONFIG_DYNAMIC_FTRACE. Ftrace itself is
implemented as virtual filesystem. The filesystem is called tracefs. The default mount
point is /sys/kernel/tracing. In case the kernel config option CONFIG_DEBUG_FS_DISALLOW_MOUNT
is set, then debugfs is available, but it is not visible.

```shell

[root@shr-linux tracing] mount | grep tracefs
tracefs on /sys/kernel/tracing type tracefs (rw,nosuid,nodev,noexec,relatime)
```

The trace information is collected inside memory buffers.

## Function tracer
The function tracer is the most simple tracer. The general approach is to start the tracer
let it run for a few seconds or a certain time period, stop the trace collection, look at
the output and then clear the trace buffers.

To user the trace buffers the following steps can be used. The `-p` flag determines which
tracer is used (sometimes the tracer is also called a plugin).
```shell
[root@shr-linux tracing] echo > trace
[root@shr-linux tracing] echo function > current_tracer
[root@shr-linux tracing] echo 1 > tracing_on

# wait a bit

[root@shr-linux tracing] echo 0 > tracing_on
[root@shr-linux tracing] cat trace | head -20
# tracer: function
#
# entries-in-buffer/entries-written: 959600/6479750   #P:24
#
#                                _-----=> irqs-off/BH-disabled
#                               / _----=> need-resched
#                              | / _---=> hardirq/softirq
#                              || / _--=> preempt-depth
#                              ||| / _-=> migrate-disable
#                              |||| /     delay
#           TASK-PID     CPU#  |||||  TIMESTAMP  FUNCTION
#              | |         |   |||||     |         |
<idle>-0       [005] d..2. 12618.684925: sched_idle_set_state <-cpuidle_enter_state
<idle>-0       [005] d..2. 12618.684928: irq_enter_rcu <-sysvec_apic_timer_interrupt
<idle>-0       [005] d.h2. 12618.684929: tick_irq_enter <-irq_enter_rcu
<idle>-0       [005] d.h2. 12618.684929: tick_check_oneshot_broadcast_this_cpu <-tick_irq_enter
<idle>-0       [005] d.h2. 12618.684930: ktime_get <-tick_irq_enter
<idle>-0       [005] d.h2. 12618.684931: nr_iowait_cpu <-tick_irq_enter
<idle>-0       [005] d.h2. 12618.684931: _raw_spin_lock <-tick_irq_enter
<idle>-0       [005] d.h3. 12618.684932: tick_do_update_jiffies64.part.0 <-tick_irq_enter
[root@shr-linux tracing] echo > trace
```
The interesting output start with line 9: it shows how many entries are in the in-memory
buffer and how many were received. In our case the in-memory buffer has 959600 entries.
It also shows that the system has 24 CPU's.

Beginning from line 19, the trace ouput starts. Each line consists of task/pid, on which CPU
it was executed, the latency trace information, timestamp/delay information and which function was
executed.

Line 27 clears the trace buffer.

## Latency trace information
The latency bits need some additional explanation. You can check the
[documentation](https://docs.kernel.org/trace/ftrace.html#latency-trace-format) for a detailled
explanation. Here is a short summary:

If the value is not set, it has the value `"."`.
If the `irqs-off` has the value `d`, then interrupts are disabled, otherwise the irqs are on.
The `need-resched` column has the following meaning. They help with determining if the scheduler
will be invoked and what bit is set.

| Value | Explanation |
|:-------|:------------|
| B      | TIF_NEED_RESCHED, PREEMPT_NEED_RESCHED, TIF_RESCHED_LAZY set |
| N      | TIF_NEED_RESCHED, PREEMPT_NEED_RESCHED set                   |
| n      | TIF_NEED_RESCHED set                                         |
| p      | PREEMPT_NEED_RESCHED set                                     |
| L      | PREEMPT_NEED_RESCHED, TIF_RESCHED_LAZY set                   |
| b      | TIF_NEED_RESCHED, TIF_RESCHED_LAZY set                       |
| l      | TIF_RESCHED_LAZY set                                         |

The `hardirq/softirq` column reports in which context the processor is currently running. The
values have the following meaning:

| Value  | Explanation |
|:-------|:------------|
| Z      | NMI occured inside a hardirq     |
| z      | NMI is running                   | 
| H      | Hardirq occured inside a softirq |
| h      | Hardirq is running               |
| s      | Softirq is running               |

## Which functions can be traced?
The trace-cmd also allows to query which functions can be traced (this only lists the first 20
functions)
```shell
[root@shr-linux tracing] cat available_filter_functions | head -20

__traceiter_initcall_level
__probestub_initcall_level
__traceiter_initcall_start
__probestub_initcall_start
__traceiter_initcall_finish
__probestub_initcall_finish
trace_initcall_finish_cb
initcall_blacklisted
do_one_initcall
__ftrace_invalid_address___740
rootfs_init_fs_context
wait_for_initramfs
__ftrace_invalid_address___84
calibration_delay_done
calibrate_delay
tdx_tlb_flush_required
tdx_cache_flush_required
tdx_enc_status_changed
tdx_enc_status_change_finish
tdx_enc_status_change_prepare
```

If we are interested in the kmalloc allocation functions, the following can be used instead:
```shell
[root@shr-linux tracing] grep vfs available_filter_functions | grep read

vfs_readv
vfs_iter_read
vfs_iocb_iter_read
vfs_read
vfs_readlink
vfs_splice_read
```

## Tracing individual functions
The following approach can be used to trace an individual function:

```shell
[root@shr-linux tracing] echo "vfs_read" > set_ftrace_filter
[root@shr-linux tracing] echo 1 > tracing_on
[root@shr-linux tracing] echo 0 > tracing_on
[root@shr-linux tracing] cat trace
# tracer: function
#
# entries-in-buffer/entries-written: 12992/12992   #P:24
#
#                                _-----=> irqs-off/BH-disabled
#                               / _----=> need-resched
#                              | / _---=> hardirq/softirq
#                              || / _--=> preempt-depth
#                              ||| / _-=> migrate-disable
#                              |||| /     delay
#           TASK-PID     CPU#  |||||  TIMESTAMP  FUNCTION
#              | |         |   |||||     |         |
         konsole-2543    [016] ...1. 16904.173116: vfs_read <-__x64_sys_read
            sudo-42508   [016] ...1. 16904.173336: vfs_read <-__x64_sys_read
         konsole-2543    [016] ...1. 16904.173517: vfs_read <-__x64_sys_read
         konsole-2543    [016] ...1. 16904.184163: vfs_read <-__x64_sys_read
     data-loop.0-1997    [022] ...1. 16904.189426: vfs_read <-__x64_sys_read
     data-loop.0-2167    [023] ...1. 16904.189532: vfs_read <-__x64_sys_read
  pipewire-pulse-2162    [012] ...1. 16904.189580: vfs_read <-__x64_sys_read
     data-loop.0-1997    [022] ...1. 16904.189624: vfs_read <-__x64_sys_read
     threaded-ml-46239   [014] ...1. 16904.189800: vfs_read <-__x64_sys_read
     threaded-ml-46239   [014] ...1. 16904.189848: vfs_read <-__x64_sys_read
     threaded-ml-46239   [014] ...1. 16904.189906: vfs_read <-__x64_sys_read
     threaded-ml-46239   [014] ...1. 16904.189908: vfs_read <-__x64_sys_read
[root@shr-linux tracing] echo > trace
```

{{< admonition type=tip title="" open=true >}}
If you modify options like the set_trace_filter, it is a good practice to reset its
contents to the default after executing the script. There is no easy way to reset all
the files to their default. Otherwise you might see unexpected behavior. Its always
good to verify tracing first on a test system.
{{< /admonition >}}

## Tracing a module
It is also possible to trace the functions of a module. The module first needs to be
loaded into memory. The available modules can then be queried with:
```shell
[root@shr-linux tracing]# lsmod
Module                  Size  Used by
ccm                          20480  6
snd_seq_dummy                12288  0
rfcomm                      102400  4
snd_hrtimer                  12288  1
snd_seq                     139264  7 snd_seq_dummy
snd_seq_device               16384  1 snd_seq
qrtr                         57344  2
cmac                         12288  2
algif_hash                   12288  1
algif_skcipher               12288  1
af_alg                       32768  6 algif_hash,algif_skcipher
bnep                         32768  2
vfat                         24576  1
fat                         110592  1 vfat
snd_sof_pci_intel_tgl        16384  0
snd_sof_pci_intel_cnl        20480  1 snd_sof_pci_intel_tgl
snd_sof_intel_hda_generic    40960  2 snd_sof_pci_intel_tgl,snd_sof_pci_intel_cnl
...
```
The exported functions of the rfcomm module can then be determined with:
```shell
[root@shr-linux tracing]# cat available_filter_functions | grep '\[rfcomm\]'
rfcomm_l2state_change [rfcomm]
rfcomm_session_timeout [rfcomm]
rfcomm_l2data_ready [rfcomm]
rfcomm_dlc_debugfs_open [rfcomm]
rfcomm_dlc_debugfs_show [rfcomm]
rfcomm_apply_pn.isra.0 [rfcomm]
rfcomm_session_add [rfcomm]
...
```

## Tracing more than one function
The following approach can be used to trace an individual function:

```shell
[root@shr-linux tracing] echo "vfs_read" > set_ftrace_filter
[root@shr-linux tracing] echo "vfs_write" >> set_ftrace_filter
[root@shr-linux tracing] cat set_ftrace_filter
vfs_read
vfs_write
[root@shr-linux tracing] echo 1 > tracing_on
[root@shr-linux tracing] echo 0 > tracing_on
[root@shr-linux tracing] cat trace
# tracer: function
#
# entries-in-buffer/entries-written: 11865/11865   #P:24
#
#                                _-----=> irqs-off/BH-disabled
#                               / _----=> need-resched
#                              | / _---=> hardirq/softirq
#                              || / _--=> preempt-depth
#                              ||| / _-=> migrate-disable
#                              |||| /     delay
#           TASK-PID     CPU#  |||||  TIMESTAMP  FUNCTION
#              | |         |   |||||     |         |
           bash-42511    [008] ...1. 17270.742183: vfs_write <-__x64_sys_write
           bash-42511    [008] ...1. 17270.742272: vfs_write <-__x64_sys_write
           bash-42511    [008] ...1. 17270.742300: vfs_write <-__x64_sys_write
           sudo-42508    [010] ...1. 17270.742311: vfs_read <-__x64_sys_read
           sudo-42508    [010] ...1. 17270.742327: vfs_write <-__x64_sys_write
         konsole-2543    [012] ...1. 17270.742474: vfs_read <-__x64_sys_read
        HDMI-A-1-2000    [023] ...1. 17270.750494: vfs_write <-__x64_sys_write
    kwin_wayland-1847    [009] ...1. 17270.750540: vfs_read <-__x64_sys_read
    kwin_wayland-1847    [009] ...1. 17270.751749: vfs_read <-__x64_sys_read
         konsole-2543    [012] ...1. 17270.753188: vfs_write <-__x64_sys_write
         konsole-2543    [012] ...1. 17270.753302: vfs_read <-__x64_sys_read
     data-loop.0-1997    [020] ...1. 17270.760188: vfs_read <-__x64_sys_read
     data-loop.0-1997    [020] ...1. 17270.760228: vfs_write <-__x64_sys_write
     data-loop.0-2167    [021] ...1. 17270.760310: vfs_read <-__x64_sys_read
     data-loop.0-2167    [021] ...1. 17270.760329: vfs_write <-__x64_sys_write
     data-loop.0-2167    [021] ...1. 17270.760345: vfs_write <-__x64_sys_write
     data-loop.0-1997    [020] ...1. 17270.760411: vfs_read <-__x64_sys_read
  pipewire-pulse-2162    [006] ...1. 17270.760433: vfs_read <-__x64_sys_read

## Tracing one process
With the following approach a process can be traced:
```shell
[root@shr-linux tracing] echo $$ > set_ftrace_pid
[root@shr-linux tracing] echo 1 > tracing_on
[root@shr-linux tracing] top
[root@shr-linux tracing] echo 0 > tracing_on
[root@shr-linux tracing] cat trace | head -20
# tracer: function
#
# entries-in-buffer/entries-written: 21830/21830   #P:24
#
#                                _-----=> irqs-off/BH-disabled
#                               / _----=> need-resched
#                              | / _---=> hardirq/softirq
#                              || / _--=> preempt-depth
#                              ||| / _-=> migrate-disable
#                              |||| /     delay
#           TASK-PID     CPU#  |||||  TIMESTAMP  FUNCTION
#              | |         |   |||||     |         |
bash-42511   [010] ...1. 17663.960559: mutex_unlock <-rb_simple_write
bash-42511   [010] ...1. 17663.960561: syscall_exit_to_user_mode_prepare <-syscall_exit_to_user_mode
bash-42511   [010] ...1. 17663.960562: mem_cgroup_handle_over_high <-syscall_exit_to_user_mode
bash-42511   [010] ...1. 17663.960562: blkcg_maybe_throttle_current <-syscall_exit_to_user_mode
bash-42511   [010] ...1. 17663.960562: __rseq_handle_notify_resume <-syscall_exit_to_user_mode
bash-42511   [010] d..1. 17663.960564: switch_fpu_return <-syscall_exit_to_user_mode
bash-42511   [010] d..1. 17663.960564: restore_fpregs_from_fpstate <-switch_fpu_return
bash-42511   [010] ...1. 17663.960580: x64_sys_call <-do_syscall_64
bash-42511   [010] ...1. 17663.960580: __x64_sys_dup2 <-do_syscall_64
bash-42511   [010] ...1. 17663.960581: _raw_spin_lock <-__x64_sys_dup2
bash-42511   [010] ...2. 17663.960581: expand_files <-__x64_sys_dup2
bash-42511   [010] ...2. 17663.960582: do_dup2 <-__x64_sys_dup2
bash-42511   [010] ...2. 17663.960583: _raw_spin_unlock <-do_dup2
bash-42511   [010] ...1. 17663.960583: filp_close <-do_dup2
bash-42511   [010] ...1. 17663.960583: dnotify_flush <-filp_close
bash-42511   [010] ...1. 17663.960584: locks_remove_posix <-filp_close
bash-42511   [010] ...1. 17663.960584: fput <-filp_close
bash-42511   [010] ...1. 17663.960584: task_work_add <-fput
```
## Callers of a function
It is also possible to determine the callers of a function. To enable this feature the
`func_stack_trace` option is used.

```shell
[root@shr-linux tracing]# echo function > current_tracer
[root@shr-linux tracing]# echo vfs_read > set_ftrace_filter
[root@shr-linux tracing]# echo 1 > options/func_stack_trace
[root@shr-linux tracing]# echo 1 > tracing_on
[root@shr-linux tracing]# echo 0 > tracing_on
# tracer: function
#
# entries-in-buffer/entries-written: 11372/11372   #P:24
#
#                                _-----=> irqs-off/BH-disabled
#                               / _----=> need-resched
#                              | / _---=> hardirq/softirq
#                              || / _--=> preempt-depth
#                              ||| / _-=> migrate-disable
#                              |||| /     delay
#           TASK-PID     CPU#  |||||  TIMESTAMP  FUNCTION
#              | |         |   |||||     |         |
sudo-42508   [008] ..... 19523.441178: vfs_read <-__x64_sys_read
sudo-42508   [008] ..... 19523.441183: <stack trace>
=> crypto_ccm_module_init
=> vfs_read
=> __x64_sys_read
=> do_syscall_64
=> entry_SYSCALL_64_after_hwframe

konsole-2543    [012] ..... 19523.441328: vfs_read <-__x64_sys_read
konsole-2543    [012] ..... 19523.441333: <stack trace>
=> crypto_ccm_module_init
=> vfs_read
=> __x64_sys_read
=> do_syscall_64
=> entry_SYSCALL_64_after_hwframe
```
Here we have two call stacks. Both have the same call stack. We can see that this is
a system call, which then invokes the vfs_read function.

## Function graph tracer
To understand the code flow in a function / module, the function_graph tracer is very
helpful. It allows to see which functions are called in a hierarchic manner. An example
makes it easier to understand:

```shell
[root@shr-linux tracing] echo function_graph > current_tracer
[root@shr-linux tracing] echo 1 > tracing_on
[root@shr-linux tracing] ls
[root@shr-linux tracing] echo 0 > tracing_on
[root@shr-linux tracing] cat trace
# tracer: function_graph
#
# CPU  DURATION                  FUNCTION CALLS
# |     |   |                     |   |   |   |
19)               |  finish_task_switch.isra.0() {
19)   0.405 us    |    _raw_spin_unlock();
19)   3.156 us    |  }
19)               |  wq_worker_running() {
19)   0.227 us    |    kthread_data();
19)   0.950 us    |  }
19)   0.239 us    |  _raw_spin_lock_irq();
19)   0.282 us    |  move_linked_works();
19)               |  process_one_work() {
19)   0.239 us    |    kick_pool();
19)   0.223 us    |    _raw_spin_unlock_irq();
19)               |    vmstat_update() {
19)               |      refresh_cpu_vm_stats() {
19)   0.421 us    |        first_online_pgdat();
19)   0.260 us    |        decay_pcp_high();
19)   0.223 us    |        next_zone();
19)   0.249 us    |        decay_pcp_high();
19)   0.225 us    |        next_zone();
19)   0.228 us    |        decay_pcp_high();
19)   0.225 us    |        next_zone();
19)   0.223 us    |        next_zone();
19)   0.433 us    |        next_zone();
19)   0.223 us    |        first_online_pgdat();
19)   0.290 us    |        next_online_pgdat();
19)   7.753 us    |      }
19)   0.225 us    |      round_jiffies_relative();
19)               |      queue_delayed_work_on() {
19)               |        __queue_delayed_work() {
19)   0.315 us    |          housekeeping_enabled();
19)               |          add_timer_on() {
19)   0.242 us    |            _raw_spin_lock_irqsave();
19)   0.260 us    |            calc_wheel_index();
19)               |            enqueue_timer() {
19)   0.224 us    |              trigger_dyntick_cpu.constprop.0();
19)   0.943 us    |            }
19)   0.224 us    |            _raw_spin_unlock_irqrestore();
19)   2.934 us    |          }
19)   4.014 us    |        }
19)   4.658 us    |      }
19) + 13.819 us   |    }
19)   0.236 us    |    _raw_spin_lock_irq();
19)   0.288 us    |    pwq_dec_nr_in_flight();
19) + 16.804 us   |  }
```
The close parentheses shows when the call to the corresponding function finished. It also shows
how the duration of the function execution. If the function execution time exceeds certain limits,
it shows a n additional character like the '+' sign after the CPU column. Which sign it shows
depends on the duration of the function execution. The function_graph tracer also allows this to
be filtered to specific functions.

## Function layers / Tracing events
The functions in the kernel are divided into layers. The following command will return a list
of the existing layers.
```shell
[root@shr-linux tracing] trace-cmd list -e | awk -F: '{print $1}' | sort | uniq

alarmtimer
amd_cpu
asoc
avc
binder
block
bpf_test_run
bpf_trace
bridge
btrfs
..
```

It is possible to list all functions that belong to a specific layer:
```shell
[root@shr-linux tracing] trace-cmd list -e vmalloc

vmalloc:free_vmap_area_noflush
vmalloc:purge_vmap_area_lazy
vmalloc:alloc_vmap_area
```

## Tracing all functions calls to a certain layer
With the above information it is easy to trace all the function calls to a layer:
```shell
[root@shr-linux tracing] trace-cmd list -e | grep -i kmalloc

kmem:kmalloc

[root@shr-linux tracing] trace-cmd list -e kmem

kmem:rss_stat
kmem:mm_alloc_contig_migrate_range_info
kmem:mm_page_alloc_extfrag
kmem:mm_page_pcpu_drain
kmem:mm_page_alloc_zone_locked
kmem:mm_page_alloc
kmem:mm_page_free_batched
kmem:mm_page_free
kmem:kmem_cache_free
kmem:kfree
kmem:kmalloc
kmem:kmem_cache_alloc
```
The first command determines to which layer the kmalloc call belongs: this is the kmem module.
Then we query which other functions belong to the same layer. Now we can trace the calls to
the kmem layer.

```shell
[root@shr-linux tracing] echo "kmem*" > set_ftrace_filter
[root@shr-linux tracing] echo 1 > tracing_on

# wait a bit

[root@shr-linux tracing] echo 0 > tracing_on
[root@shr-linux tracing] cat trace | head -20
# tracer: function
#
# entries-in-buffer/entries-written: 18356/18356   #P:24
#
#                                        _-----=> irqs-off/BH-disabled
#                                       / _----=> need-resched
#                                      | / _---=> hardirq/softirq
#                                      || / _--=> preempt-depth
#                                      ||| / _-=> migrate-disable
#                                      |||| /     delay
#           TASK-PID             CPU#  |||||  TIMESTAMP  FUNCTION
#              | |                 |   |||||     |         |
            bash-42511           [012] ...1. 13490.249579: kmem_cache_free <-__fput
            bash-42511           [012] ...1. 13490.249579: kmem_cache_free <-task_work_run
            kwin_wayland-1847    [008] ...1. 13490.251524: kmem_cache_alloc_noprof <-__i915_request_create
            kwin_wayland-1847    [008] ...1. 13490.251570: kmem_cache_alloc_lru_noprof <-__d_alloc
            kwin_wayland-1847    [008] ...1. 13490.251573: kmem_cache_alloc_noprof <-alloc_empty_file
            kwin_wayland-1847    [008] ...1. 13490.251574: kmem_cache_alloc_noprof <-security_file_alloc
            kwin_wayland-1847    [008] ...1. 13490.251579: kmem_cache_free <-__dentry_kill
            kwin_wayland-1847    [008] ...1. 13490.251579: kmem_cache_free <-__fput
```
