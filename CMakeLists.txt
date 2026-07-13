cmake_minimum_required(VERSION 3.24)

project(paranoid
    VERSION 1.1.0
    DESCRIPTION "Paranoid - an Arkanoid clone"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(PARANOID_SANITIZE "Build with address and undefined-behavior sanitizers" OFF)
option(PARANOID_DEV_MODE "Enable developer cheats (defines DEV_MODE)" OFF)

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)

# Shared compile options for every target in the project.
add_library(paranoid_options INTERFACE)
target_compile_options(paranoid_options INTERFACE -Wall -Wextra -Wpedantic)

if(PARANOID_SANITIZE)
    target_compile_options(paranoid_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(paranoid_options INTERFACE -fsanitize=address,undefined)
endif()

if(PARANOID_DEV_MODE)
    target_compile_definitions(paranoid_options INTERFACE DEV_MODE)
endif()

# Reusable engine layer: rendering, animation, audio and timing on top of SDL3.
add_library(paranoid_engine STATIC
    src/engine/AnimatedSprite.cpp
    src/engine/Animation.cpp
    src/engine/AudioDevice.cpp
    src/engine/BitmapFont.cpp
    src/engine/Clock.cpp
    src/engine/Gamepad.cpp
    src/engine/Music.cpp
    src/engine/Sound.cpp
    src/engine/SoundBuffer.cpp
    src/engine/Sprite.cpp
    src/engine/Text.cpp
    src/engine/Texture.cpp
    src/engine/Transformable.cpp
    src/engine/Window.cpp
    third_party/stb/stb_vorbis.c
)

target_include_directories(paranoid_engine PUBLIC src)
target_include_directories(paranoid_engine PRIVATE third_party/stb)

# Vendored third-party code is not held to our warning level.
set_source_files_properties(third_party/stb/stb_vorbis.c PROPERTIES COMPILE_OPTIONS "-w")

target_link_libraries(paranoid_engine
    PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
    PRIVATE
        paranoid_options
)

# The game itself: entities, states and wiring.
add_library(paranoid_game STATIC
    src/CBall.cpp
    src/CBonus.cpp
    src/CBrick.cpp
    src/CEntity.cpp
    src/CGameContainer.cpp
    src/CGameStateManager.cpp
    src/CLaser.cpp
    src/CMapState.cpp
    src/CMenu.cpp
    src/CMenuState.cpp
    src/CPaddle.cpp
    src/CPlayState.cpp
    src/CResourceHolder.cpp
    src/CStageStore.cpp
)

target_link_libraries(paranoid_game
    PUBLIC
        paranoid_engine
    PRIVATE
        paranoid_options
)

add_executable(paranoid
    src/main.cpp
)

target_link_libraries(paranoid
    PRIVATE
        paranoid_game
        paranoid_options
)

enable_testing()
add_subdirectory(tests)
