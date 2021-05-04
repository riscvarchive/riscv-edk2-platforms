#!/bin/bash
ls -l
ls -l edk2
ls -l edk2-platforms
export GCC5_RISCV64_PREFIX=/riscv-gnu-toolchain-binaries/bin/riscv64-unknown-elf-
export PACKAGES_PATH=/edk2:/edk2-platforms
export EDK_TOOLS_PATH=/edk2/BaseTools

source /edk2/edksetup.sh
make -C edk2/BaseTools clean
make -C edk2/BaseTools
make -C edk2/BaseTools/Source/C

source edk2/edksetup.sh BaseTools

cd /edk2-platforms
build -a RISCV64 -p Platform/SiFive/U5SeriesPkg/FreedomU540HiFiveUnleashedBoard/U540.dsc -t GCC5
cp /Build/FreedomU540HiFiveUnleashed/DEBUG_GCC5/FV/U540.fd U540.fd
echo Finished EDK2 build

#build -a RISCV64 -p MdeModulePkg/MdeModulePkg.dsc -t GCC5
#build -a RISCV64 -p CryptoPkg/CryptoPkg.dsc -t GCC5
#build -a RISCV64 -p MdePkg/MdePkg.dsc -t GCC5
#build -a RISCV64 -p OvmfPkg/OvmfPkgRISCV64.dsc -t GCC5
#build -a RISCV64 -p RiscVPkg/RiscVPkg.dsc -t GCC5
