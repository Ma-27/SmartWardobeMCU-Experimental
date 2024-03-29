/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/12 下午 07:08
 */

// 防止头文件重复包含
#ifndef TaskScheduler_h
#define TaskScheduler_h

#include <Arduino.h>

#undef min
#undef max

#include "functional"

class TaskScheduler {
public:
    // 定义任务回调函数的类型
    // 使用std::function而不是原始的函数指针
    using TaskCallback = std::function<void(void)>;

    // 获取单例对象的引用
    static TaskScheduler &getInstance();

    // 添加任务函数（传入函数名）
    void addTask(TaskCallback callback, unsigned long interval);

    // 删除任务函数（传入任务id）
    void deleteTask(int id);

    // 每次的任务调度
    void run();

private:
    // 禁止复制和赋值
    TaskScheduler(const TaskScheduler &) = delete;

    TaskScheduler &operator=(const TaskScheduler &) = delete;

    // 单例模式的自身实例
    static TaskScheduler *instance;

    struct Task {
        // 任务的标识符
        int id;
        // 回调函数（地址）
        TaskCallback callback;
        // 规定的执行的时间间隔
        unsigned long interval;
        // 上次运行的时刻
        unsigned long lastRun;
    };

    Task *tasks;
    unsigned int capacity;
    unsigned int taskCount;

    // 为每个任务分配一个独有的ID。
    int generateUniqueId();

    // 私有构造函数
    TaskScheduler(unsigned int capacity);

    // 私有析构函数
    ~TaskScheduler();
};

#endif



