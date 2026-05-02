cmake_minimum_required(VERSION 3.10)
project(cpptui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

option(CPPTUI_BUILD_EXAMPLES "Build example programs" ON)

include_directories(${CMAKE_SOURCE_DIR})

if(CPPTUI_BUILD_EXAMPLES)
    message(STATUS "Configuring examples for ${PROJECT_NAME}...")

    file(GLOB EXAMPLE_SOURCES "examples/*.cpp")

    foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
        get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
        add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})

        # Optional: Put them in a folder for IDEs like VS or Xcode
        set_target_properties(${EXAMPLE_NAME} PROPERTIES FOLDER "Examples")
    endforeach()
endif()