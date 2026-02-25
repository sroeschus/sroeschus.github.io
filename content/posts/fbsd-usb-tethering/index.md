+++
date = '2026-02-24T20:45:55-08:00'
draft = false
title = 'How to use USB tethering on FreeBSD'
featuredImage = 'cell.png'
tags = ["FreeBSD", "USB", "tethering", "wifi", "wifibox"]
+++

This article describes how to use USB tethering on FreeBSD.
<!--more-->

## Motivation
Sometimes you are in places where there is no WIFI access or WIFI connection is not
good. Using USB tethering on your cell phone is a good way to stil get a good internet
connection.

Another reason might be that you want to install FreeBSD on a system which does not
have USB drivers yet and you don't have an ethernet connection or a USB WIFI card.

## Connecting your cell phone
You generally need a USB cable to connect your computer to your cell phone. Once the
computer is started, you can open the settings page in your mobile phone and enable
"USB Tethering". Be aware that the option generally only becomes available once the
computer and the cellphone are connected and both devices are turned on.

## Checking the network devices
The current status of your connected network devices can be checked with the ifconfig
command.

```shell
# ifconfig -a
...
ue0: flags=1008843<UP,BROADCAST,RUNNING,SIMPLEX,MULTICAST,LOWER_UP> metric 0 mtu 1500
        options=80000<LINKSTATE>
        ether xx:xx:xx:xx:xx:xx
        inet 192.168.xxx.xxx netmask 0xffffff00 broadcast 192.168.248.255
        inet6 fe80::6c8a:ecff:fec9:39c4%ue0 prefixlen 64 scopeid 0x3
        media: Ethernet autoselect
        status: active
        nd6 options=23<PERFORMNUD,ACCEPT_RTADV,AUTO_LINKLOCAL>
...
```
The device name for the USB tethering connection is generally called ue0. You should
check the status line and the flags line of the above command. If the flags value
contains "UP", then the device is online and operational. In addition it should also
have an internet address associated with it. In the above output some of the values
are x'ed out for privacy reasons.

## Starting the device
In case the device was connected during startup, the device is already operational.
Otherwise it might be necessary to start the device. With the following command the
device can be started:

```shell
dhclient ue0
```
