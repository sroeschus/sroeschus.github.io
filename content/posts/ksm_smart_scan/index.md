---
title: "Kernel Samepage Merging Smart Scan feature"
description: "Explaining the Smart Scan feature for Linux Kernel Samepage Merging"
featuredImage: "smart_scan.jpg"
date: 2024-09-13T11:51:50-07:00
toc:
  enable: true
tags: ["linux", "kernel", "samepage", "merging", "KSM", "advisor", "smart scan"]
draft: false
---

This article describes the Smart Scan feature of Linux Kernel Samepage Merging (KSM).
<!--more-->

## Overview / Motivation
KSM (Kernel samepage merging) can help in reducing the memory footprint.
However the KSM parameters need to be set correctly for the feature. There
are two challenges with this:
- Parameters need to be set appropriately, so pages are scanned fast enough
- Parameter settings need to change over time

### Appropriate setting of parameters
How aggressive the candidate pages are scanned is dependent on several parameters.
The most important parameter is `pages_to_scan`.

### Changing parameter values over time
Most workloads consist of a huge number of pages after the process has
started. Once the workload has stabilized, the amount of pages that need
to be analyzed is generally reduced.

Not all workloads have this behavior, but it can be observed for a lot of
workloads and especially the workloads that have a bigger working set.

What does this mean for KSM? It implies that when an application or an
application suite is started, it has its advantages to scan more aggresively.
Once the application enters "steady state", it makes sense to reduce the
scan rate.

## Smart Scan
The above is the foundation of "smart scan". The user specifies a target scan
time and the Smart Scan feature adapts the `pages_to_scan` setting accordingly.
The target scan time parameter is called `advisor_target_scan_time`.

## Algorithm
The algorithm to automatically calculate the scan rate is using a exponentially
weighted moving average. In simple terms the formula is the following:

> new_pages_to_scan = pages_to_scan * (scan_time / target_scan_time)

To avoid pertubations, it calculates a new change factor to the previous value. 
The new change factor is subject to the exponentially weighted moving average.

Once the new value for pages_to_scan has been calculated, it is limited by
the max values for CPU cost and pages_to_scan, which are explained below.

## How to turn on Smart Scan
The smart-scan feature supports two modes: "`none`" and "`scan-time`". Either the
feature is disabled or the `scan-time` mode is enabled. Smart Scan can be
enabled by the following command:

```shell
cat "scan-time" > /sys/kernel/mm/ksm/advisor_mode
```

By default the Smart Scan feature is disabled.

## KSM knobs
KSM can be very CPU intensive. The KSM advisor provides additional knobs in
`/sys/kernel/mm/ksm` to limit the scan rate. The following knobs are available:
- `advisor_max_cpu`
- `advisor_min_pages_to_scan`
- `advisor_max_pages_to_scan`

## Optimizations
The smart scan feature also adds an optimization: it takes the history of
scanning pages into account and skips the pages scanning, if de-duplication
was not successful at previous attempts.

How often a page is skipped is dependent on how often de-deduplication has
been tried so far. The number of skips is calculated dynamically and is
currently limited to 8. This value has shown to be effective with different
workloads.

| Current skip value | New value |
|:------------------ |:--------- |
| 0                  | 1         |
| 1                  | 2         |
| 2                  | 4         |
| 4                  | 8         |
| 8                  | 8         |

The optimization helps with CPU and memory resource comsumption. Workloads
have shown a reduction in page scan of up to 35%. This has resulted in 20%
less CPU consumption. This is very application dependent and your findings
might be different.

The feature is especially helpful once "steady state" has been achieved.

## Debugging
The decisions of the KSM smart scan feature can be traced. The `ksm_advisor`
tracepoint reports:
- scan time
- pages scanned
- CPU%
