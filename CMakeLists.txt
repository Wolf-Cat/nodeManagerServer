cmake_minimum_required(VERSION 3.15.2)
project(server_discover)

set(CMAKE_CXX_STANDARD 11)

add_definitions("-Wall -g")   #编译debug版本

#引入头文件目录
include_directories(./common)
include_directories(./core)
include_directories(./serverDiscover)
include_directories(./3rd/log)
include_directories(./3rd/json)

set(SRCFILE_ALL
        main.cpp
        core/threadPool.cpp
        common/utils.cpp
        common/dataProtocolHead.cpp
        common/pbCodec.cpp
        3rd/log/log4z.cpp
        3rd/log/mainLog.cpp
        3rd/json/jsoncpp.cpp
        serverDiscover/message.pb.cpp
        core/xthread.cpp
        serverDiscover/nodeConnect.cpp
        serverDiscover/httpReq.cpp
        serverDiscover/global.cpp
        )

add_subdirectory(./client)

#可执行文件名称 文件
add_executable(exe_server_discover ${SRCFILE_ALL})

# 显式指定libevent动态库, 必须放在可执行文件后面
target_link_libraries(exe_server_discover libevent.so pthread protobuf)
# 如果是显式指定静态库
# target_link_libraries(server_discover_exe libevent.a)

# /usr/local/include
# /usr/local/lib
# /usr/local/bin

# usr 很多人都认为是user缩写，其实不然，是unix system resource缩写