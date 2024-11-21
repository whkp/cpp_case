#pragma once

class Socket
{
public:
    Socket();
    Socket(int fd);
    ~Socket();
    void bind();
    void listen();
    void setnonblocking();
    int accept();
    int getfd();

private:
    int fd;
}