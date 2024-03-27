@echo off
set glslc="C:\VulkanSDK\1.3.275.0\Bin\glslc.exe"
set shader_dir="%~dp0Shaders"

for %%f in (%shader_dir%\*.vert) do (
    %glslc% %%f -o %shader_dir%\%%~nf.vert.spv
    echo %%f
)

for %%f in (%shader_dir%\*.frag) do (
    %glslc% %%f -o %shader_dir%\%%~nf.frag.spv
    echo %%f
)

echo Compilation des shaders terminée.