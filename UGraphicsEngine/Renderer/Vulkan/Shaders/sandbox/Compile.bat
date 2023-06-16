
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox_shadow_miss.rmiss -o .\spv\sandbox_shadow_miss.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox_complete_miss.rmiss -o .\spv\sandbox_complete_miss.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox.rgen -o .\spv\sandbox.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox.rchit -o .\spv\sandbox.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox_normals.rchit -o .\spv\sandbox_normals.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox_worldspacepos.rchit -o .\spv\sandbox_worldspacepos.rchit.spv

pause
