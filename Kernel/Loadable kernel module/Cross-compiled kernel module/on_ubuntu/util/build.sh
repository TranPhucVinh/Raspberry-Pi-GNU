#!/bin/bash

set -e

cd /src
target=$1
module_name=$(echo ${target} | cut -d '.' -f 1)

#perform rename
sed "1 s/module/${module_name}/" /raspberry/util/Makefile > Makefile

make all