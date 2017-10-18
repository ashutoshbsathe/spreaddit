#!/bin/bash
echo "Decompressing $1"
unzip $1 -d utils/uncompressed > /dev/null
if [ -d "utils/uncompressed" ]; then
	echo "Successful !";
fi
