#!/bin/zsh
mkdir -p shaders-bin
glslc ./demo-shaders/shader-triangle.vert -o ./demo-shaders-bin/vert-triangle.spv
glslc ./demo-shaders/shader-triangle.frag -o ./demo-shaders-bin/frag-triangle.spv

glslc ./demo-shaders/shader-texture.vert -o ./demo-shaders-bin/vert-texture.spv
glslc ./demo-shaders/shader-texture.frag -o ./demo-shaders-bin/frag-texture.spv

glslc ./demo-shaders/shader.vert -o ./demo-shaders-bin/vert.spv
glslc ./demo-shaders/shader.frag -o ./demo-shaders-bin/frag.spv
