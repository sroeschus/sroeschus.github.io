---
title: "Introduction to kernel tracing using trace-cmd"
date: 2025-01-07T19:33:05-08:00
featuredImage: "trace-cmd.jpg"
toc:
  enable: true
tags: ["linux", "kernel", "tracing", "ftrace", "trace-cmd"]
draft: false
---

This article describes how to use the ftrace kernel framework with the trace-cmd client
program.
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
[root@shr-linux tracing] trace-cmd list -t
timerlat osnoise hwlat blk mmiotrace branch function_graph wakeup_dl wakeup_rt wakeup
preemptirqsoff preemptoffirqsoff function nop
```
The list of tracers is described [here](https://kernel.org/doc/html/v6.12/trace/ftrace.html#the-tracers).
In addition to to the tracers also events can be traced.

## Clients
There are two ways to configure and use the ftrace framework:
- the original way by directly manipulating the files in the /sys/kernel/tracing directory
- the new way by using the [trace-cmd](https://trace-cmd.org/).

This article focuses on trace-cmd.

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

## Type of tracing:
There are two types of function tracing available:
- trace-cmd start / trace-cmd show, invokes without recording to a trace file and
- trace-cmd record / trace-cmd report records to a trace file

This guide focuses mostly on the second one. Trace-cmd start and trace-cmd report are
very similar. The difference is that trace-cmd record supports a few more options.

## Function tracer
The function tracer is the most simple tracer. The general approach is to start the tracer
let it run for a few seconds or a certain time period, stop the trace collection, look at
the output and then clear the trace buffers.

To user the trace buffers the following steps can be used. The `-p` flag determines which
tracer is used (sometimes the tracer is also called a plugin).
```shell
[root@shr-linux tracing] trace-cmd record -p function
plugin 'function'
Hit Ctrl^C to stop recording
CPU0 data recorded at offset=0x1f2000
47699 bytes in size (262144 uncompressed)
CPU1 data recorded at offset=0x1fe000
56159 bytes in size (286720 uncompressed)
CPU2 data recorded at offset=0x20c000
45343 bytes in size (221184 uncompressed)
CPU3 data recorded at offset=0x218000
34753 bytes in size (180224 uncompressed)
CPU4 data recorded at offset=0x221000
42052 bytes in size (208896 uncompressed)
CPU5 data recorded at offset=0x22c000
72769 bytes in size (352256 uncompressed)
CPU6 data recorded at offset=0x23e000
60670 bytes in size (290816 uncompressed)
CPU7 data recorded at offset=0x24d000
51192 bytes in size (258048 uncompressed)
^C

# wait a bit

[root@shr-linux tracing] trace-cmd report
cpus=8
trace-cmd-673   [005] ...1.   857.988849: function:             mutex_unlock <-- rb_simple_write
trace-cmd-673   [005] ...1.   857.988849: function:             __mutex_unlock_slowpath.isra.0 <-- rb_simple_write
trace-cmd-673   [005] ...1.   857.988850: function:             _raw_spin_lock_irqsave <-- __mutex_unlock_slowpath.isra.0
trace-cmd-673   [005] d..1.   857.988850: function:             preempt_count_add <-- _raw_spin_lock_irqsave
trace-cmd-673   [005] d..2.   857.988850: function:             wake_q_add <-- __mutex_unlock_slowpath.isra.0
trace-cmd-673   [005] d..2.   857.988851: function:             preempt_count_add <-- __mutex_unlock_slowpath.isra.0
trace-cmd-673   [005] d..3.   857.988852: function:             _raw_spin_unlock_irqrestore <-- __mutex_unlock_slowpath.isra.0
trace-cmd-673   [005] ...3.   857.988852: function:             preempt_count_sub <-- _raw_spin_unlock_irqrestore
trace-cmd-673   [005] ...2.   857.988852: function:             wake_up_q <-- __mutex_unlock_slowpath.isra.0
trace-cmd-673   [005] ...2.   857.988852: function:             try_to_wake_up <-- wake_up_q
trace-cmd-673   [005] ...2.   857.988852: function:             preempt_count_add <-- try_to_wake_up
trace-cmd-673   [005] ...3.   857.988853: function:             _raw_spin_lock_irqsave <-- try_to_wake_up
trace-cmd-673   [005] d..3.   857.988853: function:             preempt_count_add <-- _raw_spin_lock_irqsave
trace-cmd-673   [005] d..4.   857.988854: function:             select_task_rq_fair <-- try_to_wake_up
trace-cmd-673   [005] d..4.   857.988854: function:             __rcu_read_lock <-- select_task_rq_fair
trace-cmd-673   [005] d..4.   857.988854: function:             available_idle_cpu <-- select_task_rq_fair
trace-cmd-673   [005] d..4.   857.988854: function:             available_idle_cpu <-- select_task_rq_fair
[root@shr-linux tracing] trace-cmd reset
```
The first line shows that we have 8 CPU's on this system (line 25).

Beginning from line 26, the trace ouput starts. Each line consists of task/pid, on which CPU
it was executed, the latency trace information, timestamp/delay information and which function was
executed.

Line 43 clears the trace buffer.

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
[root@shr-linux tracing] trace-cmd list -f | head -20

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

If we are interested in the vfs functions, the following can be used instead:
```shell
[root@shr-linux tracing] trace-cmd list -f vfs | grep read

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
[root@shr-linux tracing] trace-cmd record -p function -l "vfs_read"
[root@shr-linux tracing] trace-cmd report | less
  make-15095 [003] ...1.  1009.638518: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.646243: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.646501: function:             vfs_read <-- ksys_pread64
  make-15095 [003] ...1.  1009.646510: function:             vfs_read <-- ksys_pread64
  make-15095 [003] ...1.  1009.648015: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.648916: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.649689: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.650693: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.651658: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.652618: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.662880: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.663284: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.663358: function:             vfs_read <-- ksys_read
  make-15095 [003] ...1.  1009.663784: function:             vfs_read <-- ksys_read
[root@shr-linux tracing] trace-cmd reset
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
[root@shr-linux tracing] lsmod
Module                  Size  Used by
kvm_intel             372736  0
evdev                  28672  0
kvm                  1585152  1 kvm_intel
button                 20480  0
virtio_mmio            16384  0
virtiofs               36864  1
fuse                  278528  1 virtiofs
```
The exported functions of the rfcomm module can then be determined with:
```shell
[root@shr-linux tracing] trace-cmd list -f 'kvm_intel]'
vmx_update_fb_clear_dis [kvm_intel]
handle_machine_check [kvm_intel]
handle_triple_fault [kvm_intel]
handle_bus_lock_vmexit [kvm_intel]
vmx_get_passthrough_msr_slot [kvm_intel]
handle_vmx_instruction [kvm_intel]
handle_preemption_timer [kvm_intel]
handle_tpr_below_threshold [kvm_intel]
vmentry_l1d_flush_get [kvm_intel]
vmx_setup_l1d_flush [kvm_intel]
vmentry_l1d_flush_set [kvm_intel]
...
```

## Tracing more than one function
The following approach can be used to trace more than one function. The -l option
can be specified several times to specify more than one function. In the output
it shows that both functions are traced.

```shell
[root@shr-linux tracing] trace-cmd record -p function -l vfs_read -l vfs_write
[root@shr-linux tracing] trace-cmd report | less
      make-15732 [006] ...1.  1309.444258: function:             vfs_read <-- ksys_read
      make-15732 [006] ...1.  1309.446765: function:             vfs_read <-- ksys_read
        sh-15733 [003] ...1.  1309.448853: function:             vfs_read <-- ksys_read
        sh-15733 [003] ...1.  1309.450523: function:             vfs_read <-- ksys_read
        sh-15733 [003] ...1.  1309.450601: function:             vfs_read <-- ksys_pread64
        sh-15733 [003] ...1.  1309.450604: function:             vfs_read <-- ksys_pread64
        sh-15733 [003] ...1.  1309.451557: function:             vfs_read <-- ksys_read
       cat-15734 [000] ...1.  1309.462952: function:             vfs_read <-- ksys_read
       cat-15734 [000] ...1.  1309.463069: function:             vfs_read <-- ksys_pread64
       cat-15734 [000] ...1.  1309.463084: function:             vfs_read <-- ksys_pread64
       cat-15734 [000] ...1.  1309.467142: function:             vfs_read <-- ksys_read
       cat-15734 [000] ...1.  1309.467210: function:             vfs_write <-- ksys_write
       cat-15734 [000] ...1.  1309.467218: function:             vfs_read <-- ksys_read
      make-15732 [006] ...1.  1309.467257: function:             vfs_read <-- ksys_read
```

## Tracing one process
With the following approach a process can be traced:
```shell
[root@shr-linux tracing] trace-cmd record -p function -P 42511
[root@shr-linux tracing] trace-cmd report | head -20
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
[root@shr-linux tracing] trace-cmd record -p function -l vfs_read --func-stack
[root@shr-linux tracing] trace-cmd  report | less
make-16227 [005] .....  1637.465848: kernel_stack:          => 0x7fffffff
=> vfs_read
=> ksys_read
=> do_syscall_64
=> entry_SYSCALL_64_after_hwframe
=> 0x0
=> 0x0
=> 0x0
make-16227 [005] .....  1637.472243: function:             vfs_read <-- ksys_read
make-16227 [005] .....  1637.472249: kernel_stack:          => 0x7fffffff
=> vfs_read
=> ksys_read
=> do_syscall_64
=> entry_SYSCALL_64_after_hwframe
=> 0x0
=> 0x0
=> 0x0
make-16227 [005] .....  1637.472437: function:             vfs_read <-- ksys_pread64
make-16227 [005] .....  1637.472442: kernel_stack:          => 0x7fffffff
```
Here we have two call stacks. Both have the same call stack. We can see that this is
a system call, which then invokes the vfs_read function.

## Function graph tracer
To understand the code flow in a function / module, the function_graph tracer is very
helpful. It allows to see which functions are called in a hierarchic manner. An example
makes it easier to understand:

```shell
[root@shr-linux tracing] trace-cmd record -p function_graph
[root@shr-linux tracing] trace-cmd report | less
make-16719 [003] ...2.  1876.648058: funcgraph_entry:        0.237 us   |  preempt_count_sub();
make-16719 [003] ...1.  1876.648059: funcgraph_entry:                   |  __f_unlock_pos() {
make-16719 [003] ...1.  1876.648059: funcgraph_entry:        0.225 us   |    mutex_unlock();
make-16719 [003] .....  1876.648059: funcgraph_exit:         0.642 us   |  }
make-16719 [003] ...1.  1876.648060: funcgraph_entry:                   |  x64_sys_call() {
make-16719 [003] ...1.  1876.648061: funcgraph_entry:                   |    __x64_sys_execve() {
make-16719 [003] ...1.  1876.648061: funcgraph_entry:                   |      getname() {
make-16719 [003] ...1.  1876.648061: funcgraph_entry:                   |        getname_flags.part.0() {
make-16719 [003] ...1.  1876.648062: funcgraph_entry:                   |          kmem_cache_alloc_noprof() {
make-16719 [003] ...1.  1876.648062: funcgraph_entry:        0.197 us   |            __cond_resched();
make-16719 [003] .....  1876.648062: funcgraph_exit:         0.801 us   |          }
make-16719 [003] .....  1876.648063: funcgraph_exit:         1.338 us   |        }
make-16719 [003] .....  1876.648063: funcgraph_exit:         1.768 us   |      }
make-16719 [003] ...1.  1876.648063: funcgraph_entry:                   |      do_execveat_common.isra.0() {
make-16719 [003] ...1.  1876.648064: funcgraph_entry:                   |        alloc_bprm() {
make-16719 [003] ...1.  1876.648065: funcgraph_entry:                   |          do_open_execat() {
make-16719 [003] ...1.  1876.648065: funcgraph_entry:                   |            do_filp_open() {
make-16719 [003] ...1.  1876.648065: funcgraph_entry:                   |              path_openat() {
make-16719 [003] ...1.  1876.648065: funcgraph_entry:                   |                alloc_empty_file() {
make-16719 [003] ...1.  1876.648066: funcgraph_entry:                   |                  kmem_cache_alloc_noprof() {
make-16719 [003] ...1.  1876.648066: funcgraph_entry:        0.195 us   |                    __cond_resched();
make-16719 [003] .....  1876.648066: funcgraph_exit:         0.769 us   |                  }
make-16719 [003] ...1.  1876.648067: funcgraph_entry:                   |                  init_file() {
make-16719 [003] ...1.  1876.648067: funcgraph_entry:                   |                    security_file_alloc() {
make-16719 [003] ...1.  1876.648067: funcgraph_entry:                   |                      kmem_cache_alloc_noprof() {
make-16719 [003] ...1.  1876.648067: funcgraph_entry:        0.195 us   |                        __cond_resched();
make-16719 [003] .....  1876.648068: funcgraph_exit:         0.785 us   |                      }
make-16719 [003] ...1.  1876.648068: funcgraph_entry:        0.184 us   |                      selinux_file_alloc_security();
make-16719 [003] .....  1876.648069: funcgraph_exit:         1.672 us   |                    }
```
The close parentheses shows when the call to the corresponding function finished. It also shows
how the duration of the function execution. If the function execution time exceeds certain limits,
it shows a n additional character like the '+' sign after the CPU column. Which sign it shows
depends on the duration of the function execution. The function_graph tracer also allows this to
be filtered to specific functions.

With the -g option only a specific function can be recorded. It is also possible to limit
the depth that is captured with the --max-graph-depth option.

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
[root@shr-linux tracing] trace-cmd record -p nop -e 'kmem:*'

# wait a bit

[root@shr-linux tracing] trace-cmd report | less
make-16810 [004] .....  2319.000857: mm_page_alloc:        page=0xffffea00001347a7 pfn=0x1347a7 order=0 migratetype=1 gfp_flags=0x152c4a
make-16810 [004] .....  2319.000861: mm_page_alloc:        page=0xffffea0000111ed0 pfn=0x111ed0 order=0 migratetype=1 gfp_flags=0x152c4a
make-16810 [004] d..1.  2319.000863: kmem_cache_alloc:     call_site=xas_alloc+0x9f ptr=0xffff88811390f6c0 bytes_req=576 bytes_alloc=584 gfp_flags=0x402800 node=-1 accounted=true
make-16810 [004] .....  2319.000866: mm_page_alloc:        page=0xffffea000011b7da pfn=0x11b7da order=0 migratetype=1 gfp_flags=0x152c4a
make-16810 [004] .....  2319.000869: mm_page_alloc:        page=0xffffea0000123de6 pfn=0x123de6 order=0 migratetype=1 gfp_flags=0x152c4a
make-16810 [004] .....  2319.000873: kmalloc:              call_site=fuse_io_alloc+0x41 ptr=0xffff888101aee200 bytes_req=216 bytes_alloc=256 gfp_flags=0xdc0 node=-1 accounted=false
make-16810 [004] .....  2319.000873: kmalloc:              call_site=fuse_io_alloc+0x64 ptr=0xffff888101a9d8c0 bytes_req=64 bytes_alloc=64 gfp_flags=0xdc0 node=-1 accounted=false
make-16810 [004] .....  2319.000876: kmem_cache_alloc:     call_site=fuse_request_alloc+0x1c ptr=0xffff888100ed5c78 bytes_req=152 bytes_alloc=152 gfp_flags=0xdc0 node=-1 accounted=false
make-16810 [004] ...1.  2319.000878: kmalloc:              call_site=virtio_fs_enqueue_req+0x131 ptr=0xffff888101a9d9c0 bytes_req=56bytes_alloc=64 gfp_flags=0x820 node=-1 accounted=false
make-16810 [004] ...1.  2319.000878: kmalloc:              call_site=virtio_fs_enqueue_req+0x16c ptr=0xffff888101aeeb00 bytes_req=224 bytes_alloc=256 gfp_flags=0x820 node=-1 accounted=false
make-16810 [004] ...1.  2319.000879: kmalloc:              call_site=virtio_fs_enqueue_req+0x1eb ptr=0xffff888101a9d840 bytes_req=40bytes_alloc=64 gfp_flags=0x820 node=-1 accounted=false
make-16810 [004] ...2.  2319.000881: kmalloc:              call_site=virtqueue_add_split+0x119 ptr=0xffff888101aef100 bytes_req=224 
```
