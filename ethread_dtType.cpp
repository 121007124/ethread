#ifndef __E_STATIC_LIB
#include "include_ethread_header.h"


static INT s_dtCmdIndex_ethread_ThreadPool_static_var_00[] =
{
    8,9,10,11,13,12,14,15,16,17,18,19,20,
};

static const int s_dtCmdIndex_ethread_ThreadPool_static_var_count_00 = sizeof(s_dtCmdIndex_ethread_ThreadPool_static_var_00) / sizeof(s_dtCmdIndex_ethread_ThreadPool_static_var_00[0]);


//1=��������,2=Ӣ������,3=��ϸ����,4=��������,5=����ֵ,6=��־ LDT_
//����Ϊ���ڻ�˵����ʱ��Ч 7=��ԴID,8=�¼�����,9=����¼�����,10=������  11=�������� 12=��������ӳ���
//��Ϊ���ڡ��˵������Ϊö����������ʱ����Ч 13=��Ա����,14=��Ա��������
LIB_DATA_TYPE_INFO g_DataType_ethread_global_var[] =
{
/*000*/ { "�̳߳�", "ThreadPool", "�̳߳�, ���������: ����һ��ʹ�õ�����ô�����߳�, �Ǿͻ�ʹ�õ��ڶ���, ���Բ����Զ��ͷſ��е��߳�",
            s_dtCmdIndex_ethread_ThreadPool_static_var_count_00, s_dtCmdIndex_ethread_ThreadPool_static_var_00, _DT_OS(__OS_WIN),
            0, 0, 0,
            0, 0,
            0,
            NULL, NULL
        },

};

int g_DataType_ethread_global_var_count = sizeof(g_DataType_ethread_global_var) / sizeof(g_DataType_ethread_global_var[0]);

#endif



