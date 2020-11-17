# bitty_xmas_bauble_device_code - microbit C++ code

## Description

This is the code from which the hex files in the binary/ folder were built. Unfortunately back when those hex files were created, more memory was available for application code. These days (2019), following changes made to the micro:bit DAL by its maintainers, there is less memory available and this code does not work (it generates crash code 0x02 meaning Out Of Memory). It may be possible to optimise this code and reduce its memory requirement, so it has been published in the hope people might find it useful. PRs welcome.

## Building this code

To build this code, you need an offline microbit development environment, based on Yotta. See https://lancaster-university.github.io/microbit-docs/offline-toolchains/ for details.

