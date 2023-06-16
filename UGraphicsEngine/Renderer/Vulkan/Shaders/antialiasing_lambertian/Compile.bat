
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\antialiasinglambertian_shadow.rmiss -o .\spv\antialiasinglambertian_shadow.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\antialiasinglambertian.rmiss -o .\spv\antialiasinglambertian.rmiss.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\antialiasinglambertian.rgen -o .\spv\antialiasinglambertian.rgen.spv
glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\antialiasinglambertian.rchit -o .\spv\antialiasinglambertian.rchit.spv

glslc.exe --target-env=vulkan1.3 --target-spv=spv1.6 .\reflectionsrefractions.rchit -o .\spv\reflectionsrefractions.rchit.spv

pause
