#!/bin/zsh
mkdir -p shaders-bin
glslc ./shaders/shader.vert -o ./shaders-bin/vert.spv
glslc ./shaders/shader.frag -o ./shaders-bin/frag.spv
