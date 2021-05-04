#!/bin/sh
set -o xtrace

timeout --foreground 15 \
  qemu-system-riscv64 -machine virt \
  -bios Build/RiscvVirt/DEBUG_GCC5/FV/RISCVVIRT.fd \
  -drive file=rootfs.ext2,format=raw,id=hd0 \
  -device virtio-blk-device,drive=hd0 \
  -drive file=esp.iso,format=raw,id=hd1 \
  -device virtio-blk-device,drive=hd1 \
  -netdev user,id=net0 \
  -device virtio-net-device,netdev=net0 \
  -m 1024 -nographic -smp cpus=1,maxcpus=1 | tee boot.log \
  || true

grep -q 'Shell>' boot.log
grep -q 'Welcome to Buildroot' boot.log
# TODO: Figure out how to log in
