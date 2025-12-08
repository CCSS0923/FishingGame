cmake_minimum_required(VERSION 3.16)

project(FishingGame LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DNOMINMAX)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/Game.cpp
    ${SRC_DIR}/Player.cpp
    ${SRC_DIR}/FishingSystem.cpp
    ${SRC_DIR}/Fish.cpp
    ${SRC_DIR}/FishManager.cpp
    ${SRC_DIR}/UI.cpp
    ${SRC_DIR}/Shop.cpp
    ${SRC_DIR}/Sprite.cpp
)

set(HEADERS
    ${SRC_DIR}/Game.h
    ${SRC_DIR}/Player.h
    ${SRC_DIR}/FishingSystem.h
    ${SRC_DIR}/Fish.h
    ${SRC_DIR}/FishManager.h
    ${SRC_DIR}/UI.h
    ${SRC_DIR}/Shop.h
    ${SRC_DIR}/Sprite.h
    ${SRC_DIR}/Resource.h
    ${SRC_DIR}/State.h
)

set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res/game.rc)

add_executable(FishingGame WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(FishingGame PRIVATE ${SRC_DIR})

target_link_libraries(FishingGame PRIVATE gdi32 gdiplus)

if(MSVC)
    target_compile_options(FishingGame PRIVATE /W4 /permissive-)
else()
    target_compile_options(FishingGame PRIVATE -Wall -Wextra -Wpedantic)
endif()
