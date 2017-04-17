#!/bin/bash -ex

RELEASE_TYPE=Debug
CORES=1

while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
    --release)
      RELEASE_TYPE=Release
      ;;
    --cores=*)
      CORES=${key#*=}
      ;;
    *)
      echo "Unknown option"
      exit 1
      ;;
   esac

   shift # past argument or value
done

mkdir -p build
pushd build

cmake -DCMAKE_BUILD_TYPE=$RELEASE_TYPE ..
make -j$CORES

popd
