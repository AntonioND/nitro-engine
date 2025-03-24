#!/bin/sh

BLOCKSDS="${BLOCKSDS:-/opt/blocksds/core/}"
GRIT=$BLOCKSDS/tools/grit/grit

rm -rf nitrofiles
mkdir -p nitrofiles

$GRIT assets/a3pal32.png -W3 \
    -ftr -fh! -p \
    -gx -gb -gBa3i5 -gT \
    -o nitrofiles/a3pal32_png

$GRIT assets/a1rgb5.png -W3 \
    -ftr -fh! -p \
    -gx -gb -gB16 -gT! \
    -o nitrofiles/a1rgb5_png
