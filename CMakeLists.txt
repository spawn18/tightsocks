cmake_minimum_required(VERSION 3.0)
project(tightsocks VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

add_executable(tightsocks
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
        main.c)

target_link_libraries(tightsocks pthread)
target_include_directories(tightsocks PUBLIC .)

if(CMAKE_BUILD_TYPE MATCHES Debug)
        message("Debug config")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
        message("Release config")
else()
        message("Unknown config")
endif()
