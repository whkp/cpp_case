#include <bits/stdc++.h>

using namespace std;

//懒汉式单例模式
//第一次使用时才创建实例

class Singleton {
public:
    static Singleton* getInstance() {
        if(instance == nullptr) {
            lock_guard<mutex> lock(mtx_);
            if(instance == nullptr) {
                instance = new Singleton();
            }
        }
        return instance;
    }

    static void destroyInstance() {
        if(instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }

private:
    static mutex mtx_; // 互斥锁
    Singleton() {}
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    ~Singleton() {}
    static Singleton* instance;
};


//俄汉式单例模式

class Singleton2 {
public:
    static Singleton2* getInstance() {
        return instance;
    }

private:
    Singleton2() {}
    Singleton2(const Singleton2&) = delete;
    Singleton2& operator=(const Singleton2&) = delete;

    ~Singleton2() {}
    static Singleton2* instance;
};

//使用C++11的静态变量初始化特性，保证线程安全

class Singleton3 {
public:
    Singleton3 *getInstance() {
        static Singleton3 *instance;
        return instance;
    }
private:
    Singleton3() {}
    ~Singleton3() {}

    Singleton3(const Singleton3&) = delete;
    Singleton3& operator= (const Singleton3&) = delete;
};