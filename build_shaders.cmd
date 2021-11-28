@echo off

powershell del ./bin/triangle_v.spv
powershell del ./bin/triangle_f.spv

powershell Copy-Item ./shaders/triangle.vs ./shaders/triangle.vert
powershell Copy-Item ./shaders/triangle.fs ./shaders/triangle.frag

echo Triangle Vertex Shader Compilation
glslc ./shaders/triangle.vert -o ./bin/triangle_v.spv
echo  ---

echo Triangle Fragment Shader Compilation
glslc ./shaders/triangle.frag -o ./bin/triangle_f.spv
echo  ---

powershell del ./shaders/triangle.vert
powershell del ./shaders/triangle.frag