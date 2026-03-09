#!/usr/bin/env bash
set -euo pipefail

# 一键 CMake 构建脚本
# 用法：
#   ./build.sh                # Debug 构建
#   ./build.sh release        # Release 构建
#   ./build.sh debug clean    # 清理后 Debug 构建
#   ./build.sh rebuild        # 等价于 debug clean

BUILD_DIR="build-cmake"
BUILD_TYPE="Debug"
DO_CLEAN="false"

for arg in "$@"; do
  case "${arg}" in
    debug|Debug)
      BUILD_TYPE="Debug"
      ;;
    release|Release)
      BUILD_TYPE="Release"
      ;;
    clean|--clean)
      DO_CLEAN="true"
      ;;
    rebuild)
      BUILD_TYPE="Debug"
      DO_CLEAN="true"
      ;;
    *)
      echo "Unknown option: ${arg}"
      echo "Usage: ./build.sh [debug|release] [clean|rebuild]"
      exit 1
      ;;
  esac
done

if [[ "${DO_CLEAN}" == "true" ]]; then
  echo "[build.sh] 清理目录: ${BUILD_DIR}"
  rm -rf "${BUILD_DIR}"
fi

echo "[build.sh] 配置 CMake (${BUILD_TYPE})"
cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

echo "[build.sh] 开始编译"
cmake --build "${BUILD_DIR}" -j

echo "[build.sh] 构建完成。可执行文件位于 bin/"
