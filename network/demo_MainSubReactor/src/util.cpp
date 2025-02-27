#include "util.h"

#include <cstdio>
#include <cstdlib>

// 错误处理函数
void ErrorIf(bool condition, const char* msg) {
    if(condition) {
        perror(msg); // 打印错误信息
        exit(EXIT_FAILURE); // 退出程序s
    }
}