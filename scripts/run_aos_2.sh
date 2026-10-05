#!/bin/bash
set -Eeuo pipefail
export LD_LIBRARY_PATH="/opt/gcc-14/lib64${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

./out/build/default/aos/Release/render-aos ./archivos_config/config2.txt ./archivos_config/scene2.txt ./archivos_config/out2.ppm

python3 ./scripts/comparar.py ./archivos_config/out2.ppm ./archivos_config/s2.ppm
