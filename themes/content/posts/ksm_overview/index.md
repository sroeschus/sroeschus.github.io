---
title: "Linux Kernel Samepage Merging (KSM) Overview"
description: "Using Linux Kernel Samepage Merging"
date: 2023-10-04T20:03:53-07:00
featuredImage: "memory.jpg"
toc:
  enable: true
tags: ["linux", "memory", "ksm", "kernel samepage merging"]
draft: false
---

This article describes how to use the Linux Kernel Samepage Merging (KSM).
<!--more-->

## Overview
Kernel Samepage Merging (KSM) is a memory saving de-duplication feature for
anonymous memory. KSM was originally developed for KVM (Kernel based virtual
machine). The idea was to fit a higher number of virtual machines on a host
by sharing anonymous memory pages. In the meantime also other workloads have
shown to benefit from using KSM.

The general approach is to share anonymous pages of different processes that have
the same content. Instead of having several pages with the same content, 
a KSM page is created that is shared by all the processes. This is achieved
by pointing the page table entries of all the pages to this KSM page.
The KSM page is marked as copy-on-write (COW). In case one of the processes
that shares this page wants to modify the page, a duplicate page is created,
that the process can then modify. This is no longer a so-called KSM page.

In case a KSM page is selected for swap-out, the page is written as an ordinary
page to the swap space. When the page is swapped in later, this is no longer
a KSM page. However if a duplicate page is found, it can be shared again.

The process of finding duplicate pages is executed by a background kernel
thread. The background kernel thread is called ksmd. When no processes have
requested KSM sharing, the background thread is sleeping and not consuming
CPU resources.
To request KSM, a process can use the [madvise](https://man7.org/linux/man-pages/man2/madvise.2.html)
system call to nominate an area of memory that should use KSM. Recently
(with Linux Kernel version 6.4)
also a new option has been added to the prctl system call. It allows to
enable for KSM for all the memory regions of a process.

## Parameters for controlling ksmd
### Enabling KSM
KSM needs to be enabled for memory regions or processes, but in addition
KSM also needs to be enabled globally. To globally enable KSM, the contents
of the file ``/sys/kernel/mm/ksm/run`` need to be changed:
- 0: stop ksmd
- 1: enable ksmd
- 2: disable ksmd and unmerge all KSM pages, but keep memory regions registered

### Scan speed
How aggressive the kernel background thread scans pages can be controlled
with two parameters: ``pages_to_scan`` and ``sleep_millisecs``. They can be set by
changing the values in the directory ``/sys/kernel/mm/ksm``. The default value
for pages_to_scan is 100. This is too low for all practical purposes and
should only be used for demos and tests. For real workloads this value 
should be set to at least 500 to 1000. If ksmd has to scan a lot of pages
values of 5000 can be required.

Pages are scanned in batches. The value pages_to_scan determines how many
pages are scanned in one batch. Between the individual batches, the ksmd
kernel thread sleeps. The sleep time is determined by the value of the
parameter sleep_millisecs. The default value is 20ms.

### Numa support
By default pages from different Numa nodes can be merged. The default value
for the parameter ``merge_across_nodes`` is set to 1. When set to 0
only pages on the same numa node are merged. The setting can only be changed
when no KSM pages are shared.

### Zero page support
This provides support for empty pages. When enabled empty pages are shared
with the kernel zero pages. It can potentially degrade the performance. The
name of the parameter is ``use_zero_pages``. The default is off (0).

## Enabling KSM for a memory region
To enable ksm for a memory region, the [madvise](https://man7.org/linux/man-pages/man2/madvise.2.html)
system call can be used. The madvise system call provides two options: 
MADV_MERGEABLE and MADV_UNMERGEABLE, to enable or disable KSM for a memory
region. To enable KSM the following code can be used:

```C
    madvise(address, length, MADV_MERGEABLE);
```
and to disable KSM for memory region this code can be used:

```C
    madvise(address, length, MADV_UNMERGEABLE);
```

## Enabling KSM for a process
A recent addition is the ability to enable KSM for a process. This makes it
easier to use KSM, as it allows to enable KSM for all compatible virtual
memory areas of a process. To make a virtual memory area (VMA) compatible it
must fullfill certain requirements (this is the same if madvise is used).
In general, if a VMA is marked as shared, it cannot be used for KSM. There are
additional reasons. For those that are interested, have a look at the
[vma_ksm_compatible](https://elixir.bootlin.com/linux/latest/source/mm/ksm.c#L526)
function.

KSM can be enabled for the process, by using the following code:
```C
    prctl(PR_SET_MEMORY_MERGE, 1, 0, 0, 0);
```
The second parameter is used to enable KSM. In addition one can also query
if KSM has been enabled with the option ``PR_GET_MEMORY_MERGE``.

## Monitoring KSM

### Monitoring with /sys/kernel/mm/ksm
KSM can be monitored. Several values are exposed in the directory /sys/kernel/mm/ksm.
To get a general overview how useful KSM is, have a look at the ouput of the file
``general_profit``. It shows if KSM is beneficial for the workload.

```shell
[root@d8120 /sys/kernel/mm/ksm]# cat general_profit
6461704512
```
In the above case over 6GB of memory have been de-duplicated. So in this case considerable
memory savings have been achieved.

``pages_shared`` reports the number of KSM pages and ``pages_sharing`` reports the
number of pages that refer to them. ``pages_volatile`` gives an indication how many
pages change too fast and cannot be used for KSM sharing. The file ``full_scans``
shows how many scans have been performed so far.

With Linux kernel version 6.5 the ``pages_scanned`` metric has been added. Comparing
the ``pages_scanned`` metric to ``pages_sharing`` gives a good indication how effective
KSM is.

### Monitoring with /proc/\<pid>
The proc filesystem contains two files: ``ksm_stat`` and ``ksm_merging_pages``. The
file ``ksm_merging_pages`` reports how many pages have been de-duplicated for this
process. The file ``ksm_stat`` reports the number of merged pages, the number of rmap
items and the profit for the process. rmap items is an internal data structure of ksm.
For every KSM candidate page an rmap item is created: so this shows the number of KSM
candidate pages of this process. The process profit calculates how much memory has
been saved by enabling KSM for this process.

```shell
[root@d8120 ~]# cat /proc/2736222/ksm_stat
ksm_rmap_items 613809
ksm_merging_pages 117015
ksm_process_profit 440009664
[root@d8120 ~]# cat /proc/2736222/ksm_merging_pages
117015
```

With Linux Kernel 6.6 there is an new addition to ``/proc/<pid>/smaps`` and
``/proc/<pid>/smaps_rollup``. It now contains an additional line that reports how much
memory has been de-duplicated by VMA. This information makes it possible to identify
which VMA's benefit the most from using KSM. A useful approach might be first to enable
KSM for the whole process with prctl, then run the workload and identify those VMA's
that benefit the most and selectively enable KSM for those VMA's with madvise calls.

```shell
f73d2e00000-7f740ae00000 rw-p 00000000 00:00 0
Size:             917504 kB
KernelPageSize:        4 kB
MMUPageSize:           4 kB
Rss:              262828 kB
Pss:               19496 kB
Shared_Clean:       2680 kB
Shared_Dirty:     248624 kB
Private_Clean:       268 kB
Private_Dirty:     11256 kB
Referenced:       214544 kB
Anonymous:        262828 kB
KSM:               96376 kB
LazyFree:              0 kB
AnonHugePages:         0 kB
ShmemPmdMapped:        0 kB
FilePmdMapped:         0 kB
Shared_Hugetlb:        0 kB
Private_Hugetlb:       0 kB
Swap:             635356 kB
SwapPss:           26143 kB
Locked:                0 kB
THPeligible:    0
ProtectionKey:         0
ksm_state:          0
ksm_skip_base:      0
ksm_skip_count:     0
VmFlags: rd wr mr mw me nr mg anon
```

## Tracing KSM operations
With Linux kernel version 6.3 tracing for the KSM layer has been added. The new tracing
events are available under ``/sys/kernel/tracing/events/ksm``. The following operations
can now be traced:
- ``ksm_start_scan``: start of a new scan
- ``ksm_stop_scan``: stop of a new scan
- ``ksm_enter``: merge a new memory region
- ``ksm_exit``: unmerge a memory region
- ``ksm_merge_one_page``: merge a KSM page
- ``ksm_merge_with_ksm_page``: merge another page with an existing KSM page
- ``ksm_remove_ksm_page``: remove a KSM page
- ``ksm_remove_rmap_item``: delete an rmap item

The trace events are defined in [include/trace/events/ksm.h](https://elixir.bootlin.com/linux/v6.5.5/source/include/trace/events/ksm.h).

## Limitations
KSM generally works best if the page size is smaller. The smaller the page size, the
higher the likelyhood of being able to share pages. It can be expected that KSM works
well on the x86 platform with 4KB default page size. KSM is not enabled for huge pages.

Security needs to be taken into consideration before enabling KSM. KSM can be abused to
execute side channel attacks. There are several papers who investigate side channel
attacks with KSM:

- [A memory-deduplication side channel attack...](https://svs.informatik.uni-hamburg.de/publications/2018/2018-04-10-Lindemann-Memory-Deduplication-Side-Channel.pdf)
- [Dedup Est Machina: Memory deduplication as an advanced expoitation vector](https://ieeexplore.ieee.org/document/7546546)

There are additional papers in that area, but this should get you started on the
subject.
