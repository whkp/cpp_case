#include "Server.h"
#include "EventLoop.h"



int main() {
    //创建一个事件循环,事件循环中创建了epoll对象
    EventLoop *loop = new EventLoop(); 
    //创建一个服务器对象,服务器对象中创建了socket对象并绑定到服务器地址进行监听
    //服务器对象中创建了channel对象,并将channel对象注册到epoll对象中
    Server *server = new Server(loop); 
    loop->loop();
    return 0;
}