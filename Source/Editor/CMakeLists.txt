PROJECT(Editor)

file(GLOB_RECURSE HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

file(GLOB_RECURSE SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

#add_executable(${PROJECT_NAME} ${HEADER} ${SOURCE})
add_library(${PROJECT_NAME} SHARED ${HEADER} ${SOURCE})
target_link_libraries(${PROJECT_NAME}
        Vulkan::Vulkan
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )