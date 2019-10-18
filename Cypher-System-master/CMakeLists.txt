cmake_minimum_required(VERSION 3.13)
project(Cypher-System)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable( Trinity             src/main.c
        libs/config.h              src/config.c
        libs/utils.h               src/utils.c
        libs/io.h                  src/io.c
        libs/network.h                 src/network.c)