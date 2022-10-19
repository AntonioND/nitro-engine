#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

python3 $IMG2DS \
    --input assets/text.png \
    --name text \
    --output data \
    --format A1RGB5

python3 $IMG2DS \
    --input assets/text2.png \
    --name text2 \
    --output data \
    --format A1RGB5
