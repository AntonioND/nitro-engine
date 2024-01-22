#!/bin/sh

rm -rf data
mkdir -p data

# ptexconv can be obtained here: https://github.com/Garhoogin/ptexconv/

/opt/blocksds/external/ptexconv/ptexconv \
    -gt -ob -k FF00FF -v -f tex4x4 \
    -o data/grill \
    assets/grill.png

/opt/blocksds/external/ptexconv/ptexconv \
    -gt -ob -k FF00FF -v -f tex4x4 \
    -o data/landscape \
    assets/landscape.jpg
