#!/usr/bin/env bash
cmake -S . -B build
cmake --build build -j
sudo cmake --install build --prefix /usr/local