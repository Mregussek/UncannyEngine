
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\antialiasinglambertian_shadow.rmiss -o .\spv\antialiasinglambertian_shadow.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\antialiasinglambertian.rmiss -o .\spv\antialiasinglambertian.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\antialiasinglambertian.rgen -o .\spv\antialiasinglambertian.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\antialiasinglambertian.rchit -o .\spv\antialiasinglambertian.rchit.spv

glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\shadows.rmiss -o .\spv\shadows.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\shadows.rchit -o .\spv\shadows.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\colors.rchit -o .\spv\colors.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\normals.rchit -o .\spv\normals.rchit.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\camera.rgen -o .\spv\camera.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rgen -o .\spv\default.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rmiss -o .\spv\default.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\default.rchit -o .\spv\default.rchit.spv

glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox_shadow_miss.rmiss -o .\spv\sandbox_shadow_miss.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox_complete_miss.rmiss -o .\spv\sandbox_complete_miss.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox.rgen -o .\spv\sandbox.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\sandbox.rchit -o .\spv\sandbox.rchit.spv

pause
