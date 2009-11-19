#!/bin/sh
DOXYGEN=/usr/bin/doxygen

[ -x $DOXYGEN ] || exit 1

$DOXYGEN ./bc_emu.doxygen
