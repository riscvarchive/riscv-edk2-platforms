#!/bin/sh
set -o xtrace
ls -l U540.fd
qemu-system-riscv64 -machine virt \
-bios U540.fd \
-m 1024 -nographic -smp cpus=1,maxcpus=1
