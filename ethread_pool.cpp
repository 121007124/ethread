#include "include_ethread_header.h"
#include "CThreadPool.h"



inline ThreadPool* pool_getobj(MDATA_INF& pArgInf, USHORT count = 0, USHORT maxCount = 0)
{
    if ( !pArgInf.m_ppCompoundData )
        return 0;
    ThreadPool* pool = (ThreadPool*)pArgInf.m_ppCompoundData[0];
    if ( !pool && count > 0 )
    {
        pool = new ThreadPool{ count, maxCount };
        pArgInf.m_ppCompoundData[0] = pool;
    }
    return pool;
}

// 调用格式: _SDT_NULL 线程池构造函数, 命令说明: "线程池构造函数"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPoolConsturct_8_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    pArgInf[0].m_ppCompoundData[0] = 0;
}

// 调用格式: _SDT_NULL 线程池复制构造函数, 命令说明: "线程池复制构造函数"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPoolCopy_9_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    // 这个是有一个参数, 参数就是等号右边的数据, 这里直接创建一个新的线程池给等号左边的对象

    ThreadPool* poolLeft = pool_getobj(pArgInf[0], 0);    // 等号左边的值
    ThreadPool* poolRight = pool_getobj(pArgInf[1], 0);    // 等号右边的值
    
    // 这里直接新建一个线程池放等号左边, 一般情况下线程池是不支持复制构造, 这里看自己发挥
    // 或者可以改成引用计数的方式, 让等号两边的线程池都是同一个对象
    if ( !poolLeft )
    {
        // 等号左边没有的才创建
        poolLeft = new ThreadPool;
        pArgInf[0].m_ppCompoundData[0] = poolLeft;
    }
}

// 调用格式: _SDT_NULL 线程池析构函数, 命令说明: "线程池析构函数"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPoolFree_10_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    auto pool = pool_getobj(pArgInf[0], 0);
    if ( pool )
    {
        delete pool;
        pArgInf[0].m_ppCompoundData[0] = 0;
    }
}



// 调用格式: SDT_BOOL 初始化, 命令说明: "初始化线程池, 返回是否初始化成功"
// 参数<1>: 最大线程数 SDT_INT, 参数说明: "线程池最大线程数, 小于等于0则默认为5"
ETHREAD_EXTERN_C void ethread_ThreadPool_Init_11_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    int count = pArgInf[1].m_int;
    int maxCount = pArgInf[2].m_int;
    if ( count <= 0 )
        count = 5;
    auto pool = pool_getobj(pArgInf[0], count, maxCount);
    pRetData->m_bool = pool != 0;
}

// 调用格式: SDT_BOOL 添加任务, 命令说明: "添加线程任务, 回调函数有多少个参数就填写多少个参数, 返回石头添加成功"
// 参数<1>: 欲执行的子程序 _SDT_ALL, 参数说明: "本参数提供创建线程时欲启动的子程序, 只支持传递整数型的子程序地址或者子程序指针类型, 其他类型直接调用失败"
// 参数<2>: 线程参数 SDT_INT, 参数说明: "传递到线程里的参数"
// 变长参数....
ETHREAD_EXTERN_C void ethread_ThreadPool_AddTask_12_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    if ( pArgInf[1].m_dtDataType != SDT_INT && pArgInf[1].m_dtDataType != SDT_SUB_PTR )
        return;
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;

    PTHREAD_ARG_STRUCT args = _thread_GetArgs(nArgCount, pArgInf, 2, (LPVOID)pArgInf[1].m_dwSubCodeAdr);
    if ( !args ) return;

    pool->commit([](PTHREAD_ARG_STRUCT args)
                {
                    int r = _threadpool_call(args->pfn, args->arr, args->count);
                    if ( args->arr )
                        delete[] args->arr;
                    delete args;
                    return r;
                }, args);
    pRetData->m_bool = true;
}


// 调用格式: SDT_BOOL (线程池).销毁, 命令说明: "销毁线程池, 根据传递的参数决定如何销毁, 默认是等待所有任务结束"
// 参数<1>: [销毁模式 SDT_INT], 参数说明: "1=让线程自生自灭, 其他=等待线程结束"
ETHREAD_EXTERN_C void ethread_ThreadPool_Destroy_13_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[1].m_int;
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_bool = pool->destroy(arg1);
}

// 调用格式: SDT_INT (线程池).取空闲线程数, 命令说明: "取当前空闲的线程数"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_GetVacant_14_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetVacant();
}

// 调用格式: SDT_INT (线程池).取执行线程数, 命令说明: "取当前执行的线程数"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_GetExecuteCount_15_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetExecuteCount();
}

// 调用格式: SDT_INT (线程池).取队列任务数, 命令说明: "取等待执行的任务数量, 添加到队列里还没被执行的任务"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_GetQueueCount_16_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetQueueCount();
}

// 调用格式: SDT_INT (线程池).取线程池容量, 命令说明: "取当前线程池创建的线程数量"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_size_17_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->size();
}

// 调用格式: SDT_INT (线程池).取线程池最大容量, 命令说明: "取当前线程池创建时填写的最大线程数量"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_MaxSize_18_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->maxSize();
}

// 调用格式: SDT_INT (线程池).是否空闲, 命令说明: "取当前线程池是否空闲"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_IsVacant_19_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->IsVacant();
}

// 调用格式: SDT_INT (线程池).取状态, 命令说明: "取当前线程池状态, 0=就绪, 1=正在工作, 2=正在创建, 3=正在销毁"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_GetState_20_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetState();
}
