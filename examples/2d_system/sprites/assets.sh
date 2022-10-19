#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

python3 $IMG2DS \
    --input assets/icon.png \
    --name icon \
    --output data \
    --format PAL16
