cmake_minimum_required(VERSION 3.15.2)
set(CMAKE_CXX_STANDARD 11)
project(client)

add_definitions("-Wall -g")   #编译debug版本

include_directories(../common)
include_directories(../serverDiscover)
include_directories(./)
include_directories(../3rd/log)
include_directories(../3rd/json)

set(SRC_FILES
        ../serverDiscover/message.pb.cpp
        ../common/utils.cpp
        ../3rd/json/jsoncpp.cpp
        main.cpp)

set(EPOLL_FILES
        epollServer.cpp
        ../common/pbCodec.cpp
        ../serverDiscover/message.pb.cpp
        ../3rd/log/log4z.cpp
        ../3rd/log/mainLog.cpp
        ../common/dataProtocolHead.cpp
        handleEvent.cpp
        )

set(LIBEVENT_CLIENT_FILES
    libeventClient.cpp
        ../3rd/log/log4z.cpp
        ../3rd/log/mainLog.cpp
        ../common/pbCodec.cpp
        ../serverDiscover/message.pb.cpp
        ../common/dataProtocolHead.cpp
        ../common/utils.cpp
        ../3rd/json/jsoncpp.cpp
        ClientConnect.cpp
)

add_executable(clienttest ${SRC_FILES})
add_executable(epollServer ${EPOLL_FILES})
add_executable(libeventClient ${LIBEVENT_CLIENT_FILES})

target_link_libraries(clienttest libevent.so pthread protobuf)
target_link_libraries(epollServer pthread protobuf)
target_link_libraries(libeventClient libevent.so pthread protobuf)