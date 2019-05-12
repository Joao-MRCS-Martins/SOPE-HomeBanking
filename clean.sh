#!/usr/bin/env bash
 
 rm server
 rm user
 rm ulog.txt
 rm slog.txt
 cd server_src
 make clean
 cd ../user_src
 make clean
 cd /tmp
 rm -rf secure_*