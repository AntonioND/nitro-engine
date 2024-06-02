#!/bin/sh

NITRO_ENGINE=../../..
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
MD5_TO_DSMA=$TOOLS/md5_to_dsma/md5_to_dsma.py
GRIT=/opt/blocksds/core/tools/grit/grit

rm -rf nitrofiles
mkdir -p nitrofiles

python3 $MD5_TO_DSMA \
    --model $ASSETS/robot/Robot.md5mesh \
    --name robot \
    --output nitrofiles \
    --texture 256 256 \
    --anim $ASSETS/robot/Wave.md5anim \
    --skip-frames 1 \
    --blender-fix

$GRIT \
    graphics/texture.png \
    -ftb -fh! -W1 \
    -onitrofiles/texture
