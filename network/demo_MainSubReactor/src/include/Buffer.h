#pragma once
#include <string>

class Buffer {
private:
    std::string buf;
public:
    Buffer() = default;
    ~Buffer() = default;

    void Append(const char* data, int len);  
    ssize_t Size() const;
    const char* to_str() const;
    void Clear();
    void Getline();
    void setBuf(const char*);
};