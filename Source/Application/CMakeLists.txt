PROJECT(EngineApplication)

include_directories(${CMAKE_SOURCE_DIR}/Source/Editor)
include_directories(${CMAKE_SOURCE_DIR}/Source/Engine)

file(GLOB_RECURSE EDITOR_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/Editor/*.h)

file(GLOB_RECURSE EDITOR_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/Editor/*.cpp)



add_executable(${PROJECT_NAME} main.cpp Application.h Application.cpp Presenter.cpp Presenter.h Editor/GameThread.h Editor/GameThread.cpp
        ${EDITOR_HEADER} ${EDITOR_SOURCE}
        ../Engine/Common/Core/WindowContext.h)

option(COMPILE_EDITOR "Enable editor" ON)
if (COMPILE_EDITOR)
    target_compile_definitions(EngineApplication PRIVATE WITH_EDITOR=1)
endif ()

target_link_libraries(${PROJECT_NAME}
        Engine
        mingw32
        SDL2main
        SDL2
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )


