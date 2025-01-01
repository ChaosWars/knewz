#!/bin/bash

export DEBIAN_FRONTEND=noninteractive

cd ~
curl 'https://invent.kde.org/sdk/kde-builder/-/raw/master/scripts/initial_setup.sh' > initial_setup.sh
bash initial_setup.sh
rm initial_setup.sh

kde-builder --generate-config

if [[ ! -f "${LOCAL_WORKSPACE_DIR}/CMakePresets.json" ]]; then
    cp "$LOCAL_WORKSPACE_DIR/.devcontainer/CMakePresets.template.json" $LOCAL_WORKSPACE_DIR/CMakePresets.json
fi
