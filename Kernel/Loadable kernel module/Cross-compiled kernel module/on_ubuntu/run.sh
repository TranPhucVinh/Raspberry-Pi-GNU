#!/bin/bash

set -e

pushd util
tar -xvf artifacts.tar.gz
cd ../
git clone --progress https://github.com/raspberrypi/linux && cd linux && git checkout $(cat ../util/build_material/hash.txt) && rm -fr .git
popd

docker build -t $1 -f Dockerfile --rm .

rm -fr util/build_material linux

