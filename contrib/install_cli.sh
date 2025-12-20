 #!/usr/bin/env bash

 # Execute this file to install the hemp0x cli tools into your path on OS X

 CURRENT_LOC="$( cd "$(dirname "$0")" ; pwd -P )"
 LOCATION=${CURRENT_LOC%Hemp0x-Qt.app*}

 # Ensure that the directory to symlink to exists
 sudo mkdir -p /usr/local/bin

 # Create symlinks to the cli tools
 sudo ln -s ${LOCATION}/Hemp0x-Qt.app/Contents/MacOS/hemp0xd /usr/local/bin/hemp0xd
 sudo ln -s ${LOCATION}/Hemp0x-Qt.app/Contents/MacOS/hemp0x-cli /usr/local/bin/hemp0x-cli
