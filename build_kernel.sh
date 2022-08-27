#!/bin/bash

mkdir out

BUILD_CROSS_COMPILE=/media/huaji/7293721d-b921-481c-a0d5-7b6419c14a01/LineageOS17.1/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-

# sdclang-8.0.6
#CLANG_PATH=/media/huaji/7293721d-b921-481c-a0d5-7b6419c14a01/LineageOS17.1/prebuilts/clang/host/linux-x86/sdclang-8.0.6/bin

# sdclang-12.1.5
#CLANG_PATH=/home/huaji/upload/proprietary_vendor_qcom_sdclang-12/compiler/bin

# sdclang-14
#CLANG_PATH=/home/huaji/upload/proprietary_vendor_qcom_sdclang-14/compiler/bin

#aosp-clang
CLANG_PATH=/media/huaji/7293721d-b921-481c-a0d5-7b6419c14a01/LineageOS18.1/prebuilts/clang/host/linux-x86/clang-r383902b1/bin

CROSS_COMPILE_ARM32=/media/huaji/7293721d-b921-481c-a0d5-7b6419c14a01/LineageOS18.1/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin/arm-linux-androideabi-

CLANG_TRIPLE=aarch64-linux-gnu-

export ARCH=arm64
export PATH=${CLANG_PATH}:${PATH}

make -j8 -C $(pwd) O=$(pwd)/out CROSS_COMPILE=$BUILD_CROSS_COMPILE CLANG_TRIPLE=$CLANG_TRIPLE CROSS_COMPILE_ARM32=$CROSS_COMPILE_ARM32 \
    CC=clang \
    vendor/op4ad9-perf_defconfig

A=$(date +%s)
make -j8 -C $(pwd) O=$(pwd)/out CROSS_COMPILE=$BUILD_CROSS_COMPILE CLANG_TRIPLE=$CLANG_TRIPLE CROSS_COMPILE_ARM32=$CROSS_COMPILE_ARM32 \
    CC=clang \
    -Werror \
    2>&1 | tee build.txt
B=$(date +%s)
C=$(expr $B - $A)
#echo $(expr $C / 60)
echo "kernel finish" $(expr $C / 60)"min"$(expr $C % 60)"s"
#cp out/arch/arm64/boot/Image $(pwd)/Image
