PROJECT(Engine)

file(GLOB_RECURSE HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


add_library(${PROJECT_NAME} SHARED ${HEADER} ${SOURCE} ../../ThirdParty/volk/volk.c)
target_link_libraries(${PROJECT_NAME}
        Vulkan::Vulkan
        mingw32
        SDL2main
        SDL2
        assimp
        )
