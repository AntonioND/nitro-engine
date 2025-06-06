#!/bin/sh

rm -rf data
mkdir -p data

BLOCKSDSEXT="${BLOCKSDSEXT:-/opt/blocksds/external/}"

# ptexconv can be obtained here: https://github.com/Garhoogin/ptexconv/

$BLOCKSDSEXT/ptexconv/ptexconv \
    -gt -ob -k FF00FF -v -f tex4x4 \
    -o data/grill \
    assets/grill.png

$BLOCKSDSEXT/ptexconv/ptexconv \
    -gt -ob -k FF00FF -v -f tex4x4 \
    -o data/landscape \
    assets/landscape.jpg
