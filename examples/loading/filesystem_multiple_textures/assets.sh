#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py
OBJ2DL=$TOOLS/obj2dl/obj2dl.py

python3 $OBJ2DL \
    --input $ASSETS/cube.obj \
    --output nitrofiles/cube.bin \
    --texture 64 64

python3 $IMG2DS \
    --input $ASSETS/spiral_blue_pal32.png \
    --name spiral_blue_pal32 \
    --output nitrofiles \
    --format A3PAL32

python3 $IMG2DS \
    --input $ASSETS/spiral_red_pal32.png \
    --name spiral_red_pal32 \
    --output nitrofiles \
    --format A3PAL32
