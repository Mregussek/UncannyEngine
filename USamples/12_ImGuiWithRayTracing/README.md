
# 12_ImGuiWithRayTracing

<p align="center">
  <img width="960" height="526" src="../../Media/SamplesPictures/12_ImGuiWithRayTracing.png">
</p>

## Issues

There can be potential issue with this sample from imgui: https://github.com/ocornut/imgui/issues/1817

With first run program should run properly, but afterwards when imgui.ini file is created, ImDrawData* pointer will null
causing a crash. You can resolve this issue by just deleting contents of imgui.ini file.

