# c++编译
## 编译器使用

编译器：是将源代码生成机器码的程序

流程：

- 预处理：对源程序中伪指令（#开头的   宏定义、条件编译、头文件包含）和特殊符号进行处理
- 编译：预处理后的文件编译成后缀为s的汇编语言文件，要做工作为语法分析和记法分析。
- 汇编：将汇编文件汇编为后缀为.o的目标文件（二进制文件）。汇编过程实际是把汇编语言翻译为目标机器指令的过程。
- 链接：将多个目标文件和库连接生成.out和.exe的可执行文件。
  - 静态编译：编译器链接时将库放到可执行程序中，多个程序用到同一静态库的函数或类，存在多份拷贝
  - 动态编译：程序运行时才将库（动态链接库）连接到程序中，多个进程用到了同一个函数或类，内存中只有一份

c语言用gcc编译，cpp用g++编译

当文件；越来越多时，一个个调用g++编译链会变得繁琐，于是使用**make**只需要写出不同文件的依赖关系和生成各文件的规则

**好处：**更新了一个依赖的cpp文件，只会对其进行重新编译，不会对主程序重新编译；

自动并行，加快编译速度

用通配符批量生成构建规则，避免对每个文件重复写命令。

**坏处：**通用性差，无法对各个系统适配；

语法简单无法逻辑判断



Cmake为了解决make的问题，可以跨平台，可以自动检测源文件和头文件之间的依赖来生成makefile