#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py
IMG2DS=$TOOLS/img2ds/img2ds.py

rm -rf data
mkdir -p data

python3 $OBJ2DL \
    --input $ASSETS/cube.obj \
    --output data/cube.bin \
    --texture 32 32 \

python3 $IMG2DS \
    --input assets/depth.png \
    --name depth \
    --output data \
    --format DEPTHBMP
