PROJECT(EngineApplication)

include_directories(${CMAKE_SOURCE_DIR}/Source/Editor)
include_directories(${CMAKE_SOURCE_DIR}/Source/Engine)

add_executable(${PROJECT_NAME} main.cpp Application.h Application.cpp)

target_link_libraries(${PROJECT_NAME}
        Editor
        Engine
        mingw32
        SDL2main
        SDL2

        )