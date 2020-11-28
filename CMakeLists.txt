cmake_minimum_required(VERSION 3.0)
project(homesocks C)

set(CMAKE_C_STANDARD 99)

add_executable(homesocks
        network/io.c
        system/args.c
        system/options.c
        system/server.c
        system/log.c
        misc/utils.c
	misc/chrono.c
        protocol/method_exchange.c
        protocol/reply.c
        protocol/request.c
        protocol/commands/connect.c 
	protocol/commands/bind.c
	protocol/commands/udp_associate.c
        main.c)

target_link_libraries(homesocks pthread)
target_include_directories(homesocks PUBLIC .)
