---
title: "Projects"
date: 2022-12-24T22:29:52-08:00
draft: true
---

For the reader who wants to get an overview over the patches that I have submitted
[upstream](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/log/?qt=grep&q=stefan+roesch) (be patient, loading git.kernel.org takes a while).

# List of patch series
The following is a list of the more important patch series that I developed and that have been upstreamed.
The list is ordered in chronological order. In parentheses it contains the date when the most current version
of the patch series was submitted and which subsystems were part of the patch series. The subsystem names are
abbreviated with the commmon short names from the kernel directory names:
- block = block layer
- mm = memory management
- nvme

## Backing device information changes (Nov 2022, mm)

This patch provides new knobs to influence the behavior of the size and management of the
dirty pagecache. In addition it also changes the internal calculation to use part of 1000000
instead of percentage values. 

[Cover letter](https://lore.kernel.org/all/20221119005215.3052436-1-shr@devkernel.io/T/#u)

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=8e9d5ead865a1a7af74a444d2f00f1ef4539bfba), [P2](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=27bbe9d48d4e298864e18b39f091342c68b81637), [P3](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=16b837eb84e6948f92411eb32e97a05f89733ddc), [P4](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=ae82291e9ca47c3d6da6b77a00f427754aca413e), [P5](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=00df7d51263b46ed93f7572e2d09579746f7b1eb), [P6](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=efc3e6ad53ea14225b434fddca261c9a1c56c707), [P7](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=1bf27e98d26d1e62166a456ef17460be085cbe0b), [P8](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=c56e049a5e401a177c7c9b39a3bcc973ff5cec0b), [P9](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=c354d9268d7825eb8643f658c5091079d4f11a4a), [P10](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=712c00d66a342a3ed375df41c3df7d3d2abad2c0), [P11](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=8021fb3232f265b81c7e4e7aba15bc3a04ff1fd3), [P12](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=803c98050569850be5fd51a2025c67622de887d9), [P13](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=9c84819bd64ec15cb15d041c45ebe4725e9d4f3b), [P14](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=9c832a8d571784c998d0f9f5df480c62f7f3064c), [P15](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=4e230b406eda9bdf7f8a71e2cc3df18a824abcb0), [P16](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=bca52dcbadc583f4db6435599c44a79f97293f06), [P17](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=54790f30fea74247e2f38b4a632ee3dc2fe42d86), [P18](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=2c44af4f2aaa260199f218f11920c406e688693c), [P19](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=ad3e6dabf6f7d9ffd68eb711191ef16cdbdd25f0), [P20](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=eba39236f18da7a50b6c51df5d902ee72c43e760)

## Enable batched completions of passthrough I/O (Sep 2022, block, nvme)

The filesystem IO path can take advantage of allocating batches of
requests, if the underlying submitter tells the block layer about it
through the blk_plug. For passthrough IO, the exported API is the
blk_mq_alloc_request() helper, and that one does not allow for
request caching.

I developed and implemented the prototype of the feature and worked with
Jens Axboe on upstreaming this patch series.

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=ab3e1d3bbab9e973aeb4dd4603251578658a47ff), [P2](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=c0a7ba77e81b8440d10f38559a5e1d219ff7e87c), [P3](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=851eb780decb7180bcf09fad0035cba9aae669df)

## Support async buffered writes for io-uring on btrfs (Sep 2022, mm, btrfs)

This adds the async buffered write support for btrfs to io-uring. So far the async buffered writes couldn't be
processed in the fast path; instead they were processed by the IO-workers. With this change the performance of
async buffered writes is better than the performance of aio.

This patch series is based on the earlier patch series, that provided async buffered writes for XFS.

[P1](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=611df5d6616d80a22906c352ccd80c395982fbd9), [P2](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=857bc13f857aea957ae038b2b43c28560976024a), [P3](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=26ce91144631a402ff82c93358d8880326a7caa3), [P4](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=1daedb1d6bf24c7185e00cd341404f262f8de7c8), [P5](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=d2c7a19f5c82ace6ea0ec00ae53c6dd97ee8e274), [P6](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=80f9d24130e45b01984a918d6b2006c10687b138), [P7](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=fc2260001232766c1836d5a6053913194ce23f88), [P8](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=2fcab928ccc2bac0c22e3b3b04f5acf0dc8de96b), [P9](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=304e45acdb8f68a974e8a64a6296803530bb851f), [P10](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=965f47aeb5deddc59a1ace28e99b2a578df57305), [P11](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=c922b016f353eafb69997d8c0f06efdf945315ce), [P12](https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/commit/?id=926078b21db91b72b444277fdc2166914cf113fc)
