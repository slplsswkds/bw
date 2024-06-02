cmake_minimum_required(VERSION 3.28)
project(bw)

set(CMAKE_CXX_STANDARD 23)

add_executable(bw main_project/main.cpp)

# Wavelib --------------------------------
add_subdirectory(wavelib)
target_include_directories(bw PUBLIC wavelib/header)
target_link_libraries(bw PUBLIC wavelib)
# ----------------------------------------

# FLTK -----------------------------------
add_subdirectory(fltk)
target_include_directories(bw PUBLIC fltk)
target_link_libraries(bw PUBLIC fltk)
# ----------------------------------------

# Miniaudio ------------------------------
target_include_directories(bw PUBLIC miniaudio)
# ----------------------------------------
