#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py
IMG2DS=$TOOLS/img2ds/img2ds.py

mkdir -p nitrofiles

python3 $OBJ2DL \
    --input $ASSETS/robot/robot.obj \
    --output nitrofiles/robot.bin \
    --texture 256 256

python3 $IMG2DS \
    --input $ASSETS/teapot.png \
    --name texture \
    --output nitrofiles \
    --format A1RGB5
