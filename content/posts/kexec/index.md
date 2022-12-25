---
title: "Kexec: Booting into another kernel"
featuredImage: "kexec.jpg"
date: 2022-11-27T22:32:21-08:00
draft: true
toc:
  enable: true
tags: ["kexec", "tools"]
---

As a kernel developer you often have to test a new kernel. The kexec command allows you to boot into
another kernel.  The kexec command utilizes the kexec system call and enables you to load and boot into another kernel.
Kexec performans the function of the boot loader. The major difference is that a kexec boot does not
perform the hardware initiaiization that is normally performed by the BIOS or firmware.

## Installation

The first step is to install the kexec tools.

<pre class="command-line language-bash" data-user="root" data-host="localhost">
<code>
> sudo dnf install kexec-tools
</code>
</pre>

## Installing the new kernel
To be able to install a new kernel, it first needs to be installed. Once the kernel is installed, it
should be listed in the /boot. The new kernel can then be installed with the following command line:

<pre class="command-line language-bash" data-user="root" data-host="localhost">
<code>
> sudo kexec -l path-to-kernel-vmlinuz --initrd=path-to-init-rd --command-line="$(cat /proc/cmdline)"
> sudo kexec -e
</code>
</pre>

## Example
An example would be for instance:

<pre class="command-line language-bash" data-user="root" data-host="localhost">
<code>
> sudo kexec -l /boot/vmlinuz-5.12.0-0_fbk8 --initrd=/boot/initramfs-5.12.0-0_fbk8_hardened_6656_gc85768aa64da.img --command-line="$(cat /proc/cmdline)"
> sudo kexec -e
</code>
</pre>

After executing the kexec -e commmand, the user process gets terminated and the new kernel is booted. Once testing
has completed and the new kernel is no longer needed, the same procedure can be used to boot into the original kernel. This
makes sure that the same kernel is active as before the test.

For more detailed information about the kexec command you can check the [kexec man page](https://man7.org/linux/man-pages/man8/kexec.8.html).
