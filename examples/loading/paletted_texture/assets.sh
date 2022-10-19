#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

python3 $IMG2DS \
    --input $ASSETS/a3pal32.png \
    --name a3pal32 \
    --output data \
    --format A3PAL32

python3 $IMG2DS \
    --input $ASSETS/a5pal8.png \
    --name a5pal8 \
    --output data \
    --format A5PAL8
