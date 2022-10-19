#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
TOOLS=$NITRO_ENGINE/tools
IMG2DS=$TOOLS/img2ds/img2ds.py

python3 $IMG2DS \
    --input assets/big.png \
    --name big \
    --output data \
    --format A1RGB5

python3 $IMG2DS \
    --input assets/medium.png \
    --name medium \
    --output data \
    --format A1RGB5

python3 $IMG2DS \
    --input assets/small.png \
    --name small \
    --output data \
    --format A1RGB5
