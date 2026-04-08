cmake_minimum_required(VERSION 4.0)
project(cpp-player VERSION 0.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Поиск pkg-config
find_package(PkgConfig REQUIRED)

# libnotify и glib
pkg_check_modules(LIBNOTIFY REQUIRED libnotify)
pkg_check_modules(GLIB REQUIRED glib-2.0)

# PortAudio
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

# Библиотека cpp-player
add_library(cpp-player STATIC 
    cpp-player.cpp 
    cpp-player.h
    globals.cpp
    globals.h
)

# Все необходимые библиотеки
target_include_directories(cpp-player PRIVATE 
    ${PORTAUDIO_INCLUDE_DIRS}
    ${LIBNOTIFY_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    "../include"
    ".."
)

target_compile_options(cpp-player PRIVATE ${LIBNOTIFY_CFLAGS_OTHER} ${GLIB_CFLAGS_OTHER})

target_link_libraries(cpp-player PRIVATE 
    sndfile 
    asound
    ${PORTAUDIO_LIBRARIES}
    ${LIBNOTIFY_LIBRARIES}
    ${GLIB_LIBRARIES}
)

add_executable(player-cpp main.cpp)

target_include_directories(player-cpp PRIVATE 
    ${LIBNOTIFY_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${PORTAUDIO_INCLUDE_DIRS}
    "."
)

target_compile_options(player-cpp PRIVATE ${LIBNOTIFY_CFLAGS_OTHER} ${GLIB_CFLAGS_OTHER})
target_link_libraries(player-cpp PRIVATE cpp-player)