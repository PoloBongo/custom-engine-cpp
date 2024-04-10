@echo off

rem Recherche du chemin vers glslc.exe dans le répertoire Bin du Vulkan SDK
for /f "delims=" %%i in ('dir /b /s /a-d "C:\VulkanSDK\*\Bin\glslc.exe"') do set "glslc=%%i"

if "%glslc%"=="" (
    echo Erreur: glslc.exe non trouvé dans le répertoire d'installation de Vulkan SDK.
    exit /b 1
)

set "shader_dir=%~dp0Shaders"

rem Compilation des fichiers .vert
for %%f in ("%shader_dir%\*.vert") do (
    echo Compiling: %%f
    "%glslc%" -c "%%f" -o "%%~dpnf.spv"
)

rem Compilation des fichiers .frag
for %%f in ("%shader_dir%\*.frag") do (
    echo Compiling: %%f
    "%glslc%" -c "%%f" -o "%%~dpnf.spv"
)

echo Compilation des shaders terminée.