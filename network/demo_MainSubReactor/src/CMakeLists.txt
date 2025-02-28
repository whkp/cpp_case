project(network)
set(net_src
    Acceptor.cpp
    Connection.cpp
    Eventloop.cpp
    Channel.cpp
    Buffer.cpp
    Epoll.cpp
    util.cpp
    TcpServer.cpp
    Socket.cpp
    )
add_library(${PROJECT_NAME} SHARED ${net_src})
target_link_libraries(network ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS network DESTINATION lib)