#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <vector>
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>


class ThreadPool
{
private:

    enum class POOL_ENUM
    {
        ready,          // 就绪, 未启动
        working,        // 正在工作
        createing,      // 正在创建
        destroying,     // 正在销毁
        suspending,     // 线程池已经挂起
        
    };

    enum POOL_DESTROY_MODE
    {
        POOL_DESTROY_MODE_WAIT,
        POOL_DESTROY_MODE_DETACH,       // 让线程自生自灭
        POOL_DESTROY_MODE_SUSPEND,      // 挂起线程, 永远不唤醒
        POOL_DESTROY_MODE_TERMINATE,    // 强制结束线程, 不推荐
    };
    
    using Task = std::function<void()>;    // 定义类型
    
    std::vector<std::thread>    workers;        // 线程池线程列表
    std::queue<Task>            tasks;          // 任务队列
    std::mutex                  queue_mutex;    // 队列的互斥锁
    std::mutex                  workers_mutex;  // 线程池列表互斥锁
    std::condition_variable     condition;      // 条件阻塞
    std::atomic<size_t>         idlThrNum;      // 空闲线程数量
    std::atomic<POOL_ENUM>      state;          // 线程池状态, POOL_ENUM 这个枚举类型里的值
    //std::atomic<size_t>         queue_count;    // 任务数量

public:
    typedef struct THREAD_POOL_INFO
    {
        size_t Vacant;          // 空闲线程数
        size_t ExecuteCount;    // 执行线程数
        size_t QueueCount;      // 队列任务数
        size_t size;            // 线程池容量
        size_t MaxSize;         // 线程池最大容量
        BOOL   IsVacant;        // 是否空闲
        size_t State;           // 状态
    }*PTHREAD_POOL_INFO;
    
public:
    ThreadPool()
    {

    }
    ThreadPool(int count, int maxCount)
    {
        create(count, maxCount);
    }
    ~ThreadPool()
    {
        destroy(POOL_DESTROY_MODE_WAIT);
    }

    // 创建线程池, 最大支持0x7fff 个线程, 内部强制限制, 正常情况也不会用这么多线程, 用这么多线程会影响效率
    inline bool create(int count = 5, int maxCount = 10)
    {
        if ( state != POOL_ENUM::ready )
            return false;   // 不是就绪状态就表示已经创建过了

        // 处理一下线程数和最大线程数的值, 最大线程数不能大于0x7fff, 也不能小于线程数
        if ( count <= 0 || count > 0x7fff )
            count = 5;
        if ( maxCount > 0x7fff )
            maxCount = 0x7fff;
        if ( maxCount < count )
            maxCount = count;

        // 初始化线程数组的缓冲区, 最大加入的线程不会超过这个最大值
        workers.reserve(maxCount);
        state = POOL_ENUM::createing;
        addThread(count);
        state = POOL_ENUM::working;
        return true;
    }

    inline bool destroy(int mode)
    {
        if ( state != POOL_ENUM::working )
            return false;
        state = POOL_ENUM::destroying;  // 正在销毁


        condition.notify_all(); // 唤醒全部线程
        // 遍历所有线程, 根据销毁模式处理
        for ( std::thread& thread : workers )
        {
            switch ( mode )
            {
            case ThreadPool::POOL_DESTROY_MODE_DETACH:
            {
                // 把线程和这个对象分离, 让线程自生自灭
                thread.detach();
                break;
            }
            case ThreadPool::POOL_DESTROY_MODE_SUSPEND:
            {
                // 挂起线程, 让线程永远不唤醒
                if ( thread.joinable() )
                {
                    HANDLE hThread = thread.native_handle();
                    if ( hThread )
                        SuspendThread(hThread);
                    thread.detach();
                }
                break;
            }
            //case ThreadPool::POOL_DESTROY_MODE_TERMINATE:
            //{
            //    // 强制结束线程, 不推荐
            //    if ( thread.joinable() )
            //    {
            //        HANDLE hThread = thread.native_handle();
            //        if ( hThread )
            //            TerminateThread(hThread, 0);
            //        thread.detach();
            //    }
            //    break;
            //}
            default:
            {
                // 默认按等待任务结束来退出
                if ( thread.joinable() )
                    thread.join();  // 等待任务结束, 前提: 线程一定会执行完, 如果线程一直不返回, 那会一直卡住
                break;
            }
            }
        }

        // 加锁, 然后清除任务队列和线程数组
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };
        std::lock_guard<std::mutex> lock_queue{ queue_mutex };
        while ( tasks.size() )
            tasks.pop();
        workers.clear();
        state = POOL_ENUM::ready;  // 状态设置为就绪, 可以进行下一次的初始化操作
        return true;
    }

    // 提交一个任务, 如果空闲线程为0, 并且线程池还能容纳线程, 那就新增加一条线程存放到线程池中
    // 调用.get()获取返回值会等待任务执行完,获取返回值
    // 有两种方法可以实现调用类成员，
    // 一种是使用   bind： .commit(std::bind(&Dog::sayHello, &dog));
    // 一种是用   mem_fn： .commit(std::mem_fn(&Dog::sayHello), this)
    template<class F, class... Args>
    auto commit(F&& f, Args&&... args) ->std::future<decltype( f(args...) )>
    {
        // 如果不是工作中, 那就抛出个异常
        if ( state != POOL_ENUM::createing && state != POOL_ENUM::working )
            throw std::runtime_error("线程池未启动");
        
        // 如果空闲线程为0, 并且线程池还能容纳线程, 那就新增加一条线程
        if ( GetVacant() == 0 && workers.size() < workers.capacity() )
            addThread(1);


        // using return_type = typename std::result_of<F(Args...)>::type;
        // typename std::result_of<F(Args...)>::type, 函数 f 的返回值类型
        using RetType = decltype( f(args...) );

        // 把函数入口及参数,打包(绑定)
        auto task = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

        std::future<RetType> future = task->get_future();

        {
            // 添加任务到队列
            std::lock_guard<std::mutex> lock{ queue_mutex };
            tasks.emplace([task]() { ( *task )( ); });
        }

        condition.notify_one(); // 唤醒一个线程执行
        return future;
    }

    // 挂起线程池
    inline bool suspend()
    {
        if ( state != POOL_ENUM::working )
            return false;
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };
        state = POOL_ENUM::suspending;
        for ( std::thread& thread : workers )
        {
            // 把线程池所有线程挂起
            if ( thread.joinable() )
            {
                HANDLE hThread = thread.native_handle();
                if ( hThread )
                    SuspendThread(hThread);
            }
        }
        return true;
    }
    
    // 恢复线程池
    inline bool resume()
    {
        if ( state != POOL_ENUM::suspending )
            return false;
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };
        state = POOL_ENUM::working;
        for ( std::thread& thread : workers )
        {
            // 把线程池所有线程恢复
            if ( thread.joinable() )
            {
                HANDLE hThread = thread.native_handle();
                if ( hThread )
                    ResumeThread(hThread);
            }
        }
        return true;
    }
    
    // 获取线程池信息
    inline bool GetInfo(PTHREAD_POOL_INFO info)
    {
        // 如果不是工作状态就返回
        if ( !info || state != POOL_ENUM::working )
            return false;
        
        std::lock_guard<std::mutex> lock{ queue_mutex };
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };

        info->Vacant        = idlThrNum;
        info->size          = workers.size();
        info->QueueCount    = tasks.size();
        info->MaxSize       = workers.capacity();
        info->ExecuteCount  = info->size - idlThrNum;
        info->IsVacant      = info->QueueCount == 0 && info->ExecuteCount == 0;
        info->State         = (size_t)(POOL_ENUM)state;
        return true;
    }

    // 取空闲线程数
    inline size_t GetVacant()
    {
        return idlThrNum;
    }
    // 取执行线程数
    inline size_t GetExecuteCount()
    {
        // 执行数等于线程数 - 空闲数
        return size() - idlThrNum;
    }
    // 取队列任务数, 等待执行的任务数量
    inline size_t GetQueueCount()
    {
        std::lock_guard<std::mutex> lock{ queue_mutex };
        return tasks.size();
    }
    // 取线程池容量
    inline size_t size()
    {
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };
        return workers.size();
    }
    // 取线程池最大容量
    inline size_t maxSize()
    {
        return workers.capacity();
    }
    // 是否空闲
    inline bool IsVacant()
    {
        // 执行数等于0, 并且任务队列等于0, 那就是空闲状态
        return GetQueueCount() == 0 && GetExecuteCount() == 0;
    }
    // 取状态, 0=就绪, 1=正在工作, 2=正在创建, 3=正在销毁
    inline int GetState()
    {
        return (int)(POOL_ENUM)state;
    }

private:
    // 添加指定数量的线程
    void addThread(int size)
    {
        // 增加线程数量, 但不超过最大线程数
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };
        size_t maxCount = workers.capacity();
        for ( ; workers.size() < maxCount && size > 0; --size )
        {
            // 新加的线程直接放到数组最后
            workers.emplace_back([this]
                                 {
                                     // 工作线程函数, 只有是创建中, 或者工作中才循环这个线程
                                     while ( state == POOL_ENUM::createing || state == POOL_ENUM::working )
                                     {
                                         Task task; // 获取一个待执行的 task

                                         // 弄一个作用域, 在这个作用域里上锁, 然后把任务队列里的任务取出来
                                         // 退出作用域后自动析构解锁, 然后执行任务, 空闲数量-1
                                         {
                                             // unique_lock 相比 lock_guard 的好处是：可以随时 unlock() 和 lock()
                                             std::unique_lock<std::mutex> lock{ queue_mutex };
                                             condition.wait(lock,
                                                            [this]
                                                            {
                                                                // 返回true就退出等待
                                                                // 状态不是工作或者创建中, 或者任务队列不为空
                                                                return state != POOL_ENUM::createing && state != POOL_ENUM::working || !tasks.empty();
                                                            });    // 等待, 直到有任务

                                             // 走到这里就是有什么地方唤醒了线程, 判断是否是销毁那唤醒的线程
                                             // 是销毁唤醒的线程, 那状态就是销毁中, 需要返回, 不继续执行这个线程
                                             if ( state == POOL_ENUM::destroying || state == POOL_ENUM::ready )
                                                 return;

                                             // 按先进先出从队列取一个任务, 然后把任务弹出队列
                                             task = move(tasks.front());
                                             tasks.pop();
                                         }
                                         idlThrNum--;    // 空闲数量-1
                                         task();         // 执行任务
                                         idlThrNum++;    // 执行完任务后空闲数量+1

                                         // 不退出循环, 继续下一轮等待线程任务
                                     }
                                 });
            
            idlThrNum++;    // 每增加一个线程任务, 空闲线程就+1
        }
    }
};


#endif // !_THREAD_POOL_H_