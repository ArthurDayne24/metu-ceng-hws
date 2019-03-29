cmake_minimum_required(VERSION 3.12)
project(hw3 C)

set(CMAKE_C_STANDARD 11)

include(FindPCAP.cmake)

set(SOURCE_FILES main.c)
add_executable(hw3c
        commons.c
        callbacks.c
        main.c)
target_link_libraries(hw3c ${PCAP_LIBRARY})
