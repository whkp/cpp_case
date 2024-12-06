#include <iostream>
#include <signal.h>
#include <unistd.h>
#include "server.h"

using namespace std;

server TcpServer; // global server object

//父进程信号处理函数
void FathEXIT(int sig) {
    //防止父进程被信号中断
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    cout << "父进程退出" << endl;
    kill(0, SIGTERM); //向所有子进程发送SIGTERM信号
    TcpServer.close_listen();
    exit(0);
}

//子进程信号处理函数
void ChldEXIT(int sig) {
    //防止子进程被信号中断
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    cout << "子进程" << getpid() <<  "退出, sig = " << sig << endl;
    TcpServer.close_client();
    exit(0);

}

int duoxiancheng_main(int argc, char* argv[]) {
    if(argc != 2) {
        cout << "Usage: ./test_serv <port> \nExample: ./test_serv 8080\n";
        return -1;
    }
    // Ignore all signals
    for(int i = 0; i < 64; i++) {
        signal(i, SIG_IGN);
    }
    signal(SIGTERM, FathEXIT);
    signal(SIGINT, FathEXIT);
    if(TcpServer.init_server(atoi(argv[1])) == false) {
        perror("init_server");
        return -1;
    }
    //父进程每受理一个客户端连接就fork一个子进程处理，子进程与客户端进行通信，父进程只受理客户端连接
    while(1) {
        //如果没有客户端连接，则继续监听
        if(TcpServer.accept_client() == false) {
            perror("accept_client");
            return -1;
        }
        int pid = fork(); // fork子进程，子进程返回0，父进程返回子进程pid，出错返回-1
        if(pid == -1) {
            perror("fork");
            return -1;
        } else if(pid > 0) {
            //父进程
            TcpServer.close_client(); // 父进程关闭客户端连接
            continue; //父进程继续监听
        }
        //子进程
        TcpServer.close_listen(); // 子进程关闭监听套接字
        signal(SIGTERM, ChldEXIT); // 子进程退出函数
        signal(SIGINT, ChldEXIT);  // 子进程不需要捕获SIGINT信号
        cout << "客户端连接成功\n";
        string buf;
        while(1) {
            if(TcpServer.recv_data(buf, 1024) == false) {
                perror("recv_data");
                break;
            }
            cout << "收到客户端数据：" << buf << endl;
            if(TcpServer.send_data(buf) == false) {
                perror("send_data");
                break;
            }
            cout << "发送客户端数据：" << buf << endl;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    //发送文件
    if(argc != 3) {
        cout << "Usage: ./test_serv <port> <file_dir> \nExample: ./test_serv 5005 /tmp \n";
        return -1;
    }
    //忽略所有信号
    for(int ii = 0; ii < 64; ii++) {
        signal(ii, SIG_IGN);
    }
    signal(SIGTERM, FathEXIT);
    signal(SIGINT, FathEXIT);
    //初始化服务器,监听端口
    if(TcpServer.init_server(atoi(argv[1])) == false) {
        perror("init_server");
        return -1;
    }
    
    while(true) {
        //受理客户端连接
        if(TcpServer.accept_client() == false) {
            perror("accept_client");
            return -1;
        }
        //创建子进程处理客户端连接
        int pid = fork();
        if(pid == -1) {
            perror("fork");
            return -1;
        } else if(pid > 0) {
            //父进程
            TcpServer.close_client(); // 父进程关闭客户端连接
            continue; //父进程继续监听
        }
        //子进程
        TcpServer.close_listen(); // 子进程关闭监听套接字
        //重新设置信号处理函数
        signal(SIGTERM, ChldEXIT); // 子进程退出函数
        signal(SIGINT, ChldEXIT);  // 子进程不需要捕获SIGINT信号

        //与客户端通讯
        cout << "客户端" << TcpServer.get_client_ip() << "连接成功\n";
        
        //接受文件流程
        struct st_file_info {
            char filename[256];
            int   filesize;
        } file_info;
        //结构体接受文件信息
        memset(&file_info, 0, sizeof(file_info));
        if(TcpServer.recv_data(&file_info, sizeof(file_info)) == false) {
            perror("recv_data");
            return -1;
        }
        cout << "文件信息结构体" << file_info.filename << "(" << file_info.filesize <<")"<< endl;

        //客户端发送确认报文
        if(TcpServer.send_data("ok") == false) {
            perror("send_data");
            return -1;
        }

        //接收文件
        if(TcpServer.recv_file(std::string(argv[2]) + "/" + file_info.filename, file_info.filesize) == false) {
            cout << "接收文件失败" << endl;
            return -1;
        }
        cout << "接收文件成功" << endl;
        //客户端发送确认报文
        if(TcpServer.send_data("ok") == false) {
            perror("send_data");
            return -1;
        }
        return 0;

    }

    
        
}