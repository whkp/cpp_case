#include "client.h"

using namespace std;
int duoxiancheng_main(int argc, char *argv[]) {
    if(argc != 3) {
        cout << "Usage: ./client <server_ip> <server_port> \n  Example: ./client 127.0.0.1 8080\n";
        return -1;
    }
    client TcpClient;
    if(TcpClient.connect(argv[1], atoi(argv[2])) == -1) {
        perror("connect");
        return -1;
    }
    string buf;
    //循环三次，与服务端进行多次通讯
    for(int ii = 0; ii < 3; ii++) {
        buf = "这是第" + to_string(ii) + "次发送的数据";
        if(TcpClient.send(buf) == false) {
            perror("send");
            break;
        }
        if(TcpClient.recv(buf, 1024) == false) {
            perror("recv");
            break;
        }
        cout << "收到服务端返回的数据：" << buf << endl;
        sleep(1);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc != 5) {
        cout << "Usage: ./client <server IP> <server port> <file name> <file size>" << endl;
        cout << "Example: ./client 127.0.0.1 8080 file.txt 1024";
        return -1;
    }
    client TcpClient;
    if(TcpClient.connect(argv[1], atoi(argv[2])) == false) {
        perror("connect");
        return -1;
    }
    //发送文件名和文件大小
    struct st_file_info
    {
        char name[256];
        int size;
    } fileinfo;
    memset(&fileinfo, 0, sizeof(fileinfo));
    strcpy(fileinfo.name, argv[3]); //文件名
    fileinfo.size = atoi(argv[4]);  //文件大小
    //文件信息发送给服务端
    if(TcpClient.send(&fileinfo, sizeof(fileinfo)) == false) {
        perror("send");
        return -1;
    }
    cout << "发送文件信息结构体" << endl;
    string buffer;
    if(TcpClient.recv(buffer, 1024) == false) {
        perror("recv");
        return -1;
    }
    if(buffer == "ok") {
        cout << "文件信息发送成功" << endl;
    } else {
        cout << "服务端没有回复" << endl;
    }
    if(TcpClient.sendfile(fileinfo.name, fileinfo.size) == false) {
        perror("sendfile");
        return -1;
    }

    if(TcpClient.recv(buffer, 1024) == false) {
        perror("recv");
        return -1;
    }
    if(buffer == "ok") {
        cout << "文件内容发送成功" << endl;
    } else {
        cout << "服务端没有回复" << endl;
    }

    return 0;

}