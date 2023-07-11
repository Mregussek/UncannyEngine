
<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/UncannyEngine.png">
</p>

# UncannyEngine

Research-based Vulkan 3D engine. Currently my primary focus includes ray tracing pipeline and global illumination
algorithms. I have implemented Kajiya-style Path Tracing.

## Current state

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/SamplesPictures/cornell_box_spheres.png">
</p>

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/SamplesPictures/cornell_box_original.png">
</p>

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/SamplesPictures/fireplace.png">
</p>

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/SamplesPictures/default_scene.png">
</p>

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/SamplesPictures/12_ImGuiWithRayTracing.png">
</p>

### Debugging

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/SamplesPictures/cornell_box_spheres_normals.png">
</p>

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/SamplesPictures/cornell_box_spheres_hitworldspacepos.png">
</p>

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/SamplesPictures/default_scene_normals.png">
</p>

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/SamplesPictures/default_scene_hitworldspacepos.png">
</p>

## Architecture

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/architecture_uncanny_engine.png">
</p>

## Samples

In USamples directory you can find all sample projects that are using UncannyEngine.
Every sample project name that I have created starts from xx number, for example: *06_RayTracingInSeveralBLASes*

Models are downloaded from Morgan McGuire's [Computer Graphics Archive](https://casual-effects.com/data)

<p align="center">
  <img style="width: 55vw; min-width: 330px;" src="Media/usamples_example.png">
</p>

## Build

Clone repository:

```bash
git clone --recurse-submodules https://github.com/Mregussek/UncannyEngine.git
```

I am using CMake as a build system and MSVC as a compiler. Please use **Visual Studio 16 2019**! 

In order to build it you should use CMakeGUI and select project directory. In project directory please create
some build directory and mark it in GUI as follows.

<p align="center">
  <img style="width: 40vw; min-width: 330px;" src="Media/Compilation/cmake-build-config.png">
</p>

Next you can choose proper startup project, every sample project that I have created starts from xx number, for
example: *06_RayTracingInSeveralBLASes*

<p align="center">
  <img style="width: 25vw; min-width: 400px;" src="Media/Compilation/startup-project-selection.png">
</p>

Afterwards you can build project and you should running project :)

## Potential issues

### Wrong materials issue with .mtl files

For now engine's shaders parse material, where illumination model in .mtl file is as follows:

* illum 5 -> metallic / reflective
* illum 7 -> dielectric / refractive
* illum 4 -> emissive material / light
* other -> lambertian / matte material

### ImGui issue

Used version of ImGui may fail, when imgui.ini file is created in executable directory. It is related  to this issue
https://github.com/ocornut/imgui/issues/1817. This is why I have decided  to write special function that  deletes this
file, when engine is being released.

```cpp
void Application::DeleteImGuiIni()
{
  FPath imgui_ini = FPath::Append(FPath::GetExecutablePath(), "imgui.ini");
  FPath::Delete(imgui_ini);
}
```

So, if you are running sandbox sample or 12 imgui sample, make sure that imgui.ini is not already created and filled.
