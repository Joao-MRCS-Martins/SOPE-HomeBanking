#!/usr/bin/env bash

cd server_src
make clean; make
mv server ..
cd ../user_src
make clean; make
mv user ..