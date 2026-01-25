+++
date = '2026-01-24T21:48:46-08:00'
draft = false
title = 'Installing FreeBSD on a USB SSD/NVME'
featuredImage = 'ssd.png'
tags = ["FreeBSD", "install", "USB", "enclosure", "SSD", "NVME"]
+++

This article describes how to install FreeBSD on an SSD or NVME when its connected
with an USB enclosure.
<!--more-->

## Installation
Follow the general installation procedure to install Freebsd on the system. When prompted
for the filesystem choose either UFS or ZFS (both works and is possible).

## Filesystem installation
Be careful to pick the correct device for the harddisk. USB device generally have the name
da<number>. If you install with a memory stick, then your USB enclosure will most likely be have the
number 1 (da1), the memory stick will be number 0 (da0).

With gpart show you can query the available disks:
```shell
gpart show

=>        34  2000409197  nda0  GPT  (954G)
          34        2014        - free -  (1007K)
        2048      532480     1  efi  (260M)
      534528       32768     2  ms-reserved  (16M)
      567296   155348992     3  ms-basic-data  (74G)
   155916288  1843200000     5  linux-data  (879G)
  1999116288     1280000     4  ms-recovery  (625M)
  2000396288       12943        - free -  (6M)

=>        40  3907029088  da0  GPT  (2T)
          40      532480    1  efi  (260M)
      532520        1024    2  freebsd-boot  (512K)
      533544         984       - free -  (492K)
      534528     4194304    3  freebsd-swap  (2G)
     4728832  3902300160    4  freebsd-zfs  (2T)
  3907028992         136       - free -  (68K)

```
Have a look at the first line of each device. The third column shows the name of the
device. The combination of the device name and the size is a good indicator to pick
the correct device.

## Labeling the partitions

The device numbers that are assigned dynamically to the USB devices. This means that on
the next boot your harddisk might get a different device number. If that happens you
will not be able to boot automatically. You can still get the device up by entering
a boot command at the boot loader prompt.

To avoid that problem, it is better to label the different partitions. This is a bit
different for UFS and ZFS. If you go with the default installation and use ZFS this
needs to be done only once (for the zfs root partition). If you use UFS, you need to
do this 3 times: for the EFI, UFS and Swap partition. However the approach is the 
same.

As the last step of the installation you get asked if you want to perform any additional
operations and it gives you the choice to open a shell. You need to answer with yes
to open a shell.

First check which partitions and filesystems are defined in /etc/fstab. Then query
the information for that disk with gpart list:

```shell
[shr@freebsd-dev ~/repo/sroeschus.github.io/content/posts/install-freebsd-on-usb-ssd]$ gpart list da0
Geom name: da0
modified: false
state: OK
fwheads: 255
fwsectors: 63
last: 3907029127
first: 40
entries: 128
scheme: GPT
Providers:
1. Name: da0p1
   Mediasize: 272629760 (260M)
   Sectorsize: 512
   Stripesize: 0
   Stripeoffset: 20480
   Mode: r1w1e2
   efimedia: HD(1,GPT,117c1209-f4dd-11f0-a0c4-7c5758c261a5,0x28,0x82000)
   rawuuid: 117c1209-f4dd-11f0-a0c4-7c5758c261a5
   rawtype: c12a7328-f81f-11d2-ba4b-00a0c93ec93b
   label: efiboot0
   length: 272629760
   offset: 20480
   type: efi
   index: 1
   end: 532519
   start: 40
2. Name: da0p2
   Mediasize: 524288 (512K)
   Sectorsize: 512
   Stripesize: 0
   Stripeoffset: 272650240
   Mode: r0w0e0
   efimedia: HD(2,GPT,1194cea3-f4dd-11f0-a0c4-7c5758c261a5,0x82028,0x400)
   rawuuid: 1194cea3-f4dd-11f0-a0c4-7c5758c261a5
   rawtype: 83bd6b9d-7f41-11dc-be0b-001560b84f0f
   label: gptboot0
   length: 524288
   offset: 272650240
   type: freebsd-boot
   index: 2
   end: 533543
   start: 532520
3. Name: da0p3
   Mediasize: 2147483648 (2.0G)
   Sectorsize: 512
   Stripesize: 0
   Stripeoffset: 273678336
   Mode: r1w1e1
   efimedia: HD(3,GPT,11bd2fb7-f4dd-11f0-a0c4-7c5758c261a5,0x82800,0x400000)
   rawuuid: 11bd2fb7-f4dd-11f0-a0c4-7c5758c261a5
   rawtype: 516e7cb5-6ecf-11d6-8ff8-00022d09712b
   label: swap0
   length: 2147483648
   offset: 273678336
   type: freebsd-swap
   index: 3
   end: 4728831
   start: 534528
4. Name: da0p4
   Mediasize: 1997977681920 (1.8T)
   Sectorsize: 512
   Stripesize: 0
   Stripeoffset: 2421161984
   Mode: r1w1e1
   efimedia: HD(4,GPT,11dec9db-f4dd-11f0-a0c4-7c5758c261a5,0x482800,0xe8986000)
   rawuuid: 11dec9db-f4dd-11f0-a0c4-7c5758c261a5
   rawtype: 516e7cba-6ecf-11d6-8ff8-00022d09712b
   label: zfs0
   length: 1997977681920
   offset: 2421161984
   type: freebsd-zfs
   index: 4
   end: 3907028991
   start: 4728832
Consumers:
1. Name: da0
   Mediasize: 2000398934016 (1.8T)
   Sectorsize: 512
   Mode: r3w3e7
```

The interesting columns are the label and the index columns. The above example
is from a ZFS installation. In that case you don't need to do anything. For UFS
the labels will be empty and you need to create labels like this

```shell
gpart modify -i 1 -l usbefi da1
gpart modify -i 2 -l usbufs da1
gpart modify -i 3 -l usbswp da1
```
The index is specified after the -i option. The label name is specified after the -l option.
You can pick any name, but it makes sense that the name matches the type. The type tells
you what type of filesystem it is. The last parameter is the device name. Make sure to pick
the correct device.

The last step is to replace the device name in /etc/fstab with the label name. Instead of
/dev/da1p1, it should be replaced with /dev/gpt/label-name.

Now its safe to reboot and the system should boot successfully.
