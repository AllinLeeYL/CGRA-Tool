#!/bin/bash
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
ROOT=${SCRIPT_DIR}/..

cd ${ROOT}
cmake . --preset release
cd ${ROOT}/build/release
ninja
ninja install
echo "Please execute the commands below to add cgra-tool to your PATH"
echo "  export PATH=\$PATH:${ROOT}/install/release/bin"