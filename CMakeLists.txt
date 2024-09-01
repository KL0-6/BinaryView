cmake_minimum_required(VERSION 3.8)

project(BinaryView)

include(FetchContent)

FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG master)
FetchContent_MakeAvailable(imgui)

set(PLATFORM_LIBS)
set(PLATFORM_SOURCES)

if (NOT MSVC)
    find_package(glfw3)
    if(APPLE)
        list(APPEND PLATFORM_LIBS "-framework OpenGL")
    else()
        list(APPEND PLATFORM_LIBS GL)
    endif()
else()
    add_subdirectory(glfw)
    list(APPEND PLATFORM_LIBS OpenGL32)
endif()

add_executable(BinaryView 
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    
    widgets/widgets.cpp

    PE/PE.cpp

    main.cpp
)

target_include_directories(BinaryView PRIVATE
    ${imgui_SOURCE_DIR} 
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(BinaryView PRIVATE
    ${PLATFORM_LIBS}
    glfw
)

set_target_properties(BinaryView PROPERTIES CXX_STANDARD 17)