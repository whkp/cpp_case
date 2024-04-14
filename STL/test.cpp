
#include "test.h"


int main()
{
    //初始化
    std::vector<int> c{1,2,3,4};
    std::vector<int> c1(c);
    std::vector<int> c2(c.begin()+1, c.end());
    std::vector<int> c3 = c;
    c3.swap(c2);
    std::vector<int> c4(10);
    std::vector<int> c5(10,5);


    //添加和删除元素
    c.push_back(5);
    c.insert(c.end(),6);
    c.insert(c.end(),c4.begin(),c4.end());


    print_vector_int(c);
    print_vector_int(c2);
    print_vector_int(c3);
    print_vector_int(c4);
    print_vector_int(c5);

    return 0;
}