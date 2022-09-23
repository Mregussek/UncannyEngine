
glslc.exe -fshader-stage=vertex triangle.vert.glsl -o triangle.vert.spv
glslc.exe -fshader-stage=fragment triangle.frag.glsl -o triangle.frag.spv

glslc.exe -fshader-stage=vertex colored_mesh.vert.glsl -o colored_mesh.vert.spv
glslc.exe -fshader-stage=fragment colored_mesh.frag.glsl -o colored_mesh.frag.spv

pause
