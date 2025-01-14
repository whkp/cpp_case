# Web server概念

Web Server就是一个服务器软件，或者是一个运行服务器软件的硬件。

主要功能是通过HTTP协议与客户端进行通讯，来接受、存储、处理来自客户端的HTTP请求并作出响应，返回给客户端请求的内容或返回一个Error信息。

HTTP请求通常为用户使用web浏览器与相应服务器进行通信，浏览器中输入域名或IP地址：端口号，浏览器将域名解析为ip地址或直接根据IP地址向对应Web服务器发送一个HTTP请求。

## 主函数逻辑

命令行解析 —— 初始化 ——初始化日志 ——初始化数据库 —— 初始化线程池 —— 设置触发模式 ——设置并监听网络事件 —— 事件循环处理客户端连接和I/O事件

```c++
int main(int argc, char *argv[])
{
    //需要修改的数据库信息,登录名,密码,库名
    string user = "root";
    string passwd = "root";
    string databasename = "qgydb";`
    //命令行解析
    Config config;
    config.parse_arg(argc, argv);
    WebServer server;
    //初始化
    server.init(config.PORT, user, passwd, databasename, config.LOGWrite, 
                config.OPT_LINGER, config.TRIGMode,  config.sql_num,  config.thread_num, 
                config.close_log, config.actor_model);
    //日志
    server.log_write();
    //数据库
    server.sql_pool();
    //线程池
    server.thread_pool();
    //触发模式
    server.trig_mode();
    //监听
    server.eventListen();
    //运行
    server.eventLoop();
    return 0;
}
```



# Web服务器如何接受客户端发来的HTTP请求报文呢？

web服务器通过socket监听来自用户的请求。

```c++
//网络编程基础步骤
    m_listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(m_listenfd >= 0);

    //优雅关闭连接
    if (0 == m_OPT_LINGER)
    {
        struct linger tmp = {0, 1};
        setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
    }
    else if (1 == m_OPT_LINGER)
    {
        struct linger tmp = {1, 1};
        setsockopt(m_listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
    }

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(m_port);

    int flag = 1;
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ret = bind(m_listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret >= 0);
    ret = listen(m_listenfd, 5);
    assert(ret >= 0);
```

远程很多用户都尝试connect()这个Web Server上正在`listen`的这个`port`，而监听到的这些连接会排队等待被`accept()`。用户连接请求是随即到达的异步事件，每当监听socket监听到新的客户端连接并放入监听队列，我们都需要告诉我们的服务器连接来了，accept这个连接，并分配一个逻辑单元来处理这个用户请求。在处理这个请求同时也需要继续监听其他客户的请求并分配另一逻辑单元来处理。

**使用epoll这种I/O复用技术来实现对监听socket和连接socket的同时监听**。但是虽然可以同时监听，但是本身是阻塞的，当有多个文件描述符同时就绪时，如果不采取额外措施，程序只能按顺序处理每一个文件描述符。

**提高效率这部分通过线程池来实现并发，为每个描述符来分配一个逻辑单元（线程)来处理**

## 提升服务器性能：池

​	服务器一般硬件资源“充裕”，提高服务器性能一个很直接的方式就是以**空间换时间**，用服务器硬件资源换取运行效率。

​	池是一组资源的集合，这组资源在服务器启动之初就被**完全创建并初始化**，当服务器进入正式运行阶段，即开始处理客户请求时，如果有它需要的相关资源，可以直接从池中获取，直接从池中获取资源速度比动态分配资源速度快很多。处理完资源后再把相关资源放回池中，无需执行系统调用释放资源。

​	池可以预先分配足够多的资源或者预先分配一定的资源，资源不够用再动态分配一些加入池子。

​	通常池子有内存池、进程池、线程池和连接池。

​	内存池通常用于socket的接受缓存和发送缓存。

​	进程池和线程池是并发编程的常用伎俩。需要工作进程或线程来处理新到来的客户请求时，可以直接从进程池或者线程池取得一个执行实体，无需调用fork或pthread_create等函数来创建进程和线程。

***在tinywebserver中，使用线程池并发处理用户请求，主线程负责读写，工作线程（线程池中的线程）负责处理逻辑（HTTP请求报文的解析等等）***。  

- 所谓线程池，就是一个pthread_t类型的普通数组，通过pthread_create()函数创建m_thread_number个线程，用来执行worker()函数以执行每个请求处理函数（HTTP请求的process函数），通过pthread_detach()将线程设置成脱离态（detached）后，当这一线程运行结束时，它的资源会被系统自动回收，而不再需要在其它线程中对其进行 pthread_join() 操作。

- 操作工作队列一定要加锁（locker），因为它被所有线程共享。

- 我们用信号量来标识请求队列中的请求数，通过m_queuestat.wait();来等待一个请求队列中待处理的HTTP请求，然后交给线程池中的空闲线程来处理。

  线程池中线程数量根据什么确定：**最直接的限制因素是CPU的数量N**；对于CPU密集型任务（消耗CPU资源较多）线程池中线程数量设置为CPU核数；IO密集型任务，一般要多于CPU核数，因为线程竞争的是IO，IO处理较慢，多的线程为CPU争取更多的任务。 

## 数据复制

高性能服务器应避免不必要的数据复制，尤其是数据复制发生在用户代码和内核之间时。用户代码内部的数据复制应该避免。两个工作进程之间需要传递大量的数据时，应该考虑使用**共享内存**在他们之间来共享这些数据，而不是使用管道或者消息队列。

## 上下文切换和锁

并发编程必须考虑上下文切换的问题，即进程或线程切换导致的系统开销，不应使用过多的工作线程，否则线程切换将占用大量时间。**因此不应该为每个客户端连接都创建一个工作线程。**

多线程服务器的优点是不同线程可以同时运行在不同的CPU上，线程数量不大于CPU数量时，上下文切换就不是问题了。

## 服务器通常处理下面三类事件：

 **I/O事件、信号以及定时事件**

两种事件处理模式：

**Reactor模式**：有一个Reactor的中心部件负责监听与分发事件，若有事件发生立即通知工作线程，将socket可读可写事件放入请求队列中，交给工作线程来处理。

**Proactor模式**：将I/O操作（读写）的完成事件作为主要的事件来处理，不是像Reactor模式那样处理I/O操作的就绪事件。

通常使用同步I/O实现Reactor，异步I/O实现Proactor。

**同步I/O：**一个线程中，CPU执行代码速度够快，一旦遇上IO操作，如读写文件、发送网络数据时需要等待IO操作完成才能进行下一步。

**异步I/O：**代码执行一个耗时的IO操作时，只发出IO指令，并不等待IO结果就去执行其他代码了，一段时间后当IO返回结果时在通知CPU处理。

# 异步日志

默认情况下，日志消息是默认打印到终端的，实际应用中，一般会写到本地文件。

最简单方式是每产生一条日志消息，就写到相应的文件中，如果很多线程某个时间输出大量日志，日志输出效率较低，**效率低的原因是每条日志消息都需要通过write这类函数输出到本地磁盘，这将频繁调用IO函数，磁盘操作较为耗时，这样后面的代码都会阻塞住，直到前一条日志写出成功**

优化的方式是：**日志积累到一定量再写到磁盘中，可以减少IO操作次数，提高效率**

需要考虑一点是，如果日志消息产生较慢，很长一段时间达不到阈值就会出现日志消息一直无法写出到磁盘，所以还应该设置一个超时值如3s，即每过3s不管是否达到阈值都将消息写到磁盘中。

## 实现细节

采用循环数组实现阻塞队列，用到了互斥锁和条件变量。

阻塞队列的push和pop实现：

**队列为空时，消费者不能继续获取元素；**

**队列中添加了新元素，会通知等待的消费者线程**

```c++
    bool push(const T &item)
    {

        m_mutex.lock();
        if (m_size >= m_max_size)
        {

            m_cond.broadcast();
            m_mutex.unlock();
            return false;
        }

        m_back = (m_back + 1) % m_max_size;
        m_array[m_back] = item;

        m_size++;

        m_cond.broadcast();
        m_mutex.unlock();
        return true;
    }
    //pop时,如果当前队列没有元素,将会等待条件变量
    //队列为空时，会阻塞，直到被生产者线程唤醒
    bool pop(T &item)
    {

        m_mutex.lock();
        while (m_size <= 0)
        {
            
            if (!m_cond.wait(m_mutex.get()))
            {
                m_mutex.unlock();
                return false;
            }
        }

        m_front = (m_front + 1) % m_max_size;
        item = m_array[m_front];
        m_size--;
        m_mutex.unlock();
        return true;
    }

    //增加了超时处理
    bool pop(T &item, int ms_timeout)
    {
        struct timespec t = {0, 0};
        struct timeval now = {0, 0};
        gettimeofday(&now, NULL);
        m_mutex.lock();
        if (m_size <= 0)
        {
            t.tv_sec = now.tv_sec + ms_timeout / 1000;
            t.tv_nsec = (ms_timeout % 1000) * 1000;
            if (!m_cond.timewait(m_mutex.get(), t))
            {
                m_mutex.unlock();
                return false;
            }
        }

        if (m_size <= 0)
        {
            m_mutex.unlock();
            return false;
        }

        m_front = (m_front + 1) % m_max_size;
        item = m_array[m_front];
        m_size--;
        m_mutex.unlock();
        return true;
    }
```

条件变量：利用线程间的共享全局变量来实现同步的一种机制，包括了两个动作：一个线程等待条件变量的条件成立而挂起；另一个线程使条件成立而发出信号。

条件变量一般和互斥锁结合使用，改变条件前首先锁住互斥量，线程挂起进入等待前解锁，条件满足后重新加锁

如果没有条件变量，消费者线程在发现队列为空时，为了等待新数据可能会陷入如下问题：

- 不断地循环检查队列状态（忙等），占用 CPU 资源；
- 或者需要自己实现复杂的“等待-唤醒”机制，条件变量则简化了这一过程。

***条件变量就是让“等不到数据的消费者线程”先睡一会儿，等“生产者线程来了”再把它们唤醒***