
glslc.exe -fshader-stage=vertex triangle.vert.glsl -o triangle.vert.spv
glslc.exe -fshader-stage=fragment triangle.frag.glsl -o triangle.frag.spv

glslc.exe -fshader-stage=vertex mesh_vertex_color.vert.glsl -o mesh_vertex_color.vert.spv
glslc.exe -fshader-stage=fragment mesh_vertex_color.frag.glsl -o mesh_vertex_color.frag.spv

pause
