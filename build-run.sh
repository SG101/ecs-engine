#!/bin/bash

./build.sh

if [ $? -eq 0 ]; then
	bin/debug/engine-deb
fi
