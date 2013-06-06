#!/bin/sh

export LD_PRELOAD=$(pwd)/\$LIB/block_net.so
$*
