#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py
GRIT=grit

rm -rf nitrofiles
mkdir -p nitrofiles

python3 $OBJ2DL \
    --input $ASSETS/robot/robot.obj \
    --output nitrofiles/robot.bin \
    --texture 256 256

$GRIT \
    graphics/texture.png \
    -ftb -fh! -W1 \
    -onitrofiles/texture
