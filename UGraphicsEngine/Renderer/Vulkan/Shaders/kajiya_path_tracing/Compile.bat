
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\pt_closesthit.rchit -o .\spv\pt_closesthit.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\pt_completemiss.rmiss -o .\spv\pt_completemiss.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\pt_normals_closesthit.rchit -o .\spv\pt_normals_closesthit.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\pt_raygeneration.rgen -o .\spv\pt_raygeneration.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\pt_shadowmiss.rmiss -o .\spv\pt_shadowmiss.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\pt_worldspacepos_closesthit.rchit -o .\spv\pt_worldspacepos_closesthit.rchit.spv

pause
