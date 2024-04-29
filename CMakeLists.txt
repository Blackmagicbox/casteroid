cmake_minimum_required(VERSION 3.28)
project(casteroid C)

set(CMAKE_C_STANDARD 11)

add_executable(casteroid main.c)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(casteroid SDL2)

find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(casteroid SDL2_image)

find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIRS})
target_link_libraries(casteroid SDL2_ttf)

find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})
target_link_libraries(casteroid SDL2_mixer)
