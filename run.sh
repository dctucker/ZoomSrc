#!/bin/bash

make clean
clear
make && echo > log.txt && open zoomsrc.app && tail -f log.txt
