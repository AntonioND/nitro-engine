#!/bin/sh

for i in `ls`; do
    if test -e $i/assets.sh ; then
        cd $i;
            sh assets.sh || { exit 1;};
        cd ..;
    fi;
done;
