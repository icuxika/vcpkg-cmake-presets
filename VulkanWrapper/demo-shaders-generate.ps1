if (!(Test-Path -Path demo-shaders-bin)) {
    New-Item -ItemType Directory -Path demo-shaders-bin
}

glslc.exe .\demo-shaders\shader-triangle.vert -o .\demo-shaders-bin\vert-triangle.spv
glslc.exe .\demo-shaders\shader-triangle.frag -o .\demo-shaders-bin\frag-triangle.spv

glslc.exe .\demo-shaders\shader-texture.vert -o .\demo-shaders-bin\vert-texture.spv
glslc.exe .\demo-shaders\shader-texture.frag -o .\demo-shaders-bin\frag-texture.spv

glslc.exe .\demo-shaders\shader.vert -o .\demo-shaders-bin\vert.spv
glslc.exe .\demo-shaders\shader.frag -o .\demo-shaders-bin\frag.spv
