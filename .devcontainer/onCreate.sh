#!/bin/bash

sudo apt-get -yq update && sudo apt-get -yq update

cd ~
curl 'https://invent.kde.org/sdk/kde-builder/-/raw/master/scripts/initial_setup.sh' > initial_setup.sh
bash initial_setup.sh
