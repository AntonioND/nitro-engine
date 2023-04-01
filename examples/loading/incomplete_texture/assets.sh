#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

mkdir -p data

python3 $IMG2DS \
    --input assets/a3pal32.png \
    --name a3pal32 \
    --output data \
    --format A3PAL32

python3 $IMG2DS \
    --input assets/pal4.png \
    --name pal4 \
    --output data \
    --format PAL4
