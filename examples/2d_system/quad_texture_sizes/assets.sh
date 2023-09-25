#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

mkdir -p data

python3 $IMG2DS \
    --input assets/s8.png \
    --name s8 \
    --output data \
    --format PAL16

python3 $IMG2DS \
    --input assets/s16.png \
    --name s16 \
    --output data \
    --format PAL16

python3 $IMG2DS \
    --input assets/s64.png \
    --name s64 \
    --output data \
    --format PAL16

python3 $IMG2DS \
    --input assets/s256.png \
    --name s256 \
    --output data \
    --format PAL16
