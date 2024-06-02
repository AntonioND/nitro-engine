#!/bin/sh

NITRO_ENGINE=../../..
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py
GRIT=/opt/blocksds/core/tools/grit/grit

rm -rf nitrofiles
mkdir -p nitrofiles

python3 $OBJ2DL \
    --input $ASSETS/cube.obj \
    --output nitrofiles/cube.bin \
    --texture 64 64

$GRIT \
    graphics/spiral_blue_pal32.png \
    -ftb -fh! -W1 \
    -onitrofiles/spiral_blue_pal32

$GRIT \
    graphics/spiral_red_pal32.png \
    -ftb -fh! -W1 \
    -onitrofiles/spiral_red_pal32
