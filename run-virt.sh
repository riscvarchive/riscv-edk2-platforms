#!/bin/sh
set -o xtrace
ls -l U540.fd
timeout --foreground 10 \
  qemu-system-riscv64 -machine virt \
  -bios U540.fd \
  -m 1024 -nographic -smp cpus=1,maxcpus=1 | tee boot.log \
  || true
file U540.fd
grep -q 'Shell>' boot.log
