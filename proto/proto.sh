#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
protoc ./*.proto --cpp_out=.
mv *.pb.h *.pb.cc ./build/

