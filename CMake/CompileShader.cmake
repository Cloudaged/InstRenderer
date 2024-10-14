add_custom_target(compile_shaders ALL
        COMMAND cmd.exe /c ${CMAKE_SOURCE_DIR}/Tools/ShaderCompileTool.bat
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Compiling all shaders"
        )