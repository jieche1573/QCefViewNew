#!/bin/bash

BUILD_PROJECT=0

ARCH=arm64
# 或 ARCH=aarch64

BUILD_DIR="$(pwd)/.build/linux.${ARCH}"

while getopts bi flag
do
    case "${flag}" in
        b) BUILD_PROJECT=1;;
    esac
done

echo ============== Config project ==============
cmake -G "Unix Makefiles" \
    -S . \
    -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DPROJECT_ARCH=${ARCH} \
    -DBUILD_DEMO=ON \
    -DUSE_SANDBOX=ON \
    -DCMAKE_INSTALL_PREFIX:PATH="$(pwd)/out/linux.${ARCH}" \
    $*

if [ ${BUILD_PROJECT} -eq 1 ] 
then
    echo ============== Build project ==============
    cmake --build "${BUILD_DIR}"
fi
