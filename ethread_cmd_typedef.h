#pragma once

#define __ETHREAD_NAME(_index, _name) __LIB2_FNE_NAME_LEFT(__E_FNENAME)##_##_name##_##_index##_

// ���ݺ�����������, ƴ�ӳ� �������_����_���_�������, ���� ethread_test_0_ethread
#define ETHREAD_NAME(_index, _name) __LIB2_FNE_NAME_LEFT(__ETHREAD_NAME(_index, _name))__LIB2_FNE_NAME_LEFT(__E_FNENAME)

// ���ݺ�����������, ƴ�ӳ� "�������_����_���_�������", ���� "ethread_test_0_ethread"
#define ETHREAD_NAME_STR(_index, _name) ______E_FNENAME(__ETHREAD_NAME(_index, _name))

#define ETHREAD_EMPTY(_s) 
// ����궨�������е�����, �Ժ���Ҫ����������, ���������, ������ʹ�������
#define ETHREAD_DEF(_MAKE) \
    _MAKE(  0, "�����߳�", CreateThread, "����������һ���̣߳����ظ�ʹ���Դ��������̣߳��ɹ������棬ʧ�ܷ��ؼ�", 1, _CMD_OS(__OS_WIN), SDT_BOOL, 0, LVL_SIMPLE, 0, 0, 3, g_argumentInfo_ethread_global_var + 0)\
    _MAKE(  1, "�����������֤", CreateCriticalSection, "����������һ���������֤��ֵ�������ֵ֤������������е�ָ����ɴ��������Ա�����̳߳�ͻ���ɹ����ط�������ֵ��ʧ�ܷ���0�������������֤�ڲ���ʹ�ú󣬱���ʹ�á�ɾ���������֤�������ɾ��", 1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 0, g_argumentInfo_ethread_global_var + 0)\
    _MAKE(  2, "ɾ���������֤", DeleteCriticalSection, "ɾ���ɡ������������֤���������������صĽ������֤���ɹ������棬ʧ�ܷ��ؼ�", 1, _CMD_OS(__OS_WIN), SDT_BOOL, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 3)\
    _MAKE(  3, "���������", EnterCriticalSection, "�����Ѿ����������֤����ָ����ɴ��������ڴ��߳�δ�˳�֮ǰ�������߳���Ҫͨ��ͬһ���������֤�������ɴ�����������ȵȴ����߳��˳���ɴ��������Ӷ���֤��ָ����ɴ��������κ�ʱ��ֻ����һ���߳̽��벢ִ��", 1, _CMD_OS(__OS_WIN), _SDT_NULL, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 3)\
    _MAKE(  4, "�˳������", LeaveCriticalSection, "ָʾ��ǰ�߳̽��˳���ɴ�����������������ʹ��ͬһ�������֤���߳̽������ɴ�����", 1, _CMD_OS(__OS_WIN), _SDT_NULL, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 3)\
    _MAKE(  5, "�ȴ��߳�", WaitThread, "�ȴ���ֱ��ָ���߳̽�����ȴ���ʱ�ŷ��ء��������ʾ�ȴ��ɹ���ע�⣬��Linux���ݲ�֧�ֳ�ʱ�ȴ���", 1, _CMD_OS(__OS_WIN), SDT_BOOL, 0, LVL_SIMPLE, 0, 0, 2, g_argumentInfo_ethread_global_var + 4)\
    _MAKE(  6, "ǿ�ƽ����߳�", TerminateThread, "�������ʾִ�гɹ���", 1, _CMD_OS(__OS_WIN), SDT_BOOL, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 4)\
    _MAKE(  7, "�ر��߳̾��", CloseThreadHandle, "�������ʾ�ѳɹ��ر��߳̾������Linux�£�����߳��Ѿ���������������ܷ��ؼ١�", 1, _CMD_OS(__OS_WIN), SDT_BOOL, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 4)\
    \
    ETHREAD_EMPTY("���濪ʼ�����̳߳صķ���")\
    \
    _MAKE(  8, "�̳߳ع��캯��", ThreadPoolConsturct, "�̳߳ع��캯��", -1, _CMD_OS(__OS_WIN) | CT_IS_OBJ_CONSTURCT_CMD | CT_IS_HIDED, _SDT_NULL, 0, LVL_SIMPLE, 0, 0, 0, 0)\
    _MAKE(  9, "�̳߳ظ��ƹ��캯��", ThreadPoolCopy, "�̳߳ظ��ƹ��캯��", -1, _CMD_OS(__OS_WIN) | CT_IS_OBJ_COPY_CMD | CT_IS_HIDED, _SDT_NULL, 0, LVL_SIMPLE, 0, 0, 0, 0)\
    _MAKE( 10, "�̳߳���������", ThreadPoolFree, "�̳߳���������", -1, _CMD_OS(__OS_WIN) | CT_IS_OBJ_FREE_CMD | CT_IS_HIDED, _SDT_NULL, 0, LVL_SIMPLE, 0, 0, 0, 0)\
    _MAKE( 11, "��ʼ��", ThreadPool_Init, "��ʼ���̳߳�, �����Ƿ��ʼ���ɹ�\r\n���������: ����һ��ʹ�õ�����ô�����߳�, �Ǿͻ�ʹ�õ��ڶ���, ���Բ����Զ��ͷſ��е��߳�", -1, _CMD_OS(__OS_WIN), SDT_BOOL, 0, LVL_SIMPLE, 0, 0, 2, g_argumentInfo_ethread_global_var + 6)\
    _MAKE( 12, "�������", ThreadPool_AddTask, "����߳�����, �ص������ж��ٸ���������д���ٸ�����, ����ʯͷ��ӳɹ�", -1, _CMD_OS(__OS_WIN) | CT_ALLOW_APPEND_NEW_ARG, SDT_BOOL, 0, LVL_SIMPLE, 0, 0, 2, g_argumentInfo_ethread_global_var + 8)\
    _MAKE( 13, "����", ThreadPool_Destroy, "�����̳߳�, ���ݴ��ݵĲ��������������, Ĭ���ǵȴ������������", -1, _CMD_OS(__OS_WIN), SDT_BOOL, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 10)\
    _MAKE( 14, "ȡ�����߳���", ThreadPool_GetVacant, "ȡ��ǰ���е��߳���", -1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 0, 0)\
    _MAKE( 15, "ȡִ���߳���", ThreadPool_GetExecuteCount, "ȡ��ǰִ�е��߳���", -1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 0, 0)\
    _MAKE( 16, "ȡ����������", ThreadPool_GetQueueCount, "ȡ�ȴ�ִ�е���������, ��ӵ������ﻹû��ִ�е�����", -1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 0, 0)\
    _MAKE( 17, "ȡ�̳߳�����", ThreadPool_size, "ȡ��ǰ�̳߳ش������߳�����", -1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 0, 0)\
    _MAKE( 18, "ȡ�̳߳��������", ThreadPool_MaxSize, "ȡ��ǰ�̳߳ش���ʱ��д������߳�����", -1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 0, 0)\
    _MAKE( 19, "�Ƿ����", ThreadPool_IsVacant, "ȡ��ǰ�̳߳��Ƿ����", -1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 0, 0)\
    _MAKE( 20, "ȡ״̬", ThreadPool_GetState, "ȡ��ǰ�̳߳�״̬, 0=����, 1=���ڹ���, 2=���ڴ���, 3=��������", -1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 0, 0)\
    \
    ETHREAD_EMPTY("���濪ʼ���Ƕ��߳���չ����")\
    \
    _MAKE( 21, "�����߳�Ex", CreateThreadEx, "����������һ���߳�, ���ظ�ʹ���Դ��������߳�, �ɹ������߳�ID, ʧ�ܷ���0", 1, _CMD_OS(__OS_WIN) | CT_ALLOW_APPEND_NEW_ARG, SDT_INT, 0, LVL_SIMPLE, 0, 0, 4, g_argumentInfo_ethread_global_var + 11)\
    _MAKE( 22, "�ȴ��߳�Ex", WaitThreadEx, "�ȴ��߳�, ֱ���߳̽������߳�ʱ�ŷ���, ���صȴ���ֵ\r\n"\
            "        ����0��ʾ�߳�ִ�����\r\n"\
            "        ����258���ʾ�ȴ���ʱ, �̻߳���������\r\n"\
            "        ����-1��ʾ����ʧ��", 1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 3, g_argumentInfo_ethread_global_var + 15)\
    _MAKE( 23, "�����߳�", SuspendThread, "��ָ���̵߳�ִ�й���,ע�⣬ͬһ���߳̿��Ա������Σ���������Σ���Ҫ�ָ����", 1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 15)\
    _MAKE( 24, "�����߳�", ResumeThread, "���ѹ����߳�", 1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 15)\
    _MAKE( 25, "���߳�", OpenThread, "��һ���߳�, ���ش򿪵��߳̾��, ��ʹ��������ʱ��Ҫ����\"�ر��߳̾��\"�ͷ�������", 1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 18)\
    _MAKE( 26, "�����߳���Ϣѭ��", CreateThreadMsgLoop, "����һ���߳���Ϣѭ��, �����߳�ID, ����ͨ���߳�ID�������Ϣѭ��Ͷ����Ϣ\r\n"\
            "        �����������Ϣѭ������ WM_QUIT(18) ��Ϣʱ�ͻ��˳���Ϣѭ��", 1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 19)\
    _MAKE( 27, "�˳��߳���Ϣѭ��", QuitThreadMsgLoop, "�˳��߳���Ϣѭ��, ֻ���˳��� \"�����߳���Ϣѭ��\" ��������Ϣѭ��\r\n"\
            "        ���ñ������Ǹ���Ϣѭ�����߳�Ͷ��һ�� WM_QUIT(18) ����Ϣ\r\n"\
            "        \"�����߳���Ϣѭ��\" ��������Ϣѭ������ WM_QUIT(18) ��Ϣʱ�ͻ��˳���Ϣѭ��", 1, _CMD_OS(__OS_WIN), SDT_INT, 0, LVL_SIMPLE, 0, 0, 1, g_argumentInfo_ethread_global_var + 20)\
    _MAKE( 28, "Ͷ���߳���Ϣ", PostThreadMsg, "������Ϣѭ�����߳�Ͷ��һ����Ϣ, �ɹ�������, ʧ�ܷ��ؼ�, ���ؼ�ʱ���Ե��� GetLastError ��ȡ������\r\n"\
            "        ���Ͷ�� WM_QUIT(18) ��Ϣ, �� \"�����߳���Ϣѭ��\" ��������Ϣѭ�����˳�ѭ��\r\n"\
            "    ��ע�⡿�������������Ͷ���߳���Ϣ�����Ȩ�޵�����, ������ؼ�, ���� GetLastError() ����5, �Ǿ��ǵ�ǰ���̵�Ȩ�޲�֧��Ͷ����Ϣ��Ŀ�����", 1, _CMD_OS(__OS_WIN), SDT_BOOL, 0, LVL_SIMPLE, 0, 0, 4, g_argumentInfo_ethread_global_var + 20)\
    \
    ETHREAD_EMPTY("���߳�֧�ֿ�������������")

