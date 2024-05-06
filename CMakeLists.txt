cmake_minimum_required(VERSION 3.28)
project(casteroid C)

set(CMAKE_C_STANDARD 11)

add_executable(casteroid main.c)

if (WIN32)
    set(SDL2_PATH "C:/libs/SDL2")
    set(SDL2_INCLUDE_DIRS "${SDL2_PATH}/include")
    set(SDL2_LIBRARIES "${SDL2_PATH}/lib/x64")

    set(SDL2_IMAGE_PATH "C:/libs/SDL2_image")
    set(SDL2_IMAGE_INCLUDE_DIRS "${SDL2_IMAGE_PATH}/include")
    set(SDL2_IMAGE_LIBRARIES "${SDL2_IMAGE_PATH}/lib/x64")

    set(SDL2_TTF_PATH "C:/libs/SDL2_ttf")
    set(SDL2_TTF_INCLUDE_DIRS "${SDL2_TTF_PATH}/include")
    set(SDL2_TTF_LIBRARIES "${SDL2_TTF_PATH}/lib/x64")

    set(SDL2_MIXER_PATH "C:/libs/SDL2_mixer")
    set(SDL2_MIXER_INCLUDE_DIRS "${SDL2_MIXER_PATH}/include")
    set(SDL2_MIXER_LIBRARIES "${SDL2_MIXER_PATH}/lib/x64")

    # Include directories for headers
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})

    target_link_libraries(casteroid
            "${SDL2_LIBRARIES}/SDL2.dll"
            "${SDL2_LIBRARIES}/SDL2.lib"
            "${SDL2_LIBRARIES}/SDL2main.lib"
            "${SDL2_IMAGE_LIBRARIES}/SDL2_image.dll"
            "${SDL2_IMAGE_LIBRARIES}/SDL2_image.lib"
            "${SDL2_TTF_LIBRARIES}/SDL2_ttf.dll"
            "${SDL2_TTF_LIBRARIES}/SDL2_ttf.lib"
            "${SDL2_MIXER_LIBRARIES}/SDL2_mixer.dll"
            "${SDL2_MIXER_LIBRARIES}/SDL2_mixer.lib"
    )
else ()
    # For Linux use the default config
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
endif ()

# Copy resources to build directory
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
add_custom_command(TARGET casteroid POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RESOURCE_DIR} $<TARGET_FILE_DIR:casteroid>/resources)