#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <deque>
#include <string>

class test
{
private:
    std::string time ;
    double data ;
public:
    test() = default;
    test(const std::string &time, const double &data):
    time(time), data(data){ }
    ~test();
    
    void print_test(){
        std::cout << "Time: " << time << std::endl;
        std::cout << "Data: " << data << std::endl;
    }
};

void print_vector_int(const std::vector<int>& a ){
    for(auto c:a){
        std::cout << c << " ";
    }
    std::cout << std::endl;
}

