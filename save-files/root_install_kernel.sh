#!/bin/sh

export MAKEOBJDIRPREFIX=/home/shr/freebsd-dest/obj
make installkernel KERNCONF=MYKERNEL

