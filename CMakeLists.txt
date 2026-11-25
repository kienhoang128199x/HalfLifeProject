cmake_minimum_required(VERSION 3.16)

project(HalfLifeClone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(HalfLifeClone
    src/main.cpp
)

target_link_libraries(HalfLifeClone
    PRIVATE
    glfw
    OpenGL::GL
    GLEW::GLEW
)

target_compile_definitions(HalfLifeClone PRIVATE
    ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)