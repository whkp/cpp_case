#pragma once
#include <mutex>
#include <queue>
#include <thread>
#include <memory>
#include <condition_variable>
#include <functional>
#include <vector>
#include <future>


//池式结构解决缓存问题，起到缓冲区作用

//线程池可以有效降低多线程任务中任务申请和释放产生的性能消耗；
//当我们每个线程任务处理较快时，系统大部分性能消耗在线程创建和释放过程中；
//线程池就是在程序运行之前创建好一堆线程，需要使用就去拿一个线程，用完再放回去，程序结束后就统一释放线程
//线程池中关键： 工作队列、任务队列、池、回调函数
class Threadpool {
private:
    std::vector<std::thread> workers; //线程池
    int maxSize; //线程池最大线程数
    int freeSize; //线程池空闲线程数
    int busySize; //线程池忙线程数
    std::queue<std::function<void()>> tasks;  //任务队列
    std::mutex mtx;  //互斥锁
    std::condition_variable cv;  //条件变量
    bool isRunning;  //线程池是否运行

public:
    explicit Threadpool(unsigned int size = std::thread::hardware_concurrency()); //构造函数
    ~Threadpool(); //析构函数
    //F为函数类型，Args为参数类型，返回类型为std::future，用于获取任务执行结果
    template<class F, class... Args>
    auto addTask(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>; //添加任务
};


template<class F, class... Args>
auto Threadpool::addTask(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using returnType = typename std::result_of<F(Args...)>::type;
    //定义别名，表示函数返回值类型
    auto task = std::make_shared<std::packaged_task<returnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<returnType> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (!isRunning) {
            throw std::runtime_error("Threadpool is stopped!");
        }
        tasks.emplace([task]() {(*task)();});
    }
    cv.notify_one(); //唤醒一个线程
    return res;
}

Threadpool::Threadpool(unsigned int size) : maxSize(size), freeSize(size), busySize(0), isRunning(true) {
    for (unsigned int i = 0; i < size; ++i) {
        //循环创建线程，并将线程放入线程池中
        workers.emplace_back(std::thread([this] {
            while(true) {
                std::function<void()> task;
                {
                    //创建唯一锁，保护任务队列
                    std::unique_lock<std::mutex> lock(mtx);
                    //等待条件变量，当任务队列不为空时，唤醒线程
                    cv.wait(lock, [this]() {return isRunning && !tasks.empty();});
                    //如果线程池关闭且任务队列为空，则线程退出
                    if (!isRunning && tasks.empty()) {
                        return;
                    } 
                    //从任务队列取出任务
                    task = tasks.front();
                    tasks.pop();
                }
                task(); //执行任务
            }
        }));
    }
}

Threadpool::~Threadpool() {
    {
        //创建唯一锁，锁定互斥量，确保线程安全
        std::unique_lock<std::mutex> lock(mtx);
        isRunning = false;
    }
    cv.notify_all(); //唤醒所有线程
    for (std::thread &worker : workers) {
        //遍历所有线程，等待线程结束
        if(worker.joinable()) {
            worker.join(); //等待线程结束
        }
    }
}
