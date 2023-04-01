#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

mkdir -p data

python3 $IMG2DS \
    --input assets/button.png \
    --name button \
    --output data \
    --format PAL256

python3 $IMG2DS \
    --input assets/empty.png \
    --name empty \
    --output data \
    --format PAL256

python3 $IMG2DS \
    --input assets/true.png \
    --name true \
    --output data \
    --format PAL256
