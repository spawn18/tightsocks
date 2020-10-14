cmake_minimum_required(VERSION 3.16)
project(Ace C)

set(CMAKE_C_STANDARD 99)

add_executable(Ace
        network/io.h
        protocol/request.h
        system/log.h
        protocol/methods/userpass.h
        misc/utils.h
        network/io.c
        protocol/request.c
        system/log.c
        protocol/methods/userpass.c
        misc/utils.c
        main.c
        system/args.c
        system/args.h
        system/options.c
        system/options.h
        system/server.c
        system/server.h
        protocol/commands.c
        protocol/commands.h
        misc/defs.h
        protocol/method_exchange.c
        protocol/method_exchange.h
        protocol/reply.c
        protocol/reply.h misc/chrono.c misc/chrono.h misc/wrappers.c misc/wrappers.h)

target_link_libraries(Ace pthread)
target_include_directories(Ace PUBLIC .)