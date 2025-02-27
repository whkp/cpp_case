#include <iostream>
#include "pine.h"

int main(int argc, char* argv[]) {
    TcpServer* server = new TcpServer();
    server->OnConnection([](Connection* conn) {
        std::cout << "New connection, fd: " << conn->getSocket()->getFd() << std::endl;
    });
    server->OnRecv([](Connection* conn) {
        std::cout << "Message from client: " << conn->getSocket()->getFd() << std::endl;
        conn->Send(conn->getReadBuffer()->to_str());
    });
    server->Start();
    delete server;
    return 0;
}