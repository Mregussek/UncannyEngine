
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\camera.rgen -o .\spv\camera.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\colors.rchit -o .\spv\colors.rchit.spv

glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rgen -o .\spv\default.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rmiss -o .\spv\default.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rchit -o .\spv\default.rchit.spv

glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\normals.rchit -o .\spv\normals.rchit.spv

glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\reflectionsrefractions.rchit -o .\spv\reflectionsrefractions.rchit.spv

glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\shadows.rmiss -o .\spv\shadows.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\shadows.rchit -o .\spv\shadows.rchit.spv

pause
