#include "include_ethread_header.h"

__declspec( naked ) int _thread_call(LPVOID pfn, LPVOID pArg)
{
    __asm
    {
        push ebp
        mov ebp, esp
        push pArg
        call pfn        // 易语言这边的子程序
        mov esp, ebp
        pop ebp
        ret
    }
}



// 等待1个或多个对象, 只要有一个对象触发事件就返回, 返回触发对象在数组中索引
// hObjects 等待对象的对象数组
// ObjectCount = 等待的对象数量
// loopMsg = 是否消息循环, 如果是在线程内, 可以为假, 为真时会处理消息
// waitTimer = 等待时间, 默认为 INFINITE , 无限等待
inline DWORD ethread_WaitObj_ethread(HANDLE* hObjects, DWORD ObjectCount, bool loopMsg = false, DWORD waitTimer = INFINITE)
{
    if ( ObjectCount == 0 || hObjects == 0 || *hObjects == 0 ) return 0;

    if ( loopMsg )
    {
        ULONGLONG time = GetTickCount64();
        while ( GetTickCount64() - time < (ULONGLONG)waitTimer )
        {
            DWORD ret = MsgWaitForMultipleObjects(ObjectCount, hObjects, FALSE, waitTimer, QS_ALLINPUT);
            if ( ret >= WAIT_OBJECT_0 && ret < WAIT_OBJECT_0 + ObjectCount )
                return (DWORD)0;    // 超时, 失败, 成功 都退出循环
            if ( ret == WAIT_TIMEOUT || ret == WAIT_FAILED )
                return (DWORD)-1;
            if ( ret == WAIT_OBJECT_0 + ObjectCount )
            {
                MSG msg;
                if ( PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) )
                {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
        }
    }
    else
    {
        if ( ObjectCount == 1 )
            return WaitForSingleObject(*hObjects, waitTimer);
        return WaitForMultipleObjects(ObjectCount, hObjects, FALSE, waitTimer);
    }
    return (DWORD)-1;
}

// 等待单个对象
// hObject 等待的对象句柄
// loopMsg = 是否消息循环, 如果是在线程内, 可以为假, 为真时会处理消息
// waitTimer = 等待时间, 默认为 INFINITE , 无限等待
inline DWORD ethread_WaitObj_ethread(HANDLE hObject, bool loopMsg = false, DWORD waitTimer = INFINITE)
{
    return ethread_WaitObj_ethread(&hObject, 1, loopMsg, waitTimer);
}



// 调用格式: SDT_BOOL 启动线程, 命令说明: "创建并启动一条线程，可重复使用以创建多条线程，成功返回真，失败返回假"
// 参数<1>: 欲执行的子程序 _SDT_ALL, 参数说明: "本参数提供创建线程时欲启动的子程序，根据是否需要传递参数数据，该子程序必须没有或具有一个整数型参数，否则将出错"
// 参数<2>: [参数数据 SDT_INT], 参数说明: "本参数提供线程被启动时传递到欲启动子程序的整数数据。如果本参数未被省略，此时被启动子程序必须接收一个整数型参数，否则将出错"
// 参数<3>: [&线程句柄 SDT_INT], 参数说明: "如果提供了本参数，将向参数变量写入线程句柄（Windows下为HANDLE，Linux下为pthread_t），请在适当的时机关闭该句柄。如果不提供本参数接收线程句柄，内部将自动处理线程句柄。"
ETHREAD_EXTERN_C void ethread_CreateThread_0_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    if ( pArgInf[0].m_dtDataType != SDT_INT && pArgInf[0].m_dtDataType != SDT_SUB_PTR )
        return;
    DWORD    arg1 = pArgInf[0].m_dwSubCodeAdr;
    INT      arg2 = pArgInf[1].m_int;
    PINT     arg3 = pArgInf[2].m_pInt;
    if ( !arg1 ) return;

    typedef DWORD(CALLBACK* PFN_ThreadProc)( LPVOID );
    struct TMP_THREAD_STRUCT
    {
        PFN_ThreadProc pfn;
        LPVOID pArg;
    };

    auto pfn_thread = [](LPVOID pArg) -> DWORD
    {
        TMP_THREAD_STRUCT* ptr = (TMP_THREAD_STRUCT*)pArg;
        DWORD ret = _thread_call(ptr->pfn, ptr->pArg);
        delete ptr;
        return ret;
    };

    TMP_THREAD_STRUCT* ptr = new TMP_THREAD_STRUCT;
    ptr->pfn = (PFN_ThreadProc)arg1;
    ptr->pArg = (LPVOID)arg2;
    HANDLE hThread = CreateThread(0, 0, pfn_thread, ptr, 0, 0);
    if ( !hThread )
        return;
    pRetData->m_int = 1;
    if ( arg3 )
    {
        *arg3 = (int)hThread;
    }
    else
    {
        CloseHandle(hThread);
    }
}

// 调用格式: SDT_INT 创建进入许可证, 命令说明: "创建并返回一个进入许可证数值，此许可证值用作进入程序中的指定许可代码区，以避免多线程冲突。成功返回非零整数值，失败返回0。所创建的许可证在不再使用后，必须使用“删除进入许可证”命令将其删除"
// 无参数
ETHREAD_EXTERN_C void ethread_CreateCriticalSection_1_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    LPCRITICAL_SECTION pCriticalSection = new CRITICAL_SECTION;
    InitializeCriticalSection(pCriticalSection);
    pRetData->m_int = (int)pCriticalSection;
}

// 调用格式: SDT_BOOL 删除进入许可证, 命令说明: "删除由“创建进入许可证”命令所创建返回的进入许可证。成功返回真，失败返回假"
// 参数<1>: 进入许可证 SDT_INT, 参数说明: "本参数应提供由“创建进入许可证”命令所创建并返回的数值"
ETHREAD_EXTERN_C void ethread_DeleteCriticalSection_2_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    LPCRITICAL_SECTION pCriticalSection = (LPCRITICAL_SECTION)arg1;
    if ( !pCriticalSection ) return;
    DeleteCriticalSection(pCriticalSection);
    delete pCriticalSection;
    pRetData->m_bool = true;
}

// 调用格式: _SDT_NULL 进入许可区, 命令说明: "根据已经创建的许可证进入指定许可代码区，在此线程未退出之前，其它线程如要通过同一个进入许可证进入该许可代码区则必须先等待此线程退出许可代码区，从而保证了指定许可代码区在任何时候都只能有一条线程进入并执行"
// 参数<1>: 进入许可证 SDT_INT, 参数说明: "本参数应提供由“创建进入许可证”命令所创建并返回的数值"
ETHREAD_EXTERN_C void ethread_EnterCriticalSection_3_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    LPCRITICAL_SECTION pCriticalSection = (LPCRITICAL_SECTION)arg1;
    if ( !pCriticalSection ) return;
    EnterCriticalSection(pCriticalSection);
}

// 调用格式: _SDT_NULL 退出许可区, 命令说明: "指示当前线程将退出许可代码区，并允许其它使用同一进入许可证的线程进入此许可代码区"
// 参数<1>: 进入许可证 SDT_INT, 参数说明: "本参数应提供由“创建进入许可证”命令所创建并返回的数值"
ETHREAD_EXTERN_C void ethread_LeaveCriticalSection_4_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    LPCRITICAL_SECTION pCriticalSection = (LPCRITICAL_SECTION)arg1;
    if ( !pCriticalSection ) return;
    LeaveCriticalSection(pCriticalSection);
}

// 调用格式: SDT_BOOL 等待线程, 命令说明: "等待，直到指定线程结束或等待超时才返回。返回真表示等待成功。注意，在Linux下暂不支持超时等待。"
// 参数<1>: 线程句柄 SDT_INT, 参数说明: "可通过“启动线程”的第三个参数获取线程句柄。"
// 参数<2>: [等待时间 SDT_INT], 参数说明: "指定等待时间，单位为毫秒。如果为0或负数，或被省略，表示不限定等待时间。"
ETHREAD_EXTERN_C void ethread_WaitThread_5_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    INT      arg2 = pArgInf[1].m_int;

    //if ( arg2 <= 0 )    // 如果完整还原官方库的功能就需要这么判断, 如果需要扩展这两行代码不需要
    //    arg2 = INFINITE;
    HANDLE hThread = (HANDLE)arg1;
    DWORD dwMilliseconds = (DWORD)arg2;
    if ( !hThread ) return;

    DWORD ret = WaitForSingleObject(hThread, dwMilliseconds);
    pRetData->m_bool = ret != WAIT_FAILED;
}

// 调用格式: SDT_BOOL 强制结束线程, 命令说明: "返回真表示执行成功。"
// 参数<1>: 线程句柄 SDT_INT, 参数说明: "可通过“启动线程”的第三个参数获取线程句柄。"
ETHREAD_EXTERN_C void ethread_TerminateThread_6_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    HANDLE hThread = (HANDLE)arg1;
    if ( !hThread ) return;
    pRetData->m_bool = TerminateThread(hThread, 0);
}

// 调用格式: SDT_BOOL 关闭线程句柄, 命令说明: "返回真表示已成功关闭线程句柄。在Linux下，如果线程已经结束，本命令可能返回假。"
// 参数<1>: 线程句柄 SDT_INT, 参数说明: "可通过“启动线程”的第三个参数获取线程句柄。"
ETHREAD_EXTERN_C void ethread_CloseThreadHandle_7_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    HANDLE hThread = (HANDLE)arg1;
    if ( !hThread ) return;
    pRetData->m_bool = CloseHandle(hThread);
}


// 调用格式: SDT_INT 启动线程Ex, 命令说明: "创建并启动一条线程, 可重复使用以创建多条线程, 成功返回线程ID, 失败返回0"
// 参数<1>: 欲执行的子程序 _SDT_ALL, 参数说明: "本参数提供创建线程时欲启动的子程序, 只支持传递整数型的子程序地址或者子程序指针类型, 其他类型直接调用失败"
// 参数<2>: [创建标志 SDT_INT], 参数说明: "请参考 #启动线程_ 开头常量, 默认为 #启动线程_默认, 多个标志请使用 位或(值1, 值2, 值3, ...)"
// 参数<3>: [&线程句柄 SDT_INT], 参数说明: "如果提供了本参数，将向参数变量写入线程句柄（Windows下为HANDLE，Linux下为pthread_t），请在适当的时机关闭该句柄。如果不提供本参数接收线程句柄，内部将自动处理线程句柄。"
// 参数<4>: [线程参数 _SDT_ALL], 参数说明: "传递到线程里的参数"
// 变长参数, 命令参数表中最后一个参数可以被重复添加
ETHREAD_EXTERN_C void ethread_CreateThreadEx_21_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    LPTHREAD_START_ROUTINE pfn = (LPTHREAD_START_ROUTINE)pArgInf[0].m_pByte;
    INT      flags  = pArgInf[1].m_int;
    PINT     pThread = pArgInf[2].m_pInt;
    PVOID    arg4 = pArgInf[3].m_pByte;
    if ( !pfn ) return;
    
    PTHREAD_ARG_STRUCT args = _thread_GetArgs(nArgCount, pArgInf, 3, pfn);
    if ( !args ) return;
    
    auto pfn_threadProc = [](LPVOID pArg) -> DWORD
    {
        PTHREAD_ARG_STRUCT args = (PTHREAD_ARG_STRUCT)pArg;
        if ( args->hEvent )
            SetEvent(args->hEvent);
        int r = _threadpool_call(args->pfn, args->arr, args->count);
        if ( args->hEvent )
            CloseHandle(args->hEvent);
        delete args;
        return r;
    };
    
    enum THREAD_ENUM
    {
        THREAD_ENUM_DEF     = 0,    // 默认
        THREAD_ENUM_SUAPEND = 1,    // 启动后挂起
        THREAD_ENUM_EXECUTE = 2,    // 等待执行
        THREAD_ENUM_RET     = 4,    // 等待执行结束
        THREAD_ENUM_MSG     = 8,    // 等待时处理消息
    };

    const bool isMsg = __query(flags, THREAD_ENUM_MSG);
    DWORD tid = 0;
    DWORD dwCreationFlags = __query(flags, THREAD_ENUM_SUAPEND) ? CREATE_SUSPENDED : 0;
    // 不是等待执行结束, 且不是创建后就挂起, 且有等待线程执行的标志, 那就创建事件对象
    if ( !dwCreationFlags && !__query(flags, THREAD_ENUM_RET) && __query(flags, THREAD_ENUM_EXECUTE) )
        args->hEvent = CreateEventW(0, 0, 0, 0);
    
    HANDLE hThread = CreateThread(0, 0, pfn_threadProc, args, dwCreationFlags, &tid);
    if ( pThread )
        *pThread = (int)hThread;
    pRetData->m_int = tid;
    if ( dwCreationFlags ) return;  // 如果是创建后挂起, 那就返回
    
    // 等待线程执行结束, 这个要优先于等待线程被执行
    if ( __query(flags, THREAD_ENUM_RET) )
    {
        ethread_WaitObj_ethread(hThread, isMsg, INFINITE);  // 等待线程结束
        return;
    }
    
    // 等待线程被执行
    if ( args->hEvent )
    {
        ethread_WaitObj_ethread(args->hEvent, isMsg, INFINITE);  // 等待线程执行
        return;
    }

    
    // 否则就是默认执行, 创建后就返回, 前面已经创建了线程, 这里就不需要再处理

}

// 调用格式: SDT_INT 等待线程Ex, 命令说明: "等待线程, 直到线程结束或者超时才返回, 返回等待的值\r\n        返回0表示线程执行完毕\r\n        返回258则表示等待超时, 线程还在运行中\r\n        返回-1表示调用失败"
// 参数<1>: 线程句柄 SDT_INT, 参数说明: "可通过\"启动线程\" 或者 \"启动线程Ex\" 或者 \"打开线程\" 获得的线程句柄"
// 参数<2>: [等待时间 SDT_INT], 参数说明: "指定等待时间, 单位为毫秒, 如果为负数, 表示不限定等待时间, 如果为0则立马返回, 默认为0"
// 参数<3>: [处理事件 SDT_BOOL], 参数说明: "是否处理窗口的消息事件, 如果是窗口线程调用, 可以设置为真, 这样窗口就不会卡死, 默认为假"
ETHREAD_EXTERN_C void ethread_WaitThreadEx_22_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    INT      arg2 = pArgInf[1].m_int;
    BOOL     arg3 = pArgInf[2].m_bool;
    HANDLE hThread = (HANDLE)arg1;
    if ( !hThread ) return;
    DWORD dwRet = ethread_WaitObj_ethread(hThread, arg3, arg2);
    pRetData->m_int = dwRet;
}

// 调用格式: SDT_INT 挂起线程, 命令说明: "将指定线程的执行挂起,注意，同一个线程可以被挂起多次，如果挂起多次，就要恢复多次"
// 参数<1>: 线程句柄 SDT_INT, 参数说明: "可通过\"启动线程\" 或者 \"启动线程Ex\" 或者 \"打开线程\" 获得的线程句柄"
ETHREAD_EXTERN_C void ethread_SuspendThread_23_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    HANDLE hThread = (HANDLE)arg1;
    if ( !hThread )return;
    try
    {
        pRetData->m_bool = SuspendThread(hThread);
    }
    catch ( ... )
    {
        pRetData->m_bool = false;
    }
}

// 调用格式: SDT_INT 唤醒线程, 命令说明: "唤醒挂起线程"
// 参数<1>: 线程句柄 SDT_INT, 参数说明: "可通过\"启动线程\" 或者 \"启动线程Ex\" 或者 \"打开线程\" 获得的线程句柄"
ETHREAD_EXTERN_C void ethread_ResumeThread_24_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    HANDLE hThread = (HANDLE)arg1;
    if ( !hThread )return;
    try
    {
        pRetData->m_bool = ResumeThread(hThread);
    }
    catch ( ... )
    {
        pRetData->m_bool = false;
    }
}

// 调用格式: SDT_INT 打开线程, 命令说明: "打开一个线程, 返回打开的线程句柄, 不使用这个句柄时需要调用\"关闭线程句柄\"释放这个句柄"
// 参数<1>: 线程ID SDT_INT, 参数说明: "启动线程Ex/创建线程消息循环 返回的值"
ETHREAD_EXTERN_C void ethread_OpenThread_25_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, 0, arg1);
    pRetData->m_int = (INT)hThread;
}

typedef int( CALLBACK* PFN_THREADMSGCALLBACK )( UINT message, WPARAM wParam, LPARAM lParam );
struct THREAD_MSGLOOP_ARG
{
    PFN_THREADMSGCALLBACK pfn;
    HANDLE hEvent;
};
// 进程的线程消息, 父进程发送线程消息到这里, 这里开一条线程去处理
DWORD CALLBACK ThreadProc_MsgLoop(LPVOID pArg)  // 不释放线程, 有任务往里面丢, 没任务就等待
{
    THREAD_MSGLOOP_ARG* arg = (THREAD_MSGLOOP_ARG*)pArg;
    SetEvent(arg->hEvent);
    PFN_THREADMSGCALLBACK pfn = arg->pfn;

    MSG msg;
    while ( GetMessageW(&msg, 0, 0, 0) )
    {
        if ( msg.message == WM_QUIT )
            break;  // 退出消息循环

        // 其他消息就丢给消息回调函数处理
        int r = pfn(msg.message, msg.wParam, msg.lParam);
        // 返回值留给后续版本升级使用, 目前暂时不使用
        // 继续下一轮获取消息
    }
    return 0;
}

// 调用格式: SDT_INT 创建线程消息循环, 命令说明: "创建一个线程消息循环, 返回线程ID, 可以通过线程ID向这个消息循环投递消息\r\n        本命令创建的消息循环遇到 WM_QUIT(18) 消息时就会退出消息循环"
// 参数<1>: 消息回调 SDT_SUB_PTR, 参数说明: "接收线程消息回调的函数, 整数型 线程消息回调(整数型 消息值, 整数型 参数1, 整数型 参数2), 返回值保留给后续版本升级使用"
ETHREAD_EXTERN_C void ethread_CreateThreadMsgLoop_26_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    DWORD    arg1 = pArgInf[0].m_dwSubCodeAdr;
    if ( !arg1 ) return;
    DWORD tid = 0;
    THREAD_MSGLOOP_ARG* arg = new THREAD_MSGLOOP_ARG;
    arg->pfn = (PFN_THREADMSGCALLBACK)arg1;
    arg->hEvent = CreateEventW(0, 0, 0, 0);
    
    HANDLE hThread = CreateThread(0, 0, ThreadProc_MsgLoop, arg, 0, &tid);
    if ( hThread )
    {
        if( arg->hEvent )
        {
            // 等待事件对象触发信号, 线程被执行后就会触发这个事件
            WaitForSingleObject(arg->hEvent, INFINITE);
            CloseHandle(arg->hEvent);
            delete arg;
        }
        CloseHandle(hThread);
    }
    pRetData->m_int = tid;
}

// 调用格式: SDT_INT 退出线程消息循环, 命令说明: "退出线程消息循环, 只能退出由 \"创建线程消息循环\" 创建的消息循环\r\n        调用本命令是给消息循环的线程投递一条 WM_QUIT(18) 的消息\r\n        \"创建线程消息循环\" 创建的消息循环遇到 WM_QUIT(18) 消息时就会退出消息循环"
// 参数<1>: 线程ID SDT_INT, 参数说明: "创建线程消息循环 返回的值"
ETHREAD_EXTERN_C void ethread_QuitThreadMsgLoop_27_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    DWORD    arg1 = pArgInf[0].m_dwSubCodeAdr;
    pRetData->m_bool = PostThreadMessageW(arg1, WM_QUIT, 0, 0);
}

// 调用格式: SDT_BOOL 投递线程消息, 命令说明: "向有消息循环的线程投递一条消息, 成功返回真, 失败返回假, 返回假时可以调用 GetLastError 获取错误码\r\n        如果投递 WM_QUIT(18) 消息, 则 \"创建线程消息循环\" 创建的消息循环会退出循环"
// 参数<1>: 线程ID SDT_INT, 参数说明: "创建线程消息循环 返回的值"
// 参数<2>: 消息值 SDT_INT, 参数说明: "消息值的限制请查看MSDN, 大于65535的消息会投递失败"
// 参数<3>: [参数1 SDT_INT], 参数说明: "投递到消息队列里的参数1"
// 参数<4>: [参数2 SDT_INT], 参数说明: "投递到消息队列里的参数2"
ETHREAD_EXTERN_C void ethread_PostThreadMsg_28_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    INT      arg2 = pArgInf[1].m_int;
    INT      arg3 = pArgInf[2].m_int;
    INT      arg4 = pArgInf[3].m_int;
    pRetData->m_bool = PostThreadMessageW(arg1, arg2, arg3, arg4);
}
