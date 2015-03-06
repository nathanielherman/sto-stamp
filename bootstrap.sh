#!/bin/sh

if [ ! -f stamp-0.9.10/lib/sto/configure.ac ]; then
    git submodule init
    git submodule update
fi

if [ -x stamp-0.9.10/lib/sto/bootstrap.sh ]; then
    ( cd stamp-0.9.10/lib/sto; ./bootstrap.sh )
fi
