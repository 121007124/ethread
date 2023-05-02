#include "include_ethread_header.h"
#include "CThreadPool.h"
#include "elib/fnshare.h"
//#include <unordered_set>

// 好像没必要.... 做这个还得加锁处理, 而且释放的时候也是程序结束的时候才释放, 这就没必要记录了
//static std::unordered_set<PTHREAD_ARG_STRUCT> m_set;    // 记录生成的无限参数地址, 用于释放

typedef void ( CALLBACK* PFN_Complete )( LPVOID, LPVOID, LPVOID );
class CMyThreadPool : public ThreadPool
{
public:
    PFN_Complete m_pfnComplete; // 任务完成回调
    CMyThreadPool() :ThreadPool() { }
    CMyThreadPool(int count, int maxCount, PFN_Complete pfnComplete) :
        ThreadPool(count, maxCount),
        m_pfnComplete(pfnComplete)
    {
        
    }
};

inline CMyThreadPool* pool_getobj(MDATA_INF& pArgInf, USHORT count = 0, USHORT maxCount = 0, PFN_Complete pfnComplete = 0)
{
    if ( !pArgInf.m_ppCompoundData )
        return 0;
    CMyThreadPool* pool = (CMyThreadPool*)pArgInf.m_ppCompoundData[0];
    if ( !pool && count > 0 )
    {
        pool = new CMyThreadPool{ count, maxCount , pfnComplete };
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

    CMyThreadPool* poolLeft = pool_getobj(pArgInf[0], 0);    // 等号左边的值
    CMyThreadPool* poolRight = pool_getobj(pArgInf[1], 0);    // 等号右边的值
    
    // 这里直接新建一个线程池放等号左边, 一般情况下线程池是不支持复制构造, 这里看自己发挥
    // 或者可以改成引用计数的方式, 让等号两边的线程池都是同一个对象
    if ( !poolLeft && poolRight )
    {
        // 等号左边没有的才创建
        int size = (int)poolRight->size();
        int maxSize = (int)poolRight->maxSize();
        poolLeft = new CMyThreadPool{ size, maxSize, poolRight->m_pfnComplete };
    }
    pArgInf[0].m_ppCompoundData[0] = poolLeft;
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
// 参数<1>: 最大线程数 SDT_INT, 参数说明: "线程池最小线程数, 小于等于0则默认为5, 创建的时候就分配的线程数"
// 参数<2>: 最大线程数 SDT_INT, 参数说明: "线程池最大线程数, 小于等于0则默认为10, 最大32767条线程, 一般用不到这么多线程, 如果用到这么多线程, 那应该考虑优化程序了"
// 参数<3>: 任务完成回调 _SDT_ALL, 参数说明: "投递的任务执行完毕后会调用这个回调函数, 回调函数原型  无返回值 回调(任务返回值, 投递任务填写的参数指针)"
ETHREAD_EXTERN_C void ethread_ThreadPool_Init_11_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    int count = pArgInf[1].m_int;
    int maxCount = pArgInf[2].m_int;
    PFN_Complete pfn = 0;
    if ( pArgInf[3].m_dtDataType == SDT_INT || pArgInf[3].m_dtDataType == SDT_SUB_PTR )
        pfn = (PFN_Complete)pArgInf[3].m_int;
    
    if ( count <= 0 )
        count = 5;
    auto pool = pool_getobj(pArgInf[0], count, maxCount, pfn);
    pRetData->m_bool = pool != 0;
}

// 调用格式: SDT_BOOL 添加任务, 命令说明: "添加线程任务, 回调函数有多少个参数就填写多少个参数, 返回是否添加成功"
// 参数<1>: 欲执行的子程序 _SDT_ALL, 参数说明: "本参数提供创建线程时欲启动的子程序, 只支持传递整数型的子程序地址或者子程序指针类型, 其他类型直接调用失败"
// 参数<2>: 线程参数 SDT_INT, 参数说明: "传递到线程里的参数"
// 变长参数....
ETHREAD_EXTERN_C void ethread_ThreadPool_AddTask_12_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    if ( pArgInf[1].m_dtDataType != SDT_INT && pArgInf[1].m_dtDataType != SDT_SUB_PTR )
        return;
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 5, 10);
    if ( !pool ) return;

    PTHREAD_ARG_STRUCT args = _thread_GetArgs(nArgCount, pArgInf, 2, (LPVOID)pArgInf[1].m_dwSubCodeAdr);
    if ( !args ) return;

    pool->commit([](PTHREAD_ARG_STRUCT args, PFN_Complete pfnComplete)
                {
                    int r = _threadpool_call(args->pfn, args->arr, args->count);
                    if ( pfnComplete )  // 调用任务完成回调
                    {
                        // 第三个参数保留, 提供给后续版本扩展使用
                        int arg[3] = { r, (int)args, (int)0 };
                        
                        // 也需要多一层call, 让他能平栈
                        _threadpool_call(pfnComplete, arg, sizeof(arg) / sizeof(arg[0]));
                    }
                    delete args;
                    return r;
                }, args, pool->m_pfnComplete);
    pRetData->m_bool = true;
    
}


// 调用格式: SDT_BOOL (线程池).销毁, 命令说明: "销毁线程池, 根据传递的参数决定如何销毁, 默认是等待所有任务结束"
// 参数<1>: [销毁模式 SDT_INT], 参数说明: "1=让线程自生自灭, 其他=等待线程结束"
ETHREAD_EXTERN_C void ethread_ThreadPool_Destroy_13_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[1].m_int;
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;

    pArgInf[0].m_ppCompoundData[0] = 0; // 把易语言记录的内存池句柄设置为0

    if ( arg1 == CMyThreadPool::POOL_DESTROY_MODE::POOL_DESTROY_MODE_DETACH )
    {
        // 如果是让线程自生自灭, 则不能马上delete pool, 不然任务结束后this指针已经被释放了, 内部再继续使用就崩溃了
        std::thread t([pool, arg1]()
                      {
                          // 直接等待线程池所有任务都执行完毕后才返回
                          pool->destroy(CMyThreadPool::POOL_DESTROY_MODE::POOL_DESTROY_MODE_WAIT);
                          delete pool;  // 释放对象
                      });
        t.detach();
        pRetData->m_bool = true;
        return;
    }

    if ( arg1 == CMyThreadPool::POOL_DESTROY_MODE::POOL_DESTROY_MODE_SUSPEND
        || arg1 == CMyThreadPool::POOL_DESTROY_MODE::POOL_DESTROY_MODE_TERMINATE )
    {
        //std::thread t([pool, arg1]()
        //              {
        //                  // 直接等待线程池所有任务都执行完毕后才返回
        //                  pool->destroy(CMyThreadPool::POOL_DESTROY_MODE::POOL_DESTROY_MODE_WAIT);
        //                  delete pool;  // 释放对象
        //              });
        //t.detach();
        pRetData->m_bool = pool->destroy(arg1);
        pRetData->m_bool = true;
        return;
    }

    // 走到这里就是要等待任务结束才返回
    pRetData->m_bool = pool->destroy(arg1);
    delete pool;
}

// 调用格式: SDT_INT (线程池).取空闲线程数, 命令说明: "取当前空闲的线程数"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_GetVacant_14_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetVacant();
}

// 调用格式: SDT_INT (线程池).取执行线程数, 命令说明: "取当前执行的线程数"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_GetExecuteCount_15_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetExecuteCount();
}

// 调用格式: SDT_INT (线程池).取队列任务数, 命令说明: "取等待执行的任务数量, 添加到队列里还没被执行的任务"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_GetQueueCount_16_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetQueueCount();
}

// 调用格式: SDT_INT (线程池).取线程池容量, 命令说明: "取当前线程池创建的线程数量"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_size_17_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->size();
}

// 调用格式: SDT_INT (线程池).取线程池最大容量, 命令说明: "取当前线程池创建时填写的最大线程数量"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_MaxSize_18_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->maxSize();
}

// 调用格式: SDT_INT (线程池).是否空闲, 命令说明: "取当前线程池是否空闲"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_IsVacant_19_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->IsVacant();
}

// 调用格式: SDT_INT (线程池).取状态, 命令说明: "取当前线程池状态, 0=就绪, 1=正在工作, 2=正在创建, 3=正在销毁"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_GetState_20_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetState();
}

// 调用格式: SDT_BOOL (线程池).挂起任务, 命令说明: "挂起线程池所有任务线程"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_Suspend_29_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->suspend();
}

// 调用格式: SDT_BOOL (线程池).唤醒任务, 命令说明: "唤醒线程池所有任务线程"
// 无参数
ETHREAD_EXTERN_C void ethread_ThreadPool_Resume_30_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->suspend();
}

// 调用格式: SDT_BOOL (线程池).取信息, 命令说明: "获取线程池基础信息"
// 参数<1>: 接收信息变量& SDT_TYPE_POOLINFO, 参数说明: "接收线程池信息的结构变量"
ETHREAD_EXTERN_C void ethread_ThreadPool_GetInfo_31_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    CMyThreadPool::PTHREAD_POOL_INFO info = *((CMyThreadPool::PTHREAD_POOL_INFO*)pArgInf[1].m_int);
    pRetData->m_int = pool->GetInfo(info);
    pRetData->m_bool = true;
}


// 调用格式: SDT_BOOL 取参数数据, 命令说明: "获取线程池投递任务的参数信息"
// 参数<0>: 参数指针 SDT_INT, 参数说明: "线程池初始化时填写的回调函数的第二个参数"
// 参数<1>: 参数& _SDT_ALL, 参数说明: "接收参数, 投递任务时填写什么参数这里就可以填写什么参数, 为空不获取, 不校验参数类型"
// 变长参数...
ETHREAD_EXTERN_C void ethread_GetArgument_32_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    PTHREAD_ARG_STRUCT pArg = (PTHREAD_ARG_STRUCT)pArgInf[0].m_int;
    if ( !pArg ) return;
    
    int index = 0;
    // 第0个参数是指针, 从第1个参数开始循环
    for ( int i = 1; i < nArgCount; i++ )
    {
        MDATA_INF& arg = pArgInf[i];
        DATA_TYPE type = arg.m_dtDataType & ~DT_IS_ARY;
        if ( type == _SDT_NULL )
            break;
        switch ( type )
        {
        case SDT_BYTE:          // 字节
        case SDT_SHORT:         // 短整数
        case SDT_INT:           // 整数
        case SDT_SUB_PTR:       // 子程序指针
        case SDT_FLOAT:         // 小数
        case SDT_BOOL:          // 逻辑
            arg.m_pInt[0] = pArg->arr[index++];
            break;
        case SDT_TEXT:          // 文本
        {
            LPCSTR str = (LPCSTR)pArg->arr[index++];
            int size = ( str && str[0] ) ? (int)strlen(str) : 0;
            if ( arg.m_ppText[0] )
                efree(arg.m_ppText[0]);
            arg.m_ppText[0] = CloneTextData(str, size);
            break;
        }
        case SDT_INT64:         // 长整数
        case SDT_DOUBLE:        // 双精度小数
        case SDT_DATE_TIME:     // 日期时间
        {
            LPINT p = (LPINT)arg.m_int;
            p[0] = pArg->arr[index++];
            p[1] = pArg->arr[index++];
            break;
        }
        case SDT_BIN:           // 字节集
        {
            LPBYTE pData = (LPBYTE)pArg->arr[index++];
            int size = 0;
            LPBYTE p = GetAryElementInf(pData, &size);
            if ( arg.m_ppBin[0] )
                efree(arg.m_ppBin[0]);
            arg.m_ppBin[0] = CloneBinData(p, size);
            break;
        }
        default:
            arg.m_pInt[0] = pArg->arr[index++];
            break;
        }
    }
    pRetData->m_bool = true;
}

// 调用格式: SDT_INT 多线程_生成无限参数, 命令说明: "获取线程池投递任务的参数信息"
// 参数<1>: 参数& _SDT_ALL, 参数说明: "支持11个易语言的基础数据类型, 遇到空参数或者不是基础数据类型则视为结束传参"
// 变长参数...
ETHREAD_EXTERN_C void ethread_Make_Arguments_33_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    if ( nArgCount == 0 )
        return;
    
    PTHREAD_ARG_STRUCT args = _thread_GetArgs(nArgCount, pArgInf, 0, (LPVOID)pArgInf[1].m_dwSubCodeAdr);
    // m_set.insert(args); // 没必要记录.... 用户自己申请自己释放
    pRetData->m_int = (int)args;
}

// 调用格式: SDT_INT 多线程_还原无限参数, 命令说明: "还原 多线程_生成无限参数() 填写的参数"
// 参数<0>: 参数指针 SDT_INT, 参数说明: "线程池初始化时填写的回调函数的第二个参数"
// 参数<1>: 参数& _SDT_ALL, 参数说明: "接收参数, 投递任务时填写什么参数这里就可以填写什么参数, 为空不获取, 不校验参数类型"
// 变长参数...
ETHREAD_EXTERN_C void ethread_Restore_Argument_34_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ethread_GetArgument_32_ethread(pRetData, nArgCount, pArgInf);
}


// 调用格式: SDT_INT 多线程_释放无限参数, 命令说明: "释放 多线程_生成无限参数() 分配的内存"
// 参数<0>: 参数指针 SDT_INT, 参数说明: "线程池初始化时填写的回调函数的第二个参数"
ETHREAD_EXTERN_C void ethread_Free_Argument_35_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    PTHREAD_ARG_STRUCT pArg = (PTHREAD_ARG_STRUCT)pArgInf[0].m_int;
    if ( !pArg ) return;
    delete pArg;
    pRetData->m_bool = true;
}
