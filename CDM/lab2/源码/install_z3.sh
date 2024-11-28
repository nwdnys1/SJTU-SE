#!/bin/bash

sudo apt install git flex bison -y
cd smt
git clone https://ghproxy.com/https://github.com/Z3Prover/z3.git z3
cd z3
python3 scripts/mk_make.py
cd build
make
sudo make install
make examples
cd ../../..
