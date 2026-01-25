---
title: "Breakdown of changes to Kernel Samepage Merging (KSM) by Kernel Version"
description: "Kernel Samepage Merging changes by kernel version"
date: 2024-12-14T00:02:53-07:00
featuredImage: "versions.jpg"
toc:
  enable: true
tags: ["linux", "memory", "ksm", "kernel samepage merging", "versions"]
draft: false 
---

This article describes how Linux Kernel Samepage Merging (KSM) has changed.
<!--more-->

## Overview
Kernel Samepage Merging has changed over the last couple of versions. This
document provides an overview of the changes by version.
The document will be updated as new kernel versions become available.

## 6.12
- [32f51ead3d77](https://git.kernel.org/torvalds/p/32f51ead3d77) mm: remove PageSwapCache 

## 6.11
> Optimization for fast changing pages
- [d58a361b0350](https://git.kernel.org/torvalds/p/d58a361b0350) mm/ksm: don't waste time searching stable tree for fast changing page

## 6.10
> Several commits to transition to folios.
- [c2dc78b86e08](https://git.kernel.org/torvalds/p/c2dc78b86e08) mm/ksm: fix ksm_zero_pages accounting
- [730cdc2c72c6](https://git.kernel.org/torvalds/p/730cdc2c72c6) mm/ksm: fix ksm_pages_scanned accounting

## 6.9
- No changes.

## 6.8
- [4e5fa4f5eff6](https://git.kernel.org/torvalds/p/4e5fa4f5eff6) mm/ksm: add ksm advisor
> Adds KSM advisor to dynamically adjust KSM settings.
- [66790e9a735b](https://git.kernel.org/torvalds/p/66790e9a735b) mm/ksm: add sysfs knobs for advisor 
- [5088b49730af](https://git.kernel.org/torvalds/p/5088b49730af) mm/ksm: add tracepoint for ksm advisor
> Start of work to transition to folios with several commits.

## 6.7
- [e5a689912689](https://git.kernel.org/torvalds/p/e5a689912689) mm/ksm: add pages_skipped metric
- [b0540208a59e](https://git.kernel.org/torvalds/p/b0540208a59e) mm/ksm: document pages_skipped sysfs knob
> Add new metric pages scanned in /sys/kernel/mm/ksm/pages_scanned.
- [5e924ff54d08](https://git.kernel.org/torvalds/p/5e924ff54d08) mm/ksm: add "smart" page scanning mode
- [75d7dd4138ed](https://git.kernel.org/torvalds/p/75d7dd4138ed) mm/ksm: document smart scan mode
> Add "smart scan" mode to skip pages that have not been de-duplicated in previous scans.

## 6.6
- [8b47933544a6](https://git.kernel.org/torvalds/p/8b47933544a6) proc/ksm: add ksm stats to /proc/pid/smaps
> Add KSM information to /proc/\<pid>/smaps and /proc/\<pid>/smaps_rollup
- [d256d1cd8da1](https://git.kernel.org/torvalds/p/d256d1cd8da1) mm: memory-failure: use rcu lock instead of tasklist_lock when collect_procs()
- [5994eabf3bbb](https://git.kernel.org/torvalds/p/5994eabf3bbb) merge mm-hotfixes-stable into mm-stable to pick up depended-upon changes
- [b348b5fe2b5f](https://git.kernel.org/torvalds/p/b348b5fe2b5f) mm/ksm: add pages scanned metric
> Expose pages scanned metric in /sys/kernel/mm/ksm/pages_scanned
- [1a8e84305783](https://git.kernel.org/torvalds/p/1a8e84305783) ksm: consider KSM-placed zeropages when calculating KSM profit
> Include zero pages when calculating /sys/kernel/mm/ksm/general_profit
- [6080d19f0704](https://git.kernel.org/torvalds/p/6080d19f0704) ksm: add ksm zero pages for each process
- [e2942062e01d](https://git.kernel.org/torvalds/p/e2942062e01d) ksm: count all zero pages placed by KSM
- [79271476b336](https://git.kernel.org/torvalds/p/79271476b336) ksm: support unsharing KSM-placed zero pages

## 6.5
- [49b0638502da](https://git.kernel.org/torvalds/p/49b0638502da) mm: enable page walking API to lock vmas during the walk
- [f985fc322063](https://git.kernel.org/torvalds/p/f985fc322063) mm/swapfile: fix wrong swap entry type for hwpoisoned swapcache page
- [1fec6890bf22](https://git.kernel.org/torvalds/p/1fec6890bf22) mm: remove references to pagevec
- [c33c794828f2](https://git.kernel.org/torvalds/p/c33c794828f2) mm: ptep_get() conversion
- [04dee9e85cf5](https://git.kernel.org/torvalds/p/04dee9e85cf5) mm/various: give up if pte_offset_map[_lock]() fails
- [26e1a0c3277d](https://git.kernel.org/torvalds/p/26e1a0c3277d) mm: use pmdp_get_lockless() without surplus barrier()

## 6.4
- [2c281f54f556](https://git.kernel.org/torvalds/p/2c281f54f556) mm/ksm: move disabling KSM from s390/gmap code to KSM code
- [24139c07f413](https://git.kernel.org/torvalds/p/24139c07f413) mm/ksm: unmerge and clear VM_MERGEABLE when setting PR_SET_MEMORY_MERGE=0
- [d21077fbc2fc](https://git.kernel.org/torvalds/p/d21077fbc2fc) mm: add new KSM process and sysfs knobs
> - expose new general_profit metric /sys/kernel/mm/ksm/general_profit
> - expose process_profit metric in /proc/<pid>/ksm_stat
> - expose ksm_merging_pages metric in /proc/<pid>/ksm_stat
- [d7597f59d1d3](https://git.kernel.org/torvalds/p/d7597f59d1d3) mm: add new api to enable ksm per process
> new prctl api to enable KSM per process
- [4248d0083ec5](https://git.kernel.org/torvalds/p/4248d0083ec5) mm: ksm: support hwpoison for ksm page
- [739100c88f49](https://git.kernel.org/torvalds/p/739100c88f49) mm: add tracepoints to ksm
> KSM tracepoints for scan start and stop

## 6.3
- [6db504ce55bd](https://kernel.git.org/torvalds/p/6db504ce55bd) mm/ksm: fix race with VMA iteration and mm_struct teardown
- [f67d6b266493](https://kernel.git.org/torvalds/p/f67d6b266493) Merge branch 'mm-hotfixes-stable' into mm-stable
- [7d4a8be0c4b2](https://kernel.git.org/torvalds/p/7d4a8be0c4b2) mm/mmu_notifier: remove unused mmu_notifier_range_update_to_read_only export

## 6.2
- [b970599e807](https://git.kernel.org/torvalds/p/b970599e807) mm: hwpoison: support recovery from ksm_might_need_to_copy()
- [d7c0e68dab98](https://git.kernel.org/torvalds/p/d7c0e68dab98) mm/ksm: convert break_ksm() to use walk_page_range_vma()
- [6cce3314b928](https://git.kernel.org/torvalds/p/6cce3314b928) mm/ksm: fix KSM COW breaking with userfaultfd-wp via FAULT_FLAG_UNSHARE
- [58f595c66591](https://git.kernel.org/torvalds/p/58f595c66591) mm/ksm: simplify break_ksm() to not rely on VM_FAULT_WRITE
- [6a56ccbcf6c6](https://git.kernel.org/torvalds/p/6a56ccbcf6c6) mm/autonuma: use can_change_(pte|pmd)_writable() to replace savedwrite
- [1eeaa4fd39b0](https://git.kernel.org/torvalds/p/1eeaa4fd39b0) memory: move hotplug memory notifier priority to same file for easy sorting

## 6.1
- [b4e6f66e45b4](https://git.kernel.org/torvalds/p/b4e6f66e45b4) ksm: use a folio in replace_page()
- [58730ab6c7ca](https://git.kernel.org/torvalds/p/58730ab6c7ca) ksm: convert to use common struct mm_slot
- [79b099415637](https://git.kernel.org/torvalds/p/79b099415637) ksm: convert ksm_mm_slot.link to ksm_mm_slot.hash
- [23f746e412b4](https://git.kernel.org/torvalds/p/23f746e412b4) ksm: convert ksm_mm_slot.mm_list to ksm_mm_slot.mm_node
- [21fbd59136e0](https://git.kernel.org/torvalds/p/21fbd59136e0) ksm: add the ksm prefix to the names of the ksm private structures
- [cb4df4cae4f2](https://git.kernel.org/torvalds/p/cb4df4cae4f2) ksm: count allocated ksm rmap_items for each process
>  Added /proc/\<pid>/ksm_stat to report ksm_rmap_items.
- [f7091ed64ec8](https://git.kernel.org/torvalds/p/f7091ed64ec8) mm: fix the handling Non-LRU pages returned by follow_page
- [a5f18ba07276](https://git.kernel.org/torvalds/p/a5f18ba07276) mm/ksm: use vma iterators instead of vma linked list
- [088b8aa537c2](https://git.kernel.org/torvalds/p/088b8aa537c2) mm: fix PageAnonExclusive clearing racing with concurrent RCU GUP-fast
- [507228044236](https://git.kernel.org/torvalds/p/507228044236) mm/khugepaged: record SCAN_PMD_MAPPED when scan_pmd() finds hugepage

## 6.0
- [3218f8712d6b](https://git.kernel.org/torvalds/p/3218f8712d6b) mm: handling Non-LRU pages returned by vm_normal_pages
- [9800562f2ab4](https://git.kernel.org/torvalds/p/9800562f2ab4) mm/folio-compat: Remove migration compatibility functions


