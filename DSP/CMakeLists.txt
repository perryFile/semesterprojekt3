cmake_minimum_required(VERSION 3.16)

project(DSP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opret executable
add_executable(maintest
    maintest.cpp
    DSP.cpp
)

# PortAudio stier
target_include_directories(maintest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/include
)

target_link_directories(maintest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/lib/.libs
)

# Link PortAudio
target_link_libraries(maintest PRIVATE portaudio)