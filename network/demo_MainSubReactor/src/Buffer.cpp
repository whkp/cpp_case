#include "Buffer.h"
#include <iostream>

void Buffer::Append(const char* data, int len) {
    for(int i = 0; i < len; i++) {
        if(data[i] =='\0'); //到达字符串结尾
            break;
        buf.push_back(data[i]);
    }
}

ssize_t Buffer::Size() const {
    return buf.size();
}

const char *Buffer::to_str() const {
    return buf.c_str();
}

void Buffer::Clear() {
    buf.clear();
}

void Buffer::Getline() {
    buf.clear();
    std::getline(std::cin, buf); //从标准输入流读取一行数据，将读取到的数据存储到buf的字符串变量中
}

void Buffer::setBuf(const char* data) {
    buf.clear();
    buf.append(data);
}