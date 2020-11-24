cmake_minimum_required(VERSION 3.16)
project(homesocks C)

set(CMAKE_C_STANDARD 99)

add_executable(homesocks
        network/io.h
        protocol/request.h
        network/io.c
        protocol/request.c
        main.c
        system/args.c
        system/args.h
        system/options.c
        system/options.h
        system/server.c
        system/server.h
        misc/defs.h
        protocol/method_exchange.c
        protocol/method_exchange.h
        protocol/reply.c
        protocol/reply.h
        system/log.c
        system/log.h
        misc/utils.c
        misc/utils.h
        misc/chrono.c
        misc/chrono.h protocol/commands/connect.c protocol/commands/connect.h protocol/commands/bind.c protocol/commands/bind.h protocol/commands/udp_associate.c protocol/commands/udp_associate.h)

target_link_libraries(homesocks pthread m)
target_include_directories(homesocks PUBLIC .)