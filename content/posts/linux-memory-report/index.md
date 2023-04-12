---
title: "Query installed memory hardware information in Linux"
description: How to query which memory chips are installed on Linux 
date: 2023-04-11T22:33:15-07:00
featuredImage: "memory.jpg"
draft: false
toc:
  enable: false
tags: ["linux", "memory", "chips", "hardware"]
---

This article describes how to query what type of memory hardware chips are
installed on the Linux machine.
<!--more-->

To be able to add memory to an existing Linux machine it is useful to query
the exisiting memory configuration (like what memory chips are installed). This
is especially important if one of the memory banks is empty and the plan is
to add the new memory to the empty bank. It is advisable to use the same type
of memory that is already installed in the system. If memory chips with different
type of speed are used, problems are not unexpected.

The current configuration can be determined with the following command:
```shell
sudo dmidecode -t memory

# dmidecode 3.5
Getting SMBIOS data from sysfs.
SMBIOS 3.5.0 present.

Handle 0x0032, DMI type 16, 23 bytes
Physical Memory Array
Location: System Board Or Motherboard
Use: System Memory
Error Correction Type: None
Maximum Capacity: 128 GB
Error Information Handle: Not Provided
Number Of Devices: 4

Handle 0x0033, DMI type 17, 92 bytes
Memory Device
Array Handle: 0x0032
Error Information Handle: Not Provided
Total Width: Unknown
Data Width: Unknown
Size: No Module Installed
Form Factor: Other
Set: None
Locator: DIMM2
Bank Locator: BANK 0
Type: Unknown
Type Detail: None

Handle 0x0035, DMI type 17, 92 bytes
Memory Device
Array Handle: 0x0032
Error Information Handle: Not Provided
Total Width: 64 bits
Data Width: 64 bits
Size: 16 GB
Form Factor: DIMM
Set: None
Locator: DIMM1
Bank Locator: BANK 0
Type: DDR4
Type Detail: Synchronous
Speed: 3200 MT/s
Manufacturer: Kingston
Serial Number: F077370D
Asset Tag:
Part Number: HP32D4U2S8MF-16
Rank: 1
Configured Memory Speed: 3200 MT/s
Minimum Voltage: 1.2 V
Maximum Voltage: 1.2 V
Configured Voltage: 1.2 V
Memory Technology: DRAM
Memory Operating Mode Capability: Volatile memory
Firmware Version: Not Specified
Module Manufacturer ID: Bank 2, Hex 0x98
Module Product ID: Unknown
Memory Subsystem Controller Manufacturer ID: Unknown
Memory Subsystem Controller Product ID: Unknown
Non-Volatile Size: None
Volatile Size: 16 GB
Cache Size: None
Logical Size: None
```
The important information is contained in the rows: Type, Speed, Size, Manufacturer,
Serial Number and Part Number.
