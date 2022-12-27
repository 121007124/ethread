#include "include_ethread_header.h"

__declspec( naked ) int _thread_call(LPVOID pfn, LPVOID pArg)
{
    __asm
    {
        push ebp
        mov ebp, esp
        push pArg
        call pfn        // ��������ߵ��ӳ���
        mov esp, ebp
        pop ebp
        ret
    }
}



// �ȴ�1����������, ֻҪ��һ�����󴥷��¼��ͷ���, ���ش�������������������
// hObjects �ȴ�����Ķ�������
// ObjectCount = �ȴ��Ķ�������
// loopMsg = �Ƿ���Ϣѭ��, ��������߳���, ����Ϊ��, Ϊ��ʱ�ᴦ����Ϣ
// waitTimer = �ȴ�ʱ��, Ĭ��Ϊ INFINITE , ���޵ȴ�
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
                return (DWORD)0;    // ��ʱ, ʧ��, �ɹ� ���˳�ѭ��
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

// �ȴ���������
// hObject �ȴ��Ķ�����
// loopMsg = �Ƿ���Ϣѭ��, ��������߳���, ����Ϊ��, Ϊ��ʱ�ᴦ����Ϣ
// waitTimer = �ȴ�ʱ��, Ĭ��Ϊ INFINITE , ���޵ȴ�
inline DWORD ethread_WaitObj_ethread(HANDLE hObject, bool loopMsg = false, DWORD waitTimer = INFINITE)
{
    return ethread_WaitObj_ethread(&hObject, 1, loopMsg, waitTimer);
}



// ���ø�ʽ: SDT_BOOL �����߳�, ����˵��: "����������һ���̣߳����ظ�ʹ���Դ��������̣߳��ɹ������棬ʧ�ܷ��ؼ�"
// ����<1>: ��ִ�е��ӳ��� _SDT_ALL, ����˵��: "�������ṩ�����߳�ʱ���������ӳ��򣬸����Ƿ���Ҫ���ݲ������ݣ����ӳ������û�л����һ�������Ͳ��������򽫳���"
// ����<2>: [�������� SDT_INT], ����˵��: "�������ṩ�̱߳�����ʱ���ݵ��������ӳ�����������ݡ����������δ��ʡ�ԣ���ʱ�������ӳ���������һ�������Ͳ��������򽫳���"
// ����<3>: [&�߳̾�� SDT_INT], ����˵��: "����ṩ�˱������������������д���߳̾����Windows��ΪHANDLE��Linux��Ϊpthread_t���������ʵ���ʱ���رոþ����������ṩ�����������߳̾�����ڲ����Զ������߳̾����"
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

// ���ø�ʽ: SDT_INT �����������֤, ����˵��: "����������һ���������֤��ֵ�������ֵ֤������������е�ָ����ɴ��������Ա�����̳߳�ͻ���ɹ����ط�������ֵ��ʧ�ܷ���0�������������֤�ڲ���ʹ�ú󣬱���ʹ�á�ɾ���������֤�������ɾ��"
// �޲���
ETHREAD_EXTERN_C void ethread_CreateCriticalSection_1_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    LPCRITICAL_SECTION pCriticalSection = new CRITICAL_SECTION;
    InitializeCriticalSection(pCriticalSection);
    pRetData->m_int = (int)pCriticalSection;
}

// ���ø�ʽ: SDT_BOOL ɾ���������֤, ����˵��: "ɾ���ɡ������������֤���������������صĽ������֤���ɹ������棬ʧ�ܷ��ؼ�"
// ����<1>: �������֤ SDT_INT, ����˵��: "������Ӧ�ṩ�ɡ������������֤�����������������ص���ֵ"
ETHREAD_EXTERN_C void ethread_DeleteCriticalSection_2_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    LPCRITICAL_SECTION pCriticalSection = (LPCRITICAL_SECTION)arg1;
    if ( !pCriticalSection ) return;
    DeleteCriticalSection(pCriticalSection);
    delete pCriticalSection;
    pRetData->m_bool = true;
}

// ���ø�ʽ: _SDT_NULL ���������, ����˵��: "�����Ѿ����������֤����ָ����ɴ��������ڴ��߳�δ�˳�֮ǰ�������߳���Ҫͨ��ͬһ���������֤�������ɴ�����������ȵȴ����߳��˳���ɴ��������Ӷ���֤��ָ����ɴ��������κ�ʱ��ֻ����һ���߳̽��벢ִ��"
// ����<1>: �������֤ SDT_INT, ����˵��: "������Ӧ�ṩ�ɡ������������֤�����������������ص���ֵ"
ETHREAD_EXTERN_C void ethread_EnterCriticalSection_3_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    LPCRITICAL_SECTION pCriticalSection = (LPCRITICAL_SECTION)arg1;
    if ( !pCriticalSection ) return;
    EnterCriticalSection(pCriticalSection);
}

// ���ø�ʽ: _SDT_NULL �˳������, ����˵��: "ָʾ��ǰ�߳̽��˳���ɴ�����������������ʹ��ͬһ�������֤���߳̽������ɴ�����"
// ����<1>: �������֤ SDT_INT, ����˵��: "������Ӧ�ṩ�ɡ������������֤�����������������ص���ֵ"
ETHREAD_EXTERN_C void ethread_LeaveCriticalSection_4_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    LPCRITICAL_SECTION pCriticalSection = (LPCRITICAL_SECTION)arg1;
    if ( !pCriticalSection ) return;
    LeaveCriticalSection(pCriticalSection);
}

// ���ø�ʽ: SDT_BOOL �ȴ��߳�, ����˵��: "�ȴ���ֱ��ָ���߳̽�����ȴ���ʱ�ŷ��ء��������ʾ�ȴ��ɹ���ע�⣬��Linux���ݲ�֧�ֳ�ʱ�ȴ���"
// ����<1>: �߳̾�� SDT_INT, ����˵��: "��ͨ���������̡߳��ĵ�����������ȡ�߳̾����"
// ����<2>: [�ȴ�ʱ�� SDT_INT], ����˵��: "ָ���ȴ�ʱ�䣬��λΪ���롣���Ϊ0��������ʡ�ԣ���ʾ���޶��ȴ�ʱ�䡣"
ETHREAD_EXTERN_C void ethread_WaitThread_5_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    INT      arg2 = pArgInf[1].m_int;

    //if ( arg2 <= 0 )    // ���������ԭ�ٷ���Ĺ��ܾ���Ҫ��ô�ж�, �����Ҫ��չ�����д��벻��Ҫ
    //    arg2 = INFINITE;
    HANDLE hThread = (HANDLE)arg1;
    DWORD dwMilliseconds = (DWORD)arg2;
    if ( !hThread ) return;

    DWORD ret = WaitForSingleObject(hThread, dwMilliseconds);
    pRetData->m_bool = ret != WAIT_FAILED;
}

// ���ø�ʽ: SDT_BOOL ǿ�ƽ����߳�, ����˵��: "�������ʾִ�гɹ���"
// ����<1>: �߳̾�� SDT_INT, ����˵��: "��ͨ���������̡߳��ĵ�����������ȡ�߳̾����"
ETHREAD_EXTERN_C void ethread_TerminateThread_6_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    HANDLE hThread = (HANDLE)arg1;
    if ( !hThread ) return;
    pRetData->m_bool = TerminateThread(hThread, 0);
}

// ���ø�ʽ: SDT_BOOL �ر��߳̾��, ����˵��: "�������ʾ�ѳɹ��ر��߳̾������Linux�£�����߳��Ѿ���������������ܷ��ؼ١�"
// ����<1>: �߳̾�� SDT_INT, ����˵��: "��ͨ���������̡߳��ĵ�����������ȡ�߳̾����"
ETHREAD_EXTERN_C void ethread_CloseThreadHandle_7_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    HANDLE hThread = (HANDLE)arg1;
    if ( !hThread ) return;
    pRetData->m_bool = CloseHandle(hThread);
}


// ���ø�ʽ: SDT_INT �����߳�Ex, ����˵��: "����������һ���߳�, ���ظ�ʹ���Դ��������߳�, �ɹ������߳�ID, ʧ�ܷ���0"
// ����<1>: ��ִ�е��ӳ��� _SDT_ALL, ����˵��: "�������ṩ�����߳�ʱ���������ӳ���, ֻ֧�ִ��������͵��ӳ����ַ�����ӳ���ָ������, ��������ֱ�ӵ���ʧ��"
// ����<2>: [������־ SDT_INT], ����˵��: "��ο� #�����߳�_ ��ͷ����, Ĭ��Ϊ #�����߳�_Ĭ��, �����־��ʹ�� λ��(ֵ1, ֵ2, ֵ3, ...)"
// ����<3>: [&�߳̾�� SDT_INT], ����˵��: "����ṩ�˱������������������д���߳̾����Windows��ΪHANDLE��Linux��Ϊpthread_t���������ʵ���ʱ���رոþ����������ṩ�����������߳̾�����ڲ����Զ������߳̾����"
// ����<4>: [�̲߳��� _SDT_ALL], ����˵��: "���ݵ��߳���Ĳ���"
// �䳤����, ��������������һ���������Ա��ظ����
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
        THREAD_ENUM_DEF     = 0,    // Ĭ��
        THREAD_ENUM_SUAPEND = 1,    // ���������
        THREAD_ENUM_EXECUTE = 2,    // �ȴ�ִ��
        THREAD_ENUM_RET     = 4,    // �ȴ�ִ�н���
        THREAD_ENUM_MSG     = 8,    // �ȴ�ʱ������Ϣ
    };

    const bool isMsg = __query(flags, THREAD_ENUM_MSG);
    DWORD tid = 0;
    DWORD dwCreationFlags = __query(flags, THREAD_ENUM_SUAPEND) ? CREATE_SUSPENDED : 0;
    // ���ǵȴ�ִ�н���, �Ҳ��Ǵ�����͹���, ���еȴ��߳�ִ�еı�־, �Ǿʹ����¼�����
    if ( !dwCreationFlags && !__query(flags, THREAD_ENUM_RET) && __query(flags, THREAD_ENUM_EXECUTE) )
        args->hEvent = CreateEventW(0, 0, 0, 0);
    
    HANDLE hThread = CreateThread(0, 0, pfn_threadProc, args, dwCreationFlags, &tid);
    if ( pThread )
        *pThread = (int)hThread;
    pRetData->m_int = tid;
    if ( dwCreationFlags ) return;  // ����Ǵ��������, �Ǿͷ���
    
    // �ȴ��߳�ִ�н���, ���Ҫ�����ڵȴ��̱߳�ִ��
    if ( __query(flags, THREAD_ENUM_RET) )
    {
        ethread_WaitObj_ethread(hThread, isMsg, INFINITE);  // �ȴ��߳̽���
        return;
    }
    
    // �ȴ��̱߳�ִ��
    if ( args->hEvent )
    {
        ethread_WaitObj_ethread(args->hEvent, isMsg, INFINITE);  // �ȴ��߳�ִ��
        return;
    }

    
    // �������Ĭ��ִ��, ������ͷ���, ǰ���Ѿ��������߳�, ����Ͳ���Ҫ�ٴ���

}

// ���ø�ʽ: SDT_INT �ȴ��߳�Ex, ����˵��: "�ȴ��߳�, ֱ���߳̽������߳�ʱ�ŷ���, ���صȴ���ֵ\r\n        ����0��ʾ�߳�ִ�����\r\n        ����258���ʾ�ȴ���ʱ, �̻߳���������\r\n        ����-1��ʾ����ʧ��"
// ����<1>: �߳̾�� SDT_INT, ����˵��: "��ͨ��\"�����߳�\" ���� \"�����߳�Ex\" ���� \"���߳�\" ��õ��߳̾��"
// ����<2>: [�ȴ�ʱ�� SDT_INT], ����˵��: "ָ���ȴ�ʱ��, ��λΪ����, ���Ϊ����, ��ʾ���޶��ȴ�ʱ��, ���Ϊ0��������, Ĭ��Ϊ0"
// ����<3>: [�����¼� SDT_BOOL], ����˵��: "�Ƿ����ڵ���Ϣ�¼�, ����Ǵ����̵߳���, ��������Ϊ��, �������ھͲ��Ῠ��, Ĭ��Ϊ��"
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

// ���ø�ʽ: SDT_INT �����߳�, ����˵��: "��ָ���̵߳�ִ�й���,ע�⣬ͬһ���߳̿��Ա������Σ���������Σ���Ҫ�ָ����"
// ����<1>: �߳̾�� SDT_INT, ����˵��: "��ͨ��\"�����߳�\" ���� \"�����߳�Ex\" ���� \"���߳�\" ��õ��߳̾��"
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

// ���ø�ʽ: SDT_INT �����߳�, ����˵��: "���ѹ����߳�"
// ����<1>: �߳̾�� SDT_INT, ����˵��: "��ͨ��\"�����߳�\" ���� \"�����߳�Ex\" ���� \"���߳�\" ��õ��߳̾��"
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

// ���ø�ʽ: SDT_INT ���߳�, ����˵��: "��һ���߳�, ���ش򿪵��߳̾��, ��ʹ��������ʱ��Ҫ����\"�ر��߳̾��\"�ͷ�������"
// ����<1>: �߳�ID SDT_INT, ����˵��: "�����߳�Ex/�����߳���Ϣѭ�� ���ص�ֵ"
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
// ���̵��߳���Ϣ, �����̷����߳���Ϣ������, ���￪һ���߳�ȥ����
DWORD CALLBACK ThreadProc_MsgLoop(LPVOID pArg)  // ���ͷ��߳�, �����������涪, û����͵ȴ�
{
    THREAD_MSGLOOP_ARG* arg = (THREAD_MSGLOOP_ARG*)pArg;
    SetEvent(arg->hEvent);
    PFN_THREADMSGCALLBACK pfn = arg->pfn;

    MSG msg;
    while ( GetMessageW(&msg, 0, 0, 0) )
    {
        if ( msg.message == WM_QUIT )
            break;  // �˳���Ϣѭ��

        // ������Ϣ�Ͷ�����Ϣ�ص���������
        int r = pfn(msg.message, msg.wParam, msg.lParam);
        // ����ֵ���������汾����ʹ��, Ŀǰ��ʱ��ʹ��
        // ������һ�ֻ�ȡ��Ϣ
    }
    return 0;
}

// ���ø�ʽ: SDT_INT �����߳���Ϣѭ��, ����˵��: "����һ���߳���Ϣѭ��, �����߳�ID, ����ͨ���߳�ID�������Ϣѭ��Ͷ����Ϣ\r\n        �����������Ϣѭ������ WM_QUIT(18) ��Ϣʱ�ͻ��˳���Ϣѭ��"
// ����<1>: ��Ϣ�ص� SDT_SUB_PTR, ����˵��: "�����߳���Ϣ�ص��ĺ���, ������ �߳���Ϣ�ص�(������ ��Ϣֵ, ������ ����1, ������ ����2), ����ֵ�����������汾����ʹ��"
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
            // �ȴ��¼����󴥷��ź�, �̱߳�ִ�к�ͻᴥ������¼�
            WaitForSingleObject(arg->hEvent, INFINITE);
            CloseHandle(arg->hEvent);
            delete arg;
        }
        CloseHandle(hThread);
    }
    pRetData->m_int = tid;
}

// ���ø�ʽ: SDT_INT �˳��߳���Ϣѭ��, ����˵��: "�˳��߳���Ϣѭ��, ֻ���˳��� \"�����߳���Ϣѭ��\" ��������Ϣѭ��\r\n        ���ñ������Ǹ���Ϣѭ�����߳�Ͷ��һ�� WM_QUIT(18) ����Ϣ\r\n        \"�����߳���Ϣѭ��\" ��������Ϣѭ������ WM_QUIT(18) ��Ϣʱ�ͻ��˳���Ϣѭ��"
// ����<1>: �߳�ID SDT_INT, ����˵��: "�����߳���Ϣѭ�� ���ص�ֵ"
ETHREAD_EXTERN_C void ethread_QuitThreadMsgLoop_27_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    DWORD    arg1 = pArgInf[0].m_dwSubCodeAdr;
    pRetData->m_bool = PostThreadMessageW(arg1, WM_QUIT, 0, 0);
}

// ���ø�ʽ: SDT_BOOL Ͷ���߳���Ϣ, ����˵��: "������Ϣѭ�����߳�Ͷ��һ����Ϣ, �ɹ�������, ʧ�ܷ��ؼ�, ���ؼ�ʱ���Ե��� GetLastError ��ȡ������\r\n        ���Ͷ�� WM_QUIT(18) ��Ϣ, �� \"�����߳���Ϣѭ��\" ��������Ϣѭ�����˳�ѭ��"
// ����<1>: �߳�ID SDT_INT, ����˵��: "�����߳���Ϣѭ�� ���ص�ֵ"
// ����<2>: ��Ϣֵ SDT_INT, ����˵��: "��Ϣֵ��������鿴MSDN, ����65535����Ϣ��Ͷ��ʧ��"
// ����<3>: [����1 SDT_INT], ����˵��: "Ͷ�ݵ���Ϣ������Ĳ���1"
// ����<4>: [����2 SDT_INT], ����˵��: "Ͷ�ݵ���Ϣ������Ĳ���2"
ETHREAD_EXTERN_C void ethread_PostThreadMsg_28_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[0].m_int;
    INT      arg2 = pArgInf[1].m_int;
    INT      arg3 = pArgInf[2].m_int;
    INT      arg4 = pArgInf[3].m_int;
    pRetData->m_bool = PostThreadMessageW(arg1, arg2, arg3, arg4);
}
