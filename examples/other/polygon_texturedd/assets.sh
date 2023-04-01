#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

mkdir -p data

python3 $IMG2DS \
    --input assets/texture.png \
    --name texture \
    --output data \
    --format A1RGB5
