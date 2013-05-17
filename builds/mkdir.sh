#!/bin/bash

dir="$(git describe --tag --always)"
mkdir -p "$dir/Debug" "$dir/Release"
