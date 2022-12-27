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

    ThreadPool* poolLeft = pool_getobj(pArgInf[0], 0);    // �Ⱥ���ߵ�ֵ
    ThreadPool* poolRight = pool_getobj(pArgInf[1], 0);    // �Ⱥ��ұߵ�ֵ
    
    // ����ֱ���½�һ���̳߳طŵȺ����, һ��������̳߳��ǲ�֧�ָ��ƹ���, ���￴�Լ�����
    // ���߿��Ըĳ����ü����ķ�ʽ, �õȺ����ߵ��̳߳ض���ͬһ������
    if ( !poolLeft )
    {
        // �Ⱥ����û�еĲŴ���
        poolLeft = new ThreadPool;
        pArgInf[0].m_ppCompoundData[0] = poolLeft;
    }
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
// ����<1>: ����߳��� SDT_INT, ����˵��: "�̳߳�����߳���, С�ڵ���0��Ĭ��Ϊ5"
ETHREAD_EXTERN_C void ethread_ThreadPool_Init_11_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    int count = pArgInf[1].m_int;
    int maxCount = pArgInf[2].m_int;
    if ( count <= 0 )
        count = 5;
    auto pool = pool_getobj(pArgInf[0], count, maxCount);
    pRetData->m_bool = pool != 0;
}

// ���ø�ʽ: SDT_BOOL �������, ����˵��: "����߳�����, �ص������ж��ٸ���������д���ٸ�����, ����ʯͷ��ӳɹ�"
// ����<1>: ��ִ�е��ӳ��� _SDT_ALL, ����˵��: "�������ṩ�����߳�ʱ���������ӳ���, ֻ֧�ִ��������͵��ӳ����ַ�����ӳ���ָ������, ��������ֱ�ӵ���ʧ��"
// ����<2>: �̲߳��� SDT_INT, ����˵��: "���ݵ��߳���Ĳ���"
// �䳤����....
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


// ���ø�ʽ: SDT_BOOL (�̳߳�).����, ����˵��: "�����̳߳�, ���ݴ��ݵĲ��������������, Ĭ���ǵȴ������������"
// ����<1>: [����ģʽ SDT_INT], ����˵��: "1=���߳���������, ����=�ȴ��߳̽���"
ETHREAD_EXTERN_C void ethread_ThreadPool_Destroy_13_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    INT      arg1 = pArgInf[1].m_int;
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_bool = pool->destroy(arg1);
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡ�����߳���, ����˵��: "ȡ��ǰ���е��߳���"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_GetVacant_14_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetVacant();
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡִ���߳���, ����˵��: "ȡ��ǰִ�е��߳���"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_GetExecuteCount_15_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetExecuteCount();
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡ����������, ����˵��: "ȡ�ȴ�ִ�е���������, ��ӵ������ﻹû��ִ�е�����"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_GetQueueCount_16_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetQueueCount();
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡ�̳߳�����, ����˵��: "ȡ��ǰ�̳߳ش������߳�����"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_size_17_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->size();
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡ�̳߳��������, ����˵��: "ȡ��ǰ�̳߳ش���ʱ��д������߳�����"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_MaxSize_18_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->maxSize();
}

// ���ø�ʽ: SDT_INT (�̳߳�).�Ƿ����, ����˵��: "ȡ��ǰ�̳߳��Ƿ����"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_IsVacant_19_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->IsVacant();
}

// ���ø�ʽ: SDT_INT (�̳߳�).ȡ״̬, ����˵��: "ȡ��ǰ�̳߳�״̬, 0=����, 1=���ڹ���, 2=���ڴ���, 3=��������"
// �޲���
ETHREAD_EXTERN_C void ethread_ThreadPool_GetState_20_ethread(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
    ThreadPool* pool = pool_getobj(pArgInf[0], 0);
    if ( !pool ) return;
    pRetData->m_int = pool->GetState();
}
