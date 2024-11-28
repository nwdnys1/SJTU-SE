#!/bin/bash

echo "Start installing minisat..."
sudo apt update
sudo apt install libc6=2.35-0ubuntu3 libc-bin zip build-essential minisat zlib1g zlib1g-dev -y --allow-downgrades
echo "Installation complete"