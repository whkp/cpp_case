#include <bits/stdc++.h>

using namespace std;

class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }

    void print_value(int val) {
        cout << "val:" << val << endl;
    }

private:
    Singleton() {}
    Singleton(const Singleton&) = delete;

    Singleton& operator=(const Singleton&) = delete;
};

int main() {
    //要保持引用，不加引用会进行复制
    auto& s1 = Singleton::getInstance();
    auto& s2 = Singleton::getInstance();
    s1.print_value(1);
    cout << "s1 address:" << &s1 << endl;
    cout << "s2 address:" << &s2 << endl;
}