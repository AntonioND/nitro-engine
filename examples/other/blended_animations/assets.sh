#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py
MD5_TO_DSMA=$TOOLS/md5_to_dsma/md5_to_dsma.py

rm -rf data
mkdir -p data

python3 $MD5_TO_DSMA \
    --model $ASSETS/robot/Robot.md5mesh \
    --name robot \
    --output data \
    --texture 256 256 \
    --anim $ASSETS/robot/Walk.md5anim $ASSETS/robot/Wave.md5anim \
    --skip-frames 1 \
    --bin \
    --blender-fix
