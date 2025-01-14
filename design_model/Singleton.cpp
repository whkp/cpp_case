/*
单例模式是一种创建型设计模式， 它的核心思想是保证一个类只有一个实例，并提供一个全局访问点来访问这个实例。
    只有一个实例的意思是，在整个应用程序中，只存在该类的一个实例对象，而不是创建多个相同类型的对象。
    全局访问点的意思是，为了让其他类能够获取到这个唯一实例，该类提供了一个全局访问点（通常是一个静态方法），通过这个方法就能获得实例。

单例模式的主要优点是：
    1. 控制实例数量：单例模式可以控制实例的数量，确保一个类只有一个实例，避免了实例的重复创建和销毁，提高了系统的性能。
    2. 简化访问：单例模式提供了一个全局访问点，使得其他类可以通过这个访问点来获取唯一的实例，简化了访问过程。
（节省资源、方便控制）

单例设计模式的基本要求
    私有的构造函数：防止外部代码直接创建类的实例
    私有的静态实例变量：保存该类的唯一实例
    公有的静态方法：通过公有的静态方法来获取类的实例

需要考虑线程安全问题
*/

//单例模式方式有：懒汉式、饿汉式、双重检查锁、静态内部类、枚举类

//懒汉式：在第一次调用getInstance()方法时，才会创建实例（需要使用再创建）

#include <iostream>
#include <mutex>
using namespace std;

class Singleton {
    public:
    //获取单个实例对象
        static Singleton* getInstance() {
            if(pSingleton == nullptr) {
                pSingleton = new Singleton();
            }
            return pSingleton;
        }
    private:
        //构造函数私有化，防止外部代码直接创建类的实例
        Singleton() {}
        //防止外部析构
        ~Singleton() {}
        //防止外部拷贝
        Singleton(const Singleton& single) = delete;
        //防止外部赋值
        const Singleton &operator=(const Singleton& single) = delete;

    private:
        static Singleton* pSingleton;
};

//使用互斥锁保证线程安全
class Singleton2 {
    public:
    //获取单个实例对象
    static Singleton2* getInstance() {
        if(pSingleton == nullptr) {
            lock_guard<mutex> lock(m_mutex); //加锁
            if(pSingleton == nullptr) {
                volatile auto tmp = new Singleton2();
                pSingleton = tmp;
            }
        }
        return pSingleton;
    }
    void deleteInstance() {
        unique_lock<mutex> lock(m_mutex); //加锁
        if(pSingleton != nullptr) {
            delete pSingleton;
            pSingleton = nullptr;
        }
    }
    private:
        Singleton2() {};
        ~Singleton2() {};
        Singleton2(const Singleton2& single) = delete;
        const Singleton2 &operator=(const Singleton2& single) = delete;

    private:
        static Singleton2* pSingleton;
        static mutex m_mutex;
};

//俄汉式：在类加载时就完成了实例化，避免了线程同步问题
class Singleton3 {
    private:
        static Singleton3* pSingleton;
    public:
        static Singleton3* getInstance() {
            return pSingleton;
        }
        deleteInstance() {
            if(pSingleton != nullptr) {
                delete pSingleton;
                pSingleton = nullptr;
            }
        }
    private:
        Singleton3() {};
        ~Singleton3() {};
};