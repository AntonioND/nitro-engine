#!/bin/sh

NITRO_ENGINE=../../..
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py

mkdir -p data

python3 $OBJ2DL \
    --input $ASSETS/cube.obj \
    --output data/cube.bin \
    --texture 32 32
