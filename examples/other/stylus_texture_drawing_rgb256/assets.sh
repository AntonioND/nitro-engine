#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

mkdir -p data

python3 $IMG2DS \
    --input $ASSETS/teapot.png \
    --name pal256 \
    --output data \
    --format PAL256
