#!/bin/bash -e
(cd ../../lib/bin && ./compileLinuxSWC.sh)
./buildLinux.sh
./runLinux.sh
