
# UncannyEngine

Ongoing project, which I will use to learn Vulkan along with DearImGui.
Furthermore, I want to try implement ray tracing in future releases.

Build system: cmake

Used IDE: CLion

Compiler: Visual Studio 17 2022 MSVC 19.32.31332.0

## Build

For compilation, you should have Visual Studio installed! UncannyEngine is a library imported by UncannySandbox.

In the main CMakeLists.txt file in this repository, you should modify Vulkan SDK to your installed one.

```cmake
set(ENV{VULKAN_SDK} "<YOUR PATH TO INSTALLED VULKAN SDK>")
```

Example:

```cmake
set(ENV{VULKAN_SDK} "D:/SDKs/Vulkan/Vulkan_1.3.126.0")
```

### Automatic way

Just open in CLion as a CMake project and then load it. Afterwards CLion will get everything
prepared and ready to use.

### Manual Way

Create *build* directory and run in it cmake command:

```bash
mkdir build
cd build
cmake ..
```

At the end you should open generated .sln file and there compile the whole solution. UncannySandbox
should be set as a starting project and must be run in order to see results.

## Author

Mateusz Rzeczyca
