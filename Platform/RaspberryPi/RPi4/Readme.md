Raspberry Pi 4 Platform
=======================

# Summary

This is a port of 64-bit Tiano Core UEFI firmware for the Raspberry Pi 4 platforms.

This is intended to be useful 64-bit [TF-A](https://www.trustedfirmware.org/) +
UEFI implementation for the Raspberry Pi 4 which should be good enough for most
kind of UEFI development, as well as for running consummer Operating Systems
such as Linux or Windows.

Raspberry Pi is a trademark of the [Raspberry Pi Foundation](https://www.raspberrypi.org).

# Status

This firmware is still in early stage of development, meaning that it comes with
the following __major__ limitations:

- USB is not supported yet (will be added soon)
- Booting of vanilla Operating Systems (Windows, Linux) is not supported yet,
  let alone expected to work at all.

The only features that are expected to work with this first iteration of the
firmware are HDMI and serial I/O.

## Known issues

- The serial output from TF-A is garbled when using a `start4.elf` that was
  released after 2019.11.18. This is a TF-A issue that will be fixed in a
  later version.

# Building

Build instructions from the top level edk2-platforms Readme.md apply.

# Booting the firmware

1. Format a uSD card as FAT16 or FAT32
2. Copy the generated `RPI_EFI.fd` firmware onto the partition
3. Download and copy the following files from https://github.com/raspberrypi/firmware/tree/master/boot
  - `bcm2711-rpi-4-b.dtb`
  - `fixup4.dat`
  - `start4.elf`
4. Create a `config.txt` with the following content:
  ```
  arm_64bit=1
  enable_uart=1
  core_freq=250
  enable_gic=1
  armstub=RPI_EFI.fd
  ```
5. Insert the uSD card and power up the Pi.

# Notes

## ARM Trusted Firmware (TF-A)

The TF-A binaries were compiled from a TF-A source over which 2 serial-output related
patches were applied, the first one to fix the miniUART baud rate not being properly
set to 115200 bauds with recent versions of `start4.elf` and the second one to allow
swicthing between miniUART and PL011 at build time.

No other alterations to the official source have been applied.

For more details on the TF-A compilation, see the [Readme](./TrustedFirmware/Readme.md)
in the `TrustedFirmware/` directory.

## Device Tree

You can pass a custom Device Tree and overlays using the following:

```
(...)
disable_commandline_tags=2
device_tree_address=0x20000
device_tree_end=0x30000
device_tree=bcm2711-rpi-4-b.dtb
```

Note: the address range **must** be `[0x20000:0x30000]`.
`dtoverlay` and `dtparam` parameters are also supported **when** providing a Device Tree`.

## Custom `bootargs`

This firmware will honor the command line passed by the GPU via `cmdline.txt`.

Note, that the ultimate contents of `/chosen/bootargs` are a combination of several pieces:
- Original `/chosen/bootargs` if using the internal DTB. Seems to be completely discarded by GPU when booting with a custom device tree.
- GPU-passed hardware configuration. This one is always present.
- Additional boot options passed via `cmdline.txt`.

# Limitations

## NVRAM

The Raspberry Pi has no NVRAM.

NVRAM is emulated, with the non-volatile store backed by the UEFI image itself. This
means that any changes made in UEFI proper are persisted, but changes made from within
an Operating System aren't.

## RTC

The Rasberry Pi has no RTC.

An `RtcEpochSeconds` NVRAM variable is used to store the boot time.
This should allow you to set whatever date/time you want using the Shell date and
time commands. While in UEFI or HLOS, the time will tick forward.
`RtcEpochSeconds` is not updated on reboots.
