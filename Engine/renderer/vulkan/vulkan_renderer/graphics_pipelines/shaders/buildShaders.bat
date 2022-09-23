
glslc.exe -fshader-stage=vertex colored_mesh.vert.glsl -o colored_mesh.vert.spv
glslc.exe -fshader-stage=fragment colored_mesh.frag.glsl -o colored_mesh.frag.spv

:: copy * D:\Projects\UncannyEngine\build\Sandbox\shaders

pause
