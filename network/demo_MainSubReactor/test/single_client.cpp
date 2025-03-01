#include "Connection.h"
#include "Socket.h"
#include <iostream>

int main() {
  Socket *sock = new Socket();
  sock->Connect("127.0.0.1", 1234);

  Connection *conn = new Connection(nullptr, sock ->getFd());

  while (true) {
    conn->Send("Hello, server!");
    if (conn->getState() == Connection::State::Closed) {
      conn->Close();
      break;
    }
    conn->Read();
    std::cout << "Message from server: " << conn->getReadBuffer() << std::endl;
  }

  delete conn;
  return 0;
}
