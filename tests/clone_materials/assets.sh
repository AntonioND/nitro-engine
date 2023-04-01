#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

python3 $IMG2DS \
    --input $NITRO_ENGINE/examples/assets/a3pal32.png \
    --name a3pal32 \
    --output data \
    --format A3PAL32
