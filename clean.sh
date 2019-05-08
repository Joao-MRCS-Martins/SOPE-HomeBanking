#!/usr/bin/env bash
 
 rm server
 rm user
 cd server_src
 make clean
 cd ../user_src
 make clean