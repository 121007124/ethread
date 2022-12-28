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
        ready,          // ����, δ����
        working,        // ���ڹ���
        createing,      // ���ڴ���
        destroying,     // ��������
        suspending,     // �̳߳��Ѿ�����
        
    };

    enum POOL_DESTROY_MODE
    {
        POOL_DESTROY_MODE_WAIT,
        POOL_DESTROY_MODE_DETACH,       // ���߳���������
        POOL_DESTROY_MODE_SUSPEND,      // �����߳�, ��Զ������
        POOL_DESTROY_MODE_TERMINATE,    // ǿ�ƽ����߳�, ���Ƽ�
    };
    
    using Task = std::function<void()>;    // ��������
    
    std::vector<std::thread>    workers;        // �̳߳��߳��б�
    std::queue<Task>            tasks;          // �������
    std::mutex                  queue_mutex;    // ���еĻ�����
    std::mutex                  workers_mutex;  // �̳߳��б�����
    std::condition_variable     condition;      // ��������
    std::atomic<size_t>         idlThrNum;      // �����߳�����
    std::atomic<POOL_ENUM>      state;          // �̳߳�״̬, POOL_ENUM ���ö���������ֵ
    //std::atomic<size_t>         queue_count;    // ��������

public:
    typedef struct THREAD_POOL_INFO
    {
        size_t Vacant;          // �����߳���
        size_t ExecuteCount;    // ִ���߳���
        size_t QueueCount;      // ����������
        size_t size;            // �̳߳�����
        size_t MaxSize;         // �̳߳��������
        BOOL   IsVacant;        // �Ƿ����
        size_t State;           // ״̬
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

    // �����̳߳�, ���֧��0x7fff ���߳�, �ڲ�ǿ������, �������Ҳ��������ô���߳�, ����ô���̻߳�Ӱ��Ч��
    inline bool create(int count = 5, int maxCount = 10)
    {
        if ( state != POOL_ENUM::ready )
            return false;   // ���Ǿ���״̬�ͱ�ʾ�Ѿ���������

        // ����һ���߳���������߳�����ֵ, ����߳������ܴ���0x7fff, Ҳ����С���߳���
        if ( count <= 0 || count > 0x7fff )
            count = 5;
        if ( maxCount > 0x7fff )
            maxCount = 0x7fff;
        if ( maxCount < count )
            maxCount = count;

        // ��ʼ���߳�����Ļ�����, ��������̲߳��ᳬ��������ֵ
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
        state = POOL_ENUM::destroying;  // ��������


        condition.notify_all(); // ����ȫ���߳�
        // ���������߳�, ��������ģʽ����
        for ( std::thread& thread : workers )
        {
            switch ( mode )
            {
            case ThreadPool::POOL_DESTROY_MODE_DETACH:
            {
                // ���̺߳�����������, ���߳���������
                thread.detach();
                break;
            }
            case ThreadPool::POOL_DESTROY_MODE_SUSPEND:
            {
                // �����߳�, ���߳���Զ������
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
            //    // ǿ�ƽ����߳�, ���Ƽ�
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
                // Ĭ�ϰ��ȴ�����������˳�
                if ( thread.joinable() )
                    thread.join();  // �ȴ��������, ǰ��: �߳�һ����ִ����, ����߳�һֱ������, �ǻ�һֱ��ס
                break;
            }
            }
        }

        // ����, Ȼ�����������к��߳�����
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };
        std::lock_guard<std::mutex> lock_queue{ queue_mutex };
        while ( tasks.size() )
            tasks.pop();
        workers.clear();
        state = POOL_ENUM::ready;  // ״̬����Ϊ����, ���Խ�����һ�εĳ�ʼ������
        return true;
    }

    // �ύһ������, ��������߳�Ϊ0, �����̳߳ػ��������߳�, �Ǿ�������һ���̴߳�ŵ��̳߳���
    // ����.get()��ȡ����ֵ��ȴ�����ִ����,��ȡ����ֵ
    // �����ַ�������ʵ�ֵ������Ա��
    // һ����ʹ��   bind�� .commit(std::bind(&Dog::sayHello, &dog));
    // һ������   mem_fn�� .commit(std::mem_fn(&Dog::sayHello), this)
    template<class F, class... Args>
    auto commit(F&& f, Args&&... args) ->std::future<decltype( f(args...) )>
    {
        // ������ǹ�����, �Ǿ��׳����쳣
        if ( state != POOL_ENUM::createing && state != POOL_ENUM::working )
            throw std::runtime_error("�̳߳�δ����");
        
        // ��������߳�Ϊ0, �����̳߳ػ��������߳�, �Ǿ�������һ���߳�
        if ( GetVacant() == 0 && workers.size() < workers.capacity() )
            addThread(1);


        // using return_type = typename std::result_of<F(Args...)>::type;
        // typename std::result_of<F(Args...)>::type, ���� f �ķ���ֵ����
        using RetType = decltype( f(args...) );

        // �Ѻ�����ڼ�����,���(��)
        auto task = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

        std::future<RetType> future = task->get_future();

        {
            // ������񵽶���
            std::lock_guard<std::mutex> lock{ queue_mutex };
            tasks.emplace([task]() { ( *task )( ); });
        }

        condition.notify_one(); // ����һ���߳�ִ��
        return future;
    }

    // �����̳߳�
    inline bool suspend()
    {
        if ( state != POOL_ENUM::working )
            return false;
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };
        state = POOL_ENUM::suspending;
        for ( std::thread& thread : workers )
        {
            // ���̳߳������̹߳���
            if ( thread.joinable() )
            {
                HANDLE hThread = thread.native_handle();
                if ( hThread )
                    SuspendThread(hThread);
            }
        }
        return true;
    }
    
    // �ָ��̳߳�
    inline bool resume()
    {
        if ( state != POOL_ENUM::suspending )
            return false;
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };
        state = POOL_ENUM::working;
        for ( std::thread& thread : workers )
        {
            // ���̳߳������ָ̻߳�
            if ( thread.joinable() )
            {
                HANDLE hThread = thread.native_handle();
                if ( hThread )
                    ResumeThread(hThread);
            }
        }
        return true;
    }
    
    // ��ȡ�̳߳���Ϣ
    inline bool GetInfo(PTHREAD_POOL_INFO info)
    {
        // ������ǹ���״̬�ͷ���
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

    // ȡ�����߳���
    inline size_t GetVacant()
    {
        return idlThrNum;
    }
    // ȡִ���߳���
    inline size_t GetExecuteCount()
    {
        // ִ���������߳��� - ������
        return size() - idlThrNum;
    }
    // ȡ����������, �ȴ�ִ�е���������
    inline size_t GetQueueCount()
    {
        std::lock_guard<std::mutex> lock{ queue_mutex };
        return tasks.size();
    }
    // ȡ�̳߳�����
    inline size_t size()
    {
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };
        return workers.size();
    }
    // ȡ�̳߳��������
    inline size_t maxSize()
    {
        return workers.capacity();
    }
    // �Ƿ����
    inline bool IsVacant()
    {
        // ִ��������0, ����������е���0, �Ǿ��ǿ���״̬
        return GetQueueCount() == 0 && GetExecuteCount() == 0;
    }
    // ȡ״̬, 0=����, 1=���ڹ���, 2=���ڴ���, 3=��������
    inline int GetState()
    {
        return (int)(POOL_ENUM)state;
    }

private:
    // ���ָ���������߳�
    void addThread(int size)
    {
        // �����߳�����, ������������߳���
        std::lock_guard<std::mutex> lock_workers{ workers_mutex };
        size_t maxCount = workers.capacity();
        for ( ; workers.size() < maxCount && size > 0; --size )
        {
            // �¼ӵ��߳�ֱ�ӷŵ��������
            workers.emplace_back([this]
                                 {
                                     // �����̺߳���, ֻ���Ǵ�����, ���߹����в�ѭ������߳�
                                     while ( state == POOL_ENUM::createing || state == POOL_ENUM::working )
                                     {
                                         Task task; // ��ȡһ����ִ�е� task

                                         // Ūһ��������, �����������������, Ȼ�����������������ȡ����
                                         // �˳���������Զ���������, Ȼ��ִ������, ��������-1
                                         {
                                             // unique_lock ��� lock_guard �ĺô��ǣ�������ʱ unlock() �� lock()
                                             std::unique_lock<std::mutex> lock{ queue_mutex };
                                             condition.wait(lock,
                                                            [this]
                                                            {
                                                                // ����true���˳��ȴ�
                                                                // ״̬���ǹ������ߴ�����, ����������в�Ϊ��
                                                                return state != POOL_ENUM::createing && state != POOL_ENUM::working || !tasks.empty();
                                                            });    // �ȴ�, ֱ��������

                                             // �ߵ����������ʲô�ط��������߳�, �ж��Ƿ��������ǻ��ѵ��߳�
                                             // �����ٻ��ѵ��߳�, ��״̬����������, ��Ҫ����, ������ִ������߳�
                                             if ( state == POOL_ENUM::destroying || state == POOL_ENUM::ready )
                                                 return;

                                             // ���Ƚ��ȳ��Ӷ���ȡһ������, Ȼ������񵯳�����
                                             task = move(tasks.front());
                                             tasks.pop();
                                         }
                                         idlThrNum--;    // ��������-1
                                         task();         // ִ������
                                         idlThrNum++;    // ִ����������������+1

                                         // ���˳�ѭ��, ������һ�ֵȴ��߳�����
                                     }
                                 });
            
            idlThrNum++;    // ÿ����һ���߳�����, �����߳̾�+1
        }
    }
};


#endif // !_THREAD_POOL_H_