
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox_shadow_miss.rmiss -o .\sandbox_shadow_miss.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox_complete_miss.rmiss -o .\sandbox_complete_miss.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox.rgen -o .\sandbox.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox.rchit -o .\sandbox.rchit.spv

glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\shadows.rmiss -o .\shadows.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\shadows.rchit -o .\shadows.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\colors.rchit -o .\colors.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\normals.rchit -o .\normals.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\camera.rgen -o .\camera.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rgen -o .\default.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rmiss -o .\default.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rchit -o .\default.rchit.spv
pause
