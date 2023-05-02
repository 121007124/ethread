#include "include_ethread_header.h"
#include "CThreadPool.h"
#include "elib/fnshare.h"
//#include <unordered_set>

// ����û��Ҫ.... ��������ü�������, �����ͷŵ�ʱ��Ҳ�ǳ��������ʱ����ͷ�, ���û��Ҫ��¼��
//static std::unordered_set<PTHREAD_ARG_STRUCT> m_set;    // ��¼���ɵ����޲�����ַ, �����ͷ�

typedef void ( CALLBACK* PFN_Complete )( LPVOID, LPVOID, LPVOID );
class CMyThreadPool : public ThreadPool
{
public:
    PFN_Complete m_pfnComplete; // ������ɻص�
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

// ���ø�ʽ: _SDT_NULL �̳߳ع��캯��, ����˵��: "�̳߳ع��캯��"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPoolConsturct_8_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    pArgInf[0].m_ppCompoundData[0] = 0;
}

// ���ø�ʽ: _SDT_NULL �̳߳ظ��ƹ��캯��, ����˵��: "�̳߳ظ��ƹ��캯��"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPoolCopy_9_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    // �������һ������, �������ǵȺ��ұߵ�����, ����ֱ�Ӵ���һ���µ��̳߳ظ��Ⱥ���ߵĶ���

    CMyThreadPool* poolLeft = pool_getobj(pArgInf[0], 0);    // �Ⱥ���ߵ�ֵ
    CMyThreadPool* poolRight = pool_getobj(pArgInf[1], 0);    // �Ⱥ��ұߵ�ֵ
    
    // ����ֱ���½�һ���̳߳طŵȺ����, һ��������̳߳��ǲ�֧�ָ��ƹ���, ���￴�Լ�����
    // ���߿��Ըĳ����ü����ķ�ʽ, �õȺ����ߵ��̳߳ض���ͬһ������
    if ( !poolLeft && poolRight )
    {
        // �Ⱥ����û�еĲŴ���
        int size = (int)poolRight->size();
        int maxSize = (int)poolRight->maxSize();
        poolLeft = new CMyThreadPool{ size, maxSize, poolRight->m_pfnComplete };
    }
    pArgInf[0].m_ppCompoundData[0] = poolLeft;
}

// ���ø�ʽ: _SDT_NULL �̳߳���������, ����˵��: "�̳߳���������"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPoolFree_10_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    auto pool = pool_getobj(pArgInf[0], 0);
    if ( pool )
    {
        delete pool;
        pArgInf[0].m_ppCompoundData[0] = 0;
    }
}



// ���ø�ʽ: SDT_BOOL ��ʼ��, ����˵��: "��ʼ���̳߳�, �����Ƿ��ʼ���ɹ�"
// ����<1>: ����߳��� SDT_INT, ����˵��: "�̳߳���С�߳���, С�ڵ���0��Ĭ��Ϊ5, ������ʱ��ͷ�����߳���"
// ����<2>: ����߳��� SDT_INT, ����˵��: "�̳߳�����߳���, С�ڵ���0��Ĭ��Ϊ10, ���32767���߳�, һ���ò�����ô���߳�, ����õ���ô���߳�, ��Ӧ�ÿ����Ż�������"
// ����<3>: ������ɻص� _SDT_ALL, ����˵��: "Ͷ�ݵ�����ִ����Ϻ���������ص�����, �ص�����ԭ��  �޷���ֵ �ص�(���񷵻�ֵ, Ͷ��������д�Ĳ���ָ��)"
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

// ���ø�ʽ: SDT_BOOL �������, ����˵��: "����߳�����, �ص������ж��ٸ���������д���ٸ�����, �����Ƿ���ӳɹ�"
// ����<1>: ��ִ�е��ӳ��� _SDT_ALL, ����˵��: "�������ṩ�����߳�ʱ���������ӳ���, ֻ֧�ִ��������͵��ӳ����ַ�����ӳ���ָ������, ��������ֱ�ӵ���ʧ��"
// ����<2>: �̲߳��� SDT_INT, ����˵��: "���ݵ��߳���Ĳ���"
// �䳤����....
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
                    if ( pfnComplete )  // ����������ɻص�
                    {
                        // ��������������, �ṩ�������汾��չʹ��
                        int arg[3] = { r, (int)args, (int)0 };
                        
                        // Ҳ��Ҫ��һ��call, ������ƽջ
                        _threadpool_call(pfnComplete, arg, sizeof(arg) / sizeof(arg[0]));
                    }
                    delete args;
                    return r;
                }, args, pool->m_pfnComplete);
    pRetData->m_bool = true;
    
}


// ���ø�ʽ: SDT_BOOL (�̳߳�).����, ����˵��: "�����̳߳�, ���ݴ��ݵĲ��������������, Ĭ���ǵȴ������������"
// ����<1>: [����ģʽ SDT_INT], ����˵��: "1=���߳���������, ����=�ȴ��߳̽���"
ETHREAD_EXTERN_C void ethread_ThreadPool_Destroy_13_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[1].m_int;
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;

    pArgInf[0].m_ppCompoundData[0] = 0; // �������Լ�¼���ڴ�ؾ������Ϊ0

    if ( arg1 == CMyThreadPool::POOL_DESTROY_MODE::POOL_DESTROY_MODE_DETACH )
    {
        // ��������߳���������, ��������delete pool, ��Ȼ���������thisָ���Ѿ����ͷ���, �ڲ��ټ���ʹ�þͱ�����
        std::thread t([pool, arg1]()
                      {
                          // ֱ�ӵȴ��̳߳���������ִ����Ϻ�ŷ���
                          pool->destroy(CMyThreadPool::POOL_DESTROY_MODE::POOL_DESTROY_MODE_WAIT);
                          delete pool;  // �ͷŶ���
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
        //                  // ֱ�ӵȴ��̳߳���������ִ����Ϻ�ŷ���
        //                  pool->destroy(CMyThreadPool::POOL_DESTROY_MODE::POOL_DESTROY_MODE_WAIT);
        //                  delete pool;  // �ͷŶ���
        //              });
        //t.detach();
        pRetData->m_bool = pool->destroy(arg1);
        pRetData->m_bool = true;
        return;
    }

    // �ߵ��������Ҫ�ȴ���������ŷ���
    pRetData->m_bool = pool->destroy(arg1);
    delete pool;
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡ�����߳���, ����˵��: "ȡ��ǰ���е��߳���"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_GetVacant_14_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetVacant();
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡִ���߳���, ����˵��: "ȡ��ǰִ�е��߳���"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_GetExecuteCount_15_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetExecuteCount();
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡ����������, ����˵��: "ȡ�ȴ�ִ�е���������, ��ӵ������ﻹû��ִ�е�����"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_GetQueueCount_16_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetQueueCount();
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡ�̳߳�����, ����˵��: "ȡ��ǰ�̳߳ش������߳�����"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_size_17_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->size();
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡ�̳߳��������, ����˵��: "ȡ��ǰ�̳߳ش���ʱ��д������߳�����"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_MaxSize_18_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->maxSize();
}

// ���ø�ʽ: SDT_INT (�̳߳�).�Ƿ����, ����˵��: "ȡ��ǰ�̳߳��Ƿ����"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_IsVacant_19_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->IsVacant();
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡ״̬, ����˵��: "ȡ��ǰ�̳߳�״̬, 0=����, 1=���ڹ���, 2=���ڴ���, 3=��������"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_GetState_20_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetState();
}

// ���ø�ʽ: SDT_BOOL (�̳߳�).��������, ����˵��: "�����̳߳����������߳�"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_Suspend_29_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->suspend();
}

// ���ø�ʽ: SDT_BOOL (�̳߳�).��������, ����˵��: "�����̳߳����������߳�"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_Resume_30_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->suspend();
}

// ���ø�ʽ: SDT_BOOL (�̳߳�).ȡ��Ϣ, ����˵��: "��ȡ�̳߳ػ�����Ϣ"
// ����<1>: ������Ϣ����& SDT_TYPE_POOLINFO, ����˵��: "�����̳߳���Ϣ�Ľṹ����"
ETHREAD_EXTERN_C void ethread_ThreadPool_GetInfo_31_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    CMyThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    CMyThreadPool::PTHREAD_POOL_INFO info = *((CMyThreadPool::PTHREAD_POOL_INFO*)pArgInf[1].m_int);
    pRetData->m_int = pool->GetInfo(info);
    pRetData->m_bool = true;
}


// ���ø�ʽ: SDT_BOOL ȡ��������, ����˵��: "��ȡ�̳߳�Ͷ������Ĳ�����Ϣ"
// ����<0>: ����ָ�� SDT_INT, ����˵��: "�̳߳س�ʼ��ʱ��д�Ļص������ĵڶ�������"
// ����<1>: ����& _SDT_ALL, ����˵��: "���ղ���, Ͷ������ʱ��дʲô��������Ϳ�����дʲô����, Ϊ�ղ���ȡ, ��У���������"
// �䳤����...
ETHREAD_EXTERN_C void ethread_GetArgument_32_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    PTHREAD_ARG_STRUCT pArg = (PTHREAD_ARG_STRUCT)pArgInf[0].m_int;
    if ( !pArg ) return;
    
    int index = 0;
    // ��0��������ָ��, �ӵ�1��������ʼѭ��
    for ( int i = 1; i < nArgCount; i++ )
    {
        MDATA_INF& arg = pArgInf[i];
        DATA_TYPE type = arg.m_dtDataType & ~DT_IS_ARY;
        if ( type == _SDT_NULL )
            break;
        switch ( type )
        {
        case SDT_BYTE:          // �ֽ�
        case SDT_SHORT:         // ������
        case SDT_INT:           // ����
        case SDT_SUB_PTR:       // �ӳ���ָ��
        case SDT_FLOAT:         // С��
        case SDT_BOOL:          // �߼�
            arg.m_pInt[0] = pArg->arr[index++];
            break;
        case SDT_TEXT:          // �ı�
        {
            LPCSTR str = (LPCSTR)pArg->arr[index++];
            int size = ( str && str[0] ) ? (int)strlen(str) : 0;
            if ( arg.m_ppText[0] )
                efree(arg.m_ppText[0]);
            arg.m_ppText[0] = CloneTextData(str, size);
            break;
        }
        case SDT_INT64:         // ������
        case SDT_DOUBLE:        // ˫����С��
        case SDT_DATE_TIME:     // ����ʱ��
        {
            LPINT p = (LPINT)arg.m_int;
            p[0] = pArg->arr[index++];
            p[1] = pArg->arr[index++];
            break;
        }
        case SDT_BIN:           // �ֽڼ�
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

// ���ø�ʽ: SDT_INT ���߳�_�������޲���, ����˵��: "��ȡ�̳߳�Ͷ������Ĳ�����Ϣ"
// ����<1>: ����& _SDT_ALL, ����˵��: "֧��11�������ԵĻ�����������, �����ղ������߲��ǻ���������������Ϊ��������"
// �䳤����...
ETHREAD_EXTERN_C void ethread_Make_Arguments_33_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    if ( nArgCount == 0 )
        return;
    
    PTHREAD_ARG_STRUCT args = _thread_GetArgs(nArgCount, pArgInf, 0, (LPVOID)pArgInf[1].m_dwSubCodeAdr);
    // m_set.insert(args); // û��Ҫ��¼.... �û��Լ������Լ��ͷ�
    pRetData->m_int = (int)args;
}

// ���ø�ʽ: SDT_INT ���߳�_��ԭ���޲���, ����˵��: "��ԭ ���߳�_�������޲���() ��д�Ĳ���"
// ����<0>: ����ָ�� SDT_INT, ����˵��: "�̳߳س�ʼ��ʱ��д�Ļص������ĵڶ�������"
// ����<1>: ����& _SDT_ALL, ����˵��: "���ղ���, Ͷ������ʱ��дʲô��������Ϳ�����дʲô����, Ϊ�ղ���ȡ, ��У���������"
// �䳤����...
ETHREAD_EXTERN_C void ethread_Restore_Argument_34_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ethread_GetArgument_32_ethread(pRetData, nArgCount, pArgInf);
}


// ���ø�ʽ: SDT_INT ���߳�_�ͷ����޲���, ����˵��: "�ͷ� ���߳�_�������޲���() ������ڴ�"
// ����<0>: ����ָ�� SDT_INT, ����˵��: "�̳߳س�ʼ��ʱ��д�Ļص������ĵڶ�������"
ETHREAD_EXTERN_C void ethread_Free_Argument_35_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    PTHREAD_ARG_STRUCT pArg = (PTHREAD_ARG_STRUCT)pArgInf[0].m_int;
    if ( !pArg ) return;
    delete pArg;
    pRetData->m_bool = true;
}
