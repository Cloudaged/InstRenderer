@echo off
setlocal

rem 设置着色器文件夹和输出文件夹为上一级目录
set SHADER_DIR="..\Asset\Shader\src"
set OUTPUT_DIR="..\Asset\Shader\spv"

for %%f in (%SHADER_DIR%\*.vert) do (
    echo Compiling %%f...
    glslc "%%f" -o "%OUTPUT_DIR%\%%~nf.vert.spv"
    if %ERRORLEVEL% neq 0 (
        echo Failed to compile %%f!
        exit /b %ERRORLEVEL%
    )
)

rem 编译所有 .frag 文件
for %%f in (%SHADER_DIR%\*.frag) do (
    echo Compiling %%f...
    glslc "%%f" -o "%OUTPUT_DIR%\%%~nf.frag.spv"
    if %ERRORLEVEL% neq 0 (
        echo Failed to compile %%f!
        exit /b %ERRORLEVEL%
    )
)

echo All shaders compiled successfully!

endlocal