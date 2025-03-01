#include <iostream>
#include <unistd.h>
#include "pine.h"

int main(int argc, char* argv[]) {
    TcpServer* server = new TcpServer();

    Signal::signal(SIGINT, [&] {
        delete server;
        std::cout << "Server exit" << std::endl;
        exit(0);
    });
    server->OnConnection([](Connection* conn) {
        //建立连接后先接受客户端的数据，然后回显
        std::cout << "New connection, fd: " << conn->getSocket()->getFd() << std::endl;
        conn -> Read(); // read data
        if(conn->getState() == Connection::State::Closed) {
            conn->Close();
            return;
        }
        std::cout << "message from client: " << conn->ReadBufferToStr() << std::endl;
        conn->setSendBuffer(conn -> ReadBufferToStr());
        conn->Write();
    });
    server->Start();
    delete server;
    return 0;
}