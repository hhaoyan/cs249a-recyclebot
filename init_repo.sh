#!/bin/bash

echo "Updating buckler repo"
git submodule init && git submodule update
cd buckler
echo "Updating nrf52 SDK repo"
git submodule init && git submodule update
