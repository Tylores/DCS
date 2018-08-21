# DCS (Distributed Control System)

## Dependancies
### Linux Environment
``` console
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git build-essential gcc g++ cmake make xsltproc scons doxygen graphviz libgtk2.0-dev libssl-dev libxml2-dev libcap-dev
mkdir ~/dev ~/src
```

### AllJoyn
``` console
cd ~/src
git clone https://github.com/alljoyn/core-alljoyn

cd core-alljoyn
scons BINDINGS=cpp WS=off DOCS=html
```

#### Test AllJoyn Installation
``` console
export CPU=x86_64
export OS=linux
export VARIANT=debug
export AJ_ROOT=~/src/core-alljoyn
export AJLIB=$AJ_ROOT/build/linux/$CPU/$VARIANT/dist/cpp/lib
export LD_LIBRARY_PATH=$AJLIB:$LD_LIBRARY_PATH

cd $AJLIB
cd ../bin/samples
./AboutService
```

## Install

## Setup

## Use
