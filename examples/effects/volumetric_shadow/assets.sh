#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py

rm -rf data
mkdir -p data

python3 $OBJ2DL \
    --input $ASSETS/teapot.obj \
    --output data/teapot.bin \
    --texture 256 256 \
    --scale 0.1
