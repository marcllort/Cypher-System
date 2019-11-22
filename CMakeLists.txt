cmake_minimum_required(VERSION 3.13)
project(Cypher-System)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable( Trinity src/main.cpp
		libs/dedicatedServer.h                 src/dedicatedServer.c
		libs/list.h                 src/list.c
		libs/client.h              src/client.c
		libs/network.h             src/network.c
		libs/config.h              src/config.c
        libs/utils.h               src/utils.c
        libs/io.h                  src/io.c
	libs/commandManager.h      src/commandManager.c
	libs/listds.h                 src/listds.c
	libs/packet.h                 src/packet.c
	libs/server.h                 src/server.c
)
