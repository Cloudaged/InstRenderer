#Vulkan
find_package(Vulkan REQUIRED)

#Qt
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

#SDL2
link_directories(${CMAKE_SOURCE_DIR}/ThirdParty/sdl2/lib)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/sdl2/include)
FILE(COPY ${CMAKE_SOURCE_DIR}/ThirdParty/sdl2/bin/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR}/Bin)

#glm
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/glm)

#Assimp
link_directories(${CMAKE_SOURCE_DIR}/ThirdParty/Assimp/lib)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/Assimp/include)
FILE(COPY ${CMAKE_SOURCE_DIR}/ThirdParty/Assimp/bin/libassimp-5.dll DESTINATION ${CMAKE_BINARY_DIR}/Bin)

#stb_image
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/stb_image
        ${CMAKE_SOURCE_DIR}/ThirdParty/tinygltf
        ${CMAKE_SOURCE_DIR}/ThirdParty/entt
        ${CMAKE_SOURCE_DIR}/ThirdParty/json
        ${CMAKE_SOURCE_DIR}/ThirdParty/volk
        ${CMAKE_SOURCE_DIR}/ThirdParty/hhh)

#meshopt
link_directories(${CMAKE_SOURCE_DIR}/ThirdParty/meshopt/bin)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty/meshopt/include)
FILE(COPY ${CMAKE_SOURCE_DIR}/ThirdParty/meshopt/bin/libmeshoptimizer.dll DESTINATION ${CMAKE_BINARY_DIR}/Bin)

