cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(glwrapper VERSION 0.1)

set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

set(GLFW_USE_WAYLAND ON)

# -----------------------------------------------------------------------------
# General options for CMAKE  files n output 
# -----------------------------------------------------------------------------
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
if (${FORCE_COLORED_OUTPUT})
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
  endif ()
endif ()


# -----------------------------------------------------------------------------
# Dependecies 
# -----------------------------------------------------------------------------
include(cmake/CPM.cmake)

CPMAddPackage("gh:KDE/extra-cmake-modules@6.5.0")

CPMAddPackage("gh:nothings/stb#master")

CPMAddPackage("gh:fmtlib/fmt#7.1.3")

CPMAddPackage("gh:bishan-batel/crab#main")

find_package(OpenGL REQUIRED)

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
CPMAddPackage("gh:glfw/glfw#3.3.8")

CPMAddPackage("gh:ocornut/imgui@1.91.0")

add_library(imgui STATIC)

target_sources(imgui PRIVATE 
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${imgui_SOURCE_DIR}/misc/cpp)   

# ----------------------------

if(MSVC)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR})
  endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
endif()

file(GLOB_RECURSE SOURCE_FILES 
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.cpp)
	
file(GLOB_RECURSE HEADER_FILES 
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.hpp)


add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${PROJECT_SOURCE_DIR}/libs/glad/src/glad.c)

add_custom_target(copy_assets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)   

add_dependencies(${PROJECT_NAME} copy_assets)

# Compile Flags

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

if (MSVC)
  add_compile_options(/Wall /WX /W2 /permissive-)
  add_link_options(/WX)
else ()
  add_compile_options(${PROJECT_NAME} -Wall -Wextra -Wpedantic -Werror -std=c++23 -stdlib=libc++ -ibuiltininc)
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    "DEBUG=$<IF:$<CONFIG:Debug>,1,0>" 
    "RELEASE=$<IF:$<CONFIG:Debug>,0,1>"
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_compile_options(-g -O3)
else ()
  add_compile_options(-O3)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC 
  ${PROJECT_SOURCE_DIR}/src 
  ${PROJECT_SOURCE_DIR}/libs/glad/include 
  ${crab_SOURCE_DIR}/include 
  ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE  ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} fmt crab glfw imgui)

# vim:shiftwidth=2:
