if (!(Test-Path -Path shaders-bin)) {
    New-Item -ItemType Directory -Path shaders-bin
  }

glslc.exe .\shaders\shader.vert -o .\shaders-bin\vert.spv
glslc.exe .\shaders\shader.frag -o .\shaders-bin\frag.spv
