#if !defined(__E_STATIC_LIB)
#include "include_ethread_header.h"

//TODO ��̬����Ҫ�Ĳ���,���¼���еĺ�������������,��̬�����ʱ����Ҫȡ��������
static ARG_INFO g_argumentInfo_ethread_global_var[] = 
{
    // 1=��������, 2=������ϸ����, 3=ָ��ͼ������,��1��ʼ,0��ʾ��, 4=ͼ����Ŀ(��������)
    // 5=��������, 6=����Ĭ��ֵ,   7=������־ AS_ ��ͷ����
    // AS_HAS_DEFAULT_VALUE         ��Ĭ��ֵ,�����ڶ���������Ĭ��ֵ
    // AS_DEFAULT_VALUE_IS_EMPTY    Ĭ��ֵΪ��,�пɿձ�־
    // AS_RECEIVE_VAR               ֻ�ܴ��ݱ���,�൱�ڴ�����,���ݹ����Ŀ϶���������
    // AS_RECEIVE_VAR_ARRAY         ���ݹ����Ŀ϶��������������
    // AS_RECEIVE_VAR_OR_ARRAY      ���ݱ������������������
    // AS_RECEIVE_ARRAY_DATA        ��������
    // AS_RECEIVE_ALL_TYPE_DATA     ��������/������
    // AS_RECEIVE_VAR_OR_OTHER      ���Դ��� ����/����/����ֵ����

    /*000*/ {"��ִ�е��ӳ���", "�������ṩ�����߳�ʱ���������ӳ��򣬸����Ƿ���Ҫ���ݲ������ݣ����ӳ������û�л����һ�������Ͳ��������򽫳���", 0, 0, _SDT_ALL, 0, NULL},
    /*001*/ {"��������", "�������ṩ�̱߳�����ʱ���ݵ��������ӳ�����������ݡ����������δ��ʡ�ԣ���ʱ�������ӳ���������һ�������Ͳ��������򽫳���", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*002*/ {"�߳̾��", "����ṩ�˱������������������д���߳̾����Windows��ΪHANDLE��Linux��Ϊpthread_t���������ʵ���ʱ���رոþ����������ṩ�����������߳̾�����ڲ����Զ������߳̾����", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY | AS_RECEIVE_VAR},

    /*003*/ {"�������֤", "������Ӧ�ṩ�ɡ������������֤�����������������ص���ֵ", 0, 0, SDT_INT, 0, NULL},

    /*004*/ {"�߳̾��", "��ͨ���������̡߳��ĵ�����������ȡ�߳̾����", 0, 0, SDT_INT, 0, NULL},
    /*005*/ {"�ȴ�ʱ��", "ָ���ȴ�ʱ�䣬��λΪ���롣���Ϊ0��������ʡ�ԣ���ʾ���޶��ȴ�ʱ�䡣", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},

    /*006*/ {"��С�߳���", "�̳߳���С�߳���, С�ڵ���0��Ĭ��Ϊ5, ������ʱ��ͷ�����߳���", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*007*/ {"����߳���", "�̳߳�����߳���, С�ڵ���0��Ĭ��Ϊ10, ���32767���߳�, һ���ò�����ô���߳�, ����õ���ô���߳�, ��Ӧ�ÿ����Ż�������", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},

    /*008*/ {"��ִ�е��ӳ���", "�������ṩ�����߳�ʱ���������ӳ���, ֻ֧�ִ��������͵��ӳ����ַ�����ӳ���ָ������, ��������ֱ�ӵ���ʧ��", 0, 0, _SDT_ALL, 0, NULL},
    /*009*/ {"�̲߳���", "���ݵ��߳���Ĳ���, ֧��11�������ԵĻ�����������, �����ղ������߲��ǻ���������������Ϊ��������", 0, 0, _SDT_ALL, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    
    /*010*/ {"����ģʽ", "1=���߳���������, ����=�ȴ��߳̽���", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},

    /*011*/ {"��ִ�е��ӳ���", "�������ṩ�����߳�ʱ���������ӳ���, ֻ֧�ִ��������͵��ӳ����ַ�����ӳ���ָ������, ��������ֱ�ӵ���ʧ��", 0, 0, _SDT_ALL, 0, NULL},
    /*012*/ {"������־", "��ο� #�����߳�_ ��ͷ����, Ĭ��Ϊ #�����߳�_Ĭ��, �����־��ʹ�� λ��(ֵ1, ֵ2, ֵ3, ...)", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*013*/ {"�߳̾��", "����ṩ�˱������������������д���߳̾����Windows��ΪHANDLE��Linux��Ϊpthread_t���������ʵ���ʱ���رոþ����������ṩ�����������߳̾�����ڲ����Զ������߳̾����", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY | AS_RECEIVE_VAR},
    /*014*/ {"�̲߳���", "���ݵ��߳���Ĳ���, ֧��11�������ԵĻ�����������, �����ղ������߲��ǻ���������������Ϊ��������", 0, 0, _SDT_ALL, 0, AS_DEFAULT_VALUE_IS_EMPTY},

    /*015*/ {"�߳̾��", "��ͨ��\"�����߳�\" ���� \"�����߳�Ex\" ���� \"���߳�\" ��õ��߳̾��", 0, 0, SDT_INT, 0, NULL},
    /*016*/ {"�ȴ�ʱ��", "ָ���ȴ�ʱ��, ��λΪ����, ���Ϊ����, ��ʾ���޶��ȴ�ʱ��, ���Ϊ0��������, Ĭ��Ϊ0", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*017*/ {"�����¼�", "�Ƿ����ڵ���Ϣ�¼�, ����Ǵ����̵߳���, ��������Ϊ��, �������ھͲ��Ῠ��, Ĭ��Ϊ��", 0, 0, SDT_BOOL, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    
    /*018*/ {"�߳�ID", "�����߳�Ex/�����߳���Ϣѭ�� ���ص�ֵ", 0, 0, SDT_INT, 0, 0},
    
    /*019*/ {"��Ϣ�ص�", "�����߳���Ϣ�ص��ĺ���, ������ �߳���Ϣ�ص�(������ ��Ϣֵ, ������ ����1, ������ ����2), ����ֵ�����������汾����ʹ��", 0, 0, SDT_SUB_PTR, 0, 0},
    
    /*020*/ {"�߳�ID", "�����߳���Ϣѭ�� ���ص�ֵ", 0, 0, SDT_INT, 0, 0},
    /*021*/ {"��Ϣֵ", "��Ϣֵ��������鿴MSDN, ����65535����Ϣ��Ͷ��ʧ��", 0, 0, SDT_INT, 0, 0},
    /*022*/ {"����1", "Ͷ�ݵ���Ϣ������Ĳ���1", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*023*/ {"����2", "Ͷ�ݵ���Ϣ������Ĳ���2", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},

    
};

#ifdef _DEBUG    // ������Ϊ��ȷ�ϲ�������Ƿ���ȷ, ��Ա�������һ����Ŵ�1������ȷ
const int dbg_cmd_arg_count__ = sizeof(g_argumentInfo_ethread_global_var) / sizeof(g_argumentInfo_ethread_global_var[0]);
#endif


#define ETHREAD_DEF_CMDINFO(_index, _szName, _szEgName, _szExplain, _shtCategory, _wState, _dtRetValType, _wReserved, _shtUserLevel, _shtBitmapIndex, _shtBitmapCount, _nArgCount, _pBeginArgInfo) \
    { _szName, ______E_FNENAME(_szEgName), _szExplain, _shtCategory, _wState, _dtRetValType, _wReserved, _shtUserLevel, _shtBitmapIndex, _shtBitmapCount, _nArgCount, _pBeginArgInfo },


// ������ÿ������Ķ���, �����Ϣ��ʾ��֧�ֿ��б���, ����ÿ����Ա������ϸ������һ���������Ϣ
CMD_INFO g_cmdInfo_ethread_global_var[] = 
{
    ETHREAD_DEF(ETHREAD_DEF_CMDINFO)
};

int g_cmdInfo_ethread_global_var_count = sizeof(g_cmdInfo_ethread_global_var) / sizeof(g_cmdInfo_ethread_global_var[0]);

#endif

