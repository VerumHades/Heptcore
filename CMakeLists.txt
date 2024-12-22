cmake_minimum_required(VERSION 3.14)  # FetchContent is fully supported from CMake 3.14 onwards
# Project name
project(Heptcore LANGUAGES C CXX)
#set(CMAKE_C_COMPILER gcc) 
#set(CMAKE_CXX_COMPILER g++)  # Set g++ as the C++ compiler

set(CMAKE_C_STANDARD 11)
# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_PARALLEL_LEVEL 8)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external/include)

add_library(glad external/src/glad.c)

include(FetchContent)

# Fetch GLFW
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git)
FetchContent_MakeAvailable(glfw)

# Fetch GLM
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git)
FetchContent_MakeAvailable(glm)


# Suppress all warnings for GLFW
target_compile_options(glfw PRIVATE -w)
target_compile_options(glm PRIVATE -w)

# Declare FreeType as a dependency
FetchContent_Declare(freetype GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git)
FetchContent_MakeAvailable(freetype)

find_package(OpenGL REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES
  ${CMAKE_SOURCE_DIR}/src/*.cpp
  ${CMAKE_SOURCE_DIR}/src/*.c
  ${CMAKE_SOURCE_DIR}/external/src/*.cpp
  ${CMAKE_SOURCE_DIR}/external/src/*.c
)

# Add executable target
add_library(Heptcore ${SOURCES})

# Link libraries
target_link_libraries(Heptcore PRIVATE glfw OpenGL::GL glm::glm glad freetype)

target_compile_options(Heptcore PRIVATE -Wall)
target_include_directories(Heptcore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(Heptcore PUBLIC cxx_std_20)

install(TARGETS Heptcore EXPORT HeptcoreTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

# Export the CMake configuration for use with FetchContent
install(EXPORT HeptcoreTargets
    FILE mylibraryConfig.cmake
    NAMESPACE Heptcore::
    DESTINATION lib/cmake/heptcore
)