
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\materials.rchit -o .\materials.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\colors.rchit -o .\colors.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\normals.rchit -o .\normals.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\camera.rgen -o .\camera.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rgen -o .\default.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rmiss -o .\default.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rchit -o .\default.rchit.spv
pause
