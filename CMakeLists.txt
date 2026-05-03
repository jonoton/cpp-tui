cmake_minimum_required(VERSION 3.10)
project(cpptui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

include_directories(${PROJECT_SOURCE_DIR})

option(CPPTUI_BUILD_EXAMPLES "Build the example programs" ON)

if(CPPTUI_BUILD_EXAMPLES)
    file(GLOB EXAMPLE_SOURCES "examples/*.cpp")
    foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
    endforeach()
endif()