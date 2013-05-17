#!/bin/bash

set -xe

dir="$(git describe --tags --always)"
mkdir -p "$dir"

pushd "$dir"
for type in Release Debug; do
	mkdir -p $type

	pushd "../../src"
	xcodebuild ONLY_ACTIVE_ARCH=No -configuration $type -scheme FRESteamWorks build
	popd
	cp -r "../../lib/bin/FRESteamWorks.framework" "$type/"
done
