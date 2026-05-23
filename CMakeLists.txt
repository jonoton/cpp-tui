cmake_minimum_required(VERSION 3.10)
project(cpptui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_library(cpptui INTERFACE)
add_library(cpptui::cpptui ALIAS cpptui)
target_include_directories(cpptui INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CPPTUI_BUILD_EXAMPLES_DEFAULT ON)
else()
    set(CPPTUI_BUILD_EXAMPLES_DEFAULT OFF)
endif()

option(CPPTUI_BUILD_EXAMPLES "Build the example programs" ${CPPTUI_BUILD_EXAMPLES_DEFAULT})

if(CPPTUI_BUILD_EXAMPLES)
    file(GLOB EXAMPLE_SOURCES "examples/*.cpp")
    foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
        target_link_libraries(${EXAMPLE_NAME} PRIVATE cpptui::cpptui)
    endforeach()
endif()