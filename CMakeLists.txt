cmake_minimum_required(VERSION 3.28)
project(bw)

set(CMAKE_CXX_STANDARD 23)

add_executable(bw main_project/main.cpp miniaudio/miniaudio.h)

# Wavelib --------------------------------
add_subdirectory(wavelib)
target_link_directories(bw PUBLIC wavelib)
# ----------------------------------------

# FLTK -----------------------------------
add_subdirectory(fltk)
target_link_directories(bw PUBLIC fltk)
# ----------------------------------------