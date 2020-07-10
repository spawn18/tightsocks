cmake_minimum_required(VERSION 3.16)
project(socks C)

set(CMAKE_C_STANDARD 99)

add_executable(socks
        include/convert.h
        include/dnslookup.h
        include/io.h
        include/types.h
        include/socks.h
        include/message.h
        include/methods.h
        src/convert.c
        src/dnslookup.c
        src/io.c
        src/socks.c
        src/message.c
        src/methods.c
        main.c)