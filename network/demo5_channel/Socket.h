#pragma once

class InetAddress;

class Socket {
private:
    int fd;
public:
    Socket(int fd);
    Socket();
    ~Socket();

    void bind(const InetAddress& addr);
    void listen();
    void setnonblocking();
    int accept(InetAddress& addr);

    int getfd() const;
}