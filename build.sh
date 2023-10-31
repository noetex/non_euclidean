#!/bin/bash

gcc euclid.cpp -o thing -Wall -g -ggdb -lGL -lGLEW -lm -lglfw -lstdc++ -D EUCLID_SYSTEM_LINUX -D EUCLID_COMPILER_GCC -D EUCLID_BUILD_DEBUG
