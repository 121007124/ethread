//#define    CT_NULL 0
//#define    CT_NUM  1      // value sample: 3.1415926
//#define    CT_BOOL 2      // value sample: 1
//#define    CT_TEXT 3      // value sample: "abc"
//LPCSTR m_szName;          // ������
//LPCSTR m_szEgName;        // Ӣ����
//LPCSTR m_szExplain;       // ˵��
//SHORT  m_shtLayout;       // ����Ϊ 1
//SHORT  m_shtType;         // ��������, CT_NULL=�ճ���, CT_NUM=��ֵ��,double����ֵ, CT_BOOL=�߼���, CT_TEXT=�ı���
//LPCSTR m_szText;          // CT_TEXT
//DOUBLE m_dbValue;         // CT_NUM��CT_BOOL
#include "include_ethread_header.h"

#ifndef __E_STATIC_LIB
LIB_CONST_INFO g_ConstInfo_ethread_global_var[] =
{
    //#define    CT_NULL 0
    //#define    CT_NUM  1      // value sample: 3.1415926
    //#define    CT_BOOL 2      // value sample: 1
    //#define    CT_TEXT 3      // value sample: "abc"
    //LPCSTR m_szName;          // ������
    //LPCSTR m_szEgName;        // Ӣ����
    //LPCSTR m_szExplain;       // ˵��
    //SHORT  m_shtLayout;       // ����Ϊ 1
    //SHORT  m_shtType;         // ��������, CT_NULL=�ճ���, CT_NUM=��ֵ��,double����ֵ, CT_BOOL=�߼���, CT_TEXT=�ı���
    //LPCSTR m_szText;          // CT_TEXT
    //DOUBLE m_dbValue;         // CT_NUM��CT_BOOL
    /*000*/ { "�����߳�_Ĭ��", "", "Ĭ�Ϸ�ʽ, �����̺߳�ͷ���", 1, CT_NUM, NULL, 0 },
    /*001*/ { "�����߳�_���������", NULL, "�������־ʱ, �����̺߳������߳�, ��Ҫ���� �����߳�() �������̹߳���, �������־ʱ����������־, ������ͷ���", 1, CT_NUM, NULL, 1 },
    /*002*/ { "�����߳�_�ȴ�ִ��", NULL, "�������־ʱ, �����̺߳��ȴ��߳̿�ʼ������ŷ���", 1, CT_NUM, NULL, 2 },
    /*003*/ { "�����߳�_�ȴ�ִ�����", NULL, "�������־ʱ, �����̺߳��ȴ��̹߳�����Ϻ�ŷ���", 1, CT_NUM, NULL, 4 },
    /*004*/ { "�����߳�_�����¼�", NULL, "�������־ʱ, �ȴ��߳�ִ�����ǰ�ᴦ������Ϣ, ���û�������־", 1, CT_NUM, NULL, 8 },
};
int g_ConstInfo_ethread_global_var_count = sizeof(g_ConstInfo_ethread_global_var) / sizeof(g_ConstInfo_ethread_global_var[0]);

#endif
