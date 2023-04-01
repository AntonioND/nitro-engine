#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

mkdir -p data

python3 $IMG2DS \
    --input $ASSETS/a5pal8.png \
    --name a5pal8 \
    --output data \
    --format A5PAL8

# TODO
#
# This has been taken from a libnds example:
#
#     https://github.com/devkitPro/nds-examples/tree/6afa09b2054c9f47685514c32873b3905721c9ee/Graphics/3D/Paletted_Cube/data
#
# When img2ds supports compressed textures it will be replaced by a PNG file.
cat assets/texture10_COMP_tex.bin assets/texture10_COMP_texExt.bin > data/texture.bin
cat assets/texture10_COMP_pal.bin > data/palette.bin
