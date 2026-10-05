#!/bin/bash
set -Eeuo pipefail
export LD_LIBRARY_PATH="/opt/gcc-14/lib64${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

./out/build/default/soa/Release/render-soa ./archivos_config/config1.txt ./archivos_config/scene1.txt ./archivos_config/out1.ppm

python3 ./scripts/comparar.py ./archivos_config/out1.ppm ./archivos_config/s1.ppm