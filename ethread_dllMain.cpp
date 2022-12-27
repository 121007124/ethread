#include "include_ethread_header.h"
#include "elib/fnshare.h"
#include "elib/lang.h"
#pragma comment(lib, "legacy_stdio_definitions.lib")
#include <vcruntime_exception.h>


EXTERN_C INT WINAPI ethread_ProcessNotifyLib_ethread(INT nMsg, DWORD dwParam1, DWORD dwParam2);
#ifndef __E_STATIC_LIB
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:    // ��һ�α�����,LoadLibrary���ؾʹ���
        break;
    case DLL_PROCESS_DETACH:    // DLL���ͷ�
        break;
    case DLL_THREAD_ATTACH:     // �߳�ӳ��
        break;
    case DLL_THREAD_DETACH:     // �߳�ж��
        break;
    }
    return TRUE;
}

#define ETHREAD_DEF_CMD_PTR(_index, _szName, _szEgName, _szExplain, _shtCategory, _wState, _dtRetValType, _wReserved, _shtUserLevel, _shtBitmapIndex, _shtBitmapCount, _nArgCount, _pBeginArgInfo) \
    ETHREAD_NAME(_index, _szEgName),

PFN_EXECUTE_CMD g_cmdInfo_ethread_global_var_fun[] = { ETHREAD_DEF(ETHREAD_DEF_CMD_PTR) };

static LIB_INFO g_LibInfo_ethread_global_var = 
{
    /*m_dwLibFormatVer*/        LIB_FORMAT_VER, // ���ʽ��, Ӧ�õ���LIB_FORMAT_VER
    /*m_szGuid*/                "5F99C1642A2F4e03850721B4F5D7C3F8", // ��Ӧ�ڱ����ΨһGUID��, ����ΪNULL���, ������а汾�˴���Ӧ��ͬ
    /*m_nMajorVersion*/         3, // ��������汾��, �������0
    /*m_nMinorVersion*/         0, // ����Ĵΰ汾��
    /*m_nBuildNumber*/          3, // �����汾��, ����Դ˰汾�����κδ���

    /*m_nRqSysMajorVer*/        3, // ����Ҫ��������ϵͳ�����汾��
    /*m_nRqSysMinorVer*/        7, // ����Ҫ��������ϵͳ�Ĵΰ汾��
    /*m_nRqSysKrnlLibMajorVer*/ 3, // ����Ҫ��ϵͳ����֧�ֿ�����汾��
    /*m_nRqSysKrnlLibMinorVer*/ 7, // ����Ҫ��ϵͳ����֧�ֿ�Ĵΰ汾��

    /*m_szName*/                "���߳�֧�ֿ�Ex", // ����, ����ΪNULL���
    /*m_nLanguage*/             __GBK_LANG_VER, // ����֧�ֵ�����
    /*m_szExplain*/             "�ṩ�Զ��̵߳�֧�֣���ͨ��ʵ�ֽ������֤�����Ա�����̳߳�ͻ\r\n��֧�ֿ������2022-12-26", // ����ϸ����
    /*m_dwState*/               _LIB_OS(OS_ALL), // _LIB_OS() | LBS_ ��ͷ����, ����ǲ��,������� LBS_IDE_PLUGIN

    /*m_szAuthor*/              "����", // ����
    /*m_szZipCode*/             "000000", // �ʱ�
    /*m_szAddress*/             "�����", // ��ַ
    /*m_szPhoto*/               "131��ͷ���", // ��ϵ�绰
    /*m_szFax*/                 "123456", // ����
    /*m_szEmail*/               "121007124@qq.com", // ����
    /*m_szHomePage*/            "http://bbs.125.la", // ��ҳ
    /*m_szOther*/               "ף��һ����˳�������³ɣ�", // ������Ϣ

    /*m_nDataTypeCount*/        g_DataType_ethread_global_var_count, // �������Զ����������͵���Ŀ
    /*g_DataType_web*/          g_DataType_ethread_global_var, // ���������е��Զ�����������

    /*m_nCategoryCount*/        1, // ȫ�����������Ŀ, ��Ϊ0
    /*m_szzCategory*/           "0000���߳̿���\0""\0", // ȫ�����������Ŀ, ��Ϊ0

    /*m_nCmdCount*/             g_cmdInfo_ethread_global_var_count, // �������ṩ����������(ȫ�������������)����Ŀ, ��Ϊ0
    /*m_pBeginCmdInfo*/         g_cmdInfo_ethread_global_var, // �������ṩ����������
    /*m_pCmdsFunc*/             g_cmdInfo_ethread_global_var_fun, // ָ��ÿ�������ʵ�ִ����׵�ַ, ��ΪNULL

    /*m_pfnRunAddInFn*/         NULL, // ��ΪNULL,������IDEϵͳ��Ӹ�������, ���ṩ���ܻᱻ�Զ���ӵ�IDE�ġ����ߡ��˵���
                                         // �й�AddIn���ܵ�˵��, �����ַ���˵��һ�����ܡ���һ��Ϊ��������
                                         // (����һ��20�ַ�, ���ϣ�����г�ʼλ�ö������Զ����뵽���߲˵�, ��
                                         // ����Ӧ����@��ʼ, ��ʱ����յ�ֵΪ -(nAddInFnIndex + 1) �ĵ���֪ͨ), 
                                         // �ڶ���Ϊ������ϸ����(����һ��60�ַ�), ����������մ�������
    /*m_szzAddInFnInfo*/        NULL, // ������ϸ����(����һ��60�ַ�), ����������մ�����

    /*m_pfnNotify*/             ethread_ProcessNotifyLib_ethread, // ����ΪNULL,��������ͨѶ���ӳ���

    /*m_pfnSuperTemplate*/      NULL, // ģ�� ��ΪNULL
                                         //  �й�SuperTemplate��˵��, �����ַ���˵��һ��SuperTemplate��
                                         //  ��һ��ΪSuperTemplate����(����һ��30�ַ�), �ڶ���Ϊ��ϸ����(����), 
                                         //  ����������մ�������
    /*m_szzSuperTemplateInfo*/  NULL, // ģ�� ��ΪNULL

    /*m_nLibConstCount*/        g_ConstInfo_ethread_global_var_count, // ����Ԥ�ȶ�������г�������
    /*m_pLibConst*/             g_ConstInfo_ethread_global_var, // ����Ԥ�ȶ�������г���

    /*m_szzDependFiles*/        NULL // ��ΪNULL, ����������������Ҫ����������֧���ļ�
};

EXTERN_C PLIB_INFO WINAPI GetNewInf()
{
    return &g_LibInfo_ethread_global_var;
}

#define ETHREAD_DEF_CMDNAME_STR(_index, _szName, _szEgName, _szExplain, _shtCategory, _wState, _dtRetValType, _wReserved, _shtUserLevel, _shtBitmapIndex, _shtBitmapCount, _nArgCount, _pBeginArgInfo) \
    ______E_FNENAME(ETHREAD_NAME(_index, _szEgName)),

// ������, �������̬����ʹ��
static const LPCSTR g_cmdNamesethread[] = { ETHREAD_DEF(ETHREAD_DEF_CMDNAME_STR) };

#endif
EXTERN_C INT WINAPI ethread_ProcessNotifyLib_ethread(INT nMsg, DWORD dwParam1, DWORD dwParam2)
{
    int nRet = NR_OK;
    switch (nMsg)
    {
#ifndef __E_STATIC_LIB
    // ������������ʵ�ֺ����ĵĺ�����������(char*[]), ֧�־�̬����Ķ�̬����봦��
    case NL_GET_CMD_FUNC_NAMES:
    {
        return (INT)g_cmdNamesethread;
    }
    // ���ش���ϵͳ֪ͨ�ĺ�������(PFN_NOTIFY_LIB��������), ֧�־�̬����Ķ�̬����봦��
    case NL_GET_NOTIFY_LIB_FUNC_NAME:
    {
        return (INT)"ethread_ProcessNotifyLib_ethread";
    }
    // ���ؾ�̬����������������̬���ļ����б�(��ʽΪ\0�ָ����ı�,��β����\0), ֧�־�̬����Ķ�̬����봦��
    // kernel32.lib user32.lib gdi32.lib �ȳ��õ�ϵͳ�ⲻ��Ҫ���ڴ��б���
    // ����NULL��NR_ERR��ʾ��ָ�������ļ�
    case NL_GET_DEPENDENT_LIBS:
    {
        return (INT)"\0\0";
    }
#endif
    // ��֪��֪ͨϵͳ�õĺ���ָ��,��װ��֧�ֿ�ǰ֪ͨ,�����ж��,��֪ͨ��ֵӦ�ø���ǰ����֪ͨ��ֵ,���Է���ֵ
    // ��ɽ��˺���ָ���¼�����Ա�����Ҫʱʹ����֪ͨ��Ϣ��ϵͳ
    // dwParam1: (PFN_NOTIFY_SYS)
    case NL_SYS_NOTIFY_FUNCTION:
    {
        // �ѳ�ʼ�����¼�ת���� fnshare.cpp ��ִ��, ��fnshare��ĺ�������ʹ��
        ProcessNotifyLib(nMsg, dwParam1, dwParam2);
        break;
    }
    // ֪֧ͨ�ֿ��ͷ���Դ׼���˳����ͷ�ָ���ĸ�������
    case NL_FREE_LIB_DATA:
    {
        break;
    }
    // ��֧�ֿ���IDE�б��û�ȡ��ѡ����ͱ�֪ͨ
    case NL_UNLOAD_FROM_IDE:
    {
        break;
    }
    // ����NR_DELAY_FREE��ʾ��֧�ֿ���Ҫ���ӳ��ͷ�
    case NR_DELAY_FREE:
    {
        break;
    }
    // ��IDE�Ѿ�׼���ý����û�������,��֪ͨ�����͵����о���LBS_IDE_PLUGIN��־��֧�ֿ�
    // ע��: ���û���֧�ֿ����öԻ�����ȡ����ѡ��֧�ֿ��ѡ���µ�֧�ֿ��,��֪ͨ�ᱻ�ٴη���,֧�ֿ�����Ҫ����ô������
    case NL_IDE_READY:
    {
        break;
    }
    // ���û���IDE�е�������Ҽ����������˵�ǰ,��֪ͨ�����͵����о���LBS_IDE_PLUGIN��־��֧�ֿ�
    // dwParam1: �������˵����(HMENU)
    // dwParam2: �������˵���������������(e.exe)�е���ԴID
    case NL_RIGHT_POPUP_MENU_SHOW:
    {
        break;
    }
    // ��IDE׼����ʾ�¼����Ա�Ի���֮ǰ,��֪ͨ�����͵����о���LBS_IDE_PLUGIN��־��֧�ֿ�
    // ����1��ʾ��֪ͨ��������IDE���ݷ������������ָ�����³�Ա,����2��ʾ��֪ͨ��������IDE��ֹ����³�Ա,����0��ʾδ������
    // dwParam1: �ṩ�������Ա������(ADD_NEW_ELEMENT_TYPE)
    // dwParam2:
    //   ���dwParam1ΪNAN_NEW_VAR,��Ϊһ��ָ��RES_NEW_VAR_INFO��ָ��;
    //   ���dwParam1ΪNAN_NEW_SUB,��Ϊһ��ָ���������ӳ������Ƶ��ı�ָ��;
    //   ���dwParam1ΪNAN_NEW_CONST_RES,��Ϊһ��ָ��RES_NEW_CONST_RES_INFO��ָ��
    case NL_ADD_NEW_ELEMENT:
    {
        break;
    }
    default:
        nRet = NR_ERR;
        break;
    }
    return nRet;
}



PTHREAD_ARG_STRUCT _thread_GetArgs(INT nArgCount, PMDATA_INF pArgInf, int argStart, LPVOID pfn)
{
    const int maxArgCount = nArgCount - argStart;
    if ( maxArgCount > 256 )
        return 0; // ��һ������, ���֧��256������, һ���˲����õ���ô�����, ����õ���ô�����Ӧ�ô�������

    PTHREAD_ARG_STRUCT ret = new THREAD_ARG_STRUCT;
    ret->arr = 0;
    ret->count = 0;
    ret->pfn = pfn;
    ret->hEvent = 0;
    ret->node = 0;
    if ( maxArgCount == 0 ) return ret;
    
    
    // ������ÿ������ռ����������, ��������������*2����ȫ�����
    ret->arr = new int[maxArgCount * 2];

    // �����ڴ�, �ǰ�����ڵ���ڴ�ṹ���з����, ǰ4���ֽ���ָ����һ���ڵ��ָ��, �����ǿ���������
    auto pfn_copyData = [&](LPCVOID pData, int size) -> int
    {
        if ( !pData || size <= 0 )
            return 0;
        int newSize = size + sizeof(PLIST_NODE);
        LPVOID ptr = malloc(newSize);
        if ( !ptr )
            throw std::bad_alloc{}; // �����ڴ�ʧ��
        PLIST_NODE node = (PLIST_NODE)ptr;
        node->next = ret->node; // �¼ӵĽڵ�ֱ�ӷŵ�����ͷ
        ret->node = node;       // ��¼����ͷ
        memcpy(&node->data, pData, size);
        return (int)&node->data;
    };
    for ( int i = 0; i < maxArgCount; i++ )
    {
        MDATA_INF& arg = pArgInf[i + argStart];

        switch ( arg.m_dtDataType )
        {
        case SDT_BYTE:          // �ֽ�
        case SDT_SHORT:         // ������
        case SDT_INT:           // ����
        case SDT_SUB_PTR:       // �ӳ���ָ��
        case SDT_FLOAT:         // С��
        case SDT_BOOL:          // �߼�
            ret->arr[ret->count++] = arg.m_int;
            break;
        case SDT_TEXT:          // �ı�
        {
            int size = ( arg.m_pText && arg.m_pText[0] ) ? (int)strlen(arg.m_pText) : 0;
            ret->arr[ret->count++] = pfn_copyData(arg.m_pText, size + 1);
            break;
        }
        case SDT_INT64:         // ������
        case SDT_DOUBLE:        // ˫����С��
        case SDT_DATE_TIME:     // ����ʱ��
        {
            LPINT p = &arg.m_int;
            ret->arr[ret->count++] = *p++;  // ������, ˫����С��, ����ʱ�䶼��8���ֽ�, ��������������ʾ
            ret->arr[ret->count++] = *p++;
            break;
        }
        case SDT_BIN:           // �ֽڼ�
        {
            int size = 0;
            LPBYTE p = GetAryElementInf(arg.m_pAryData, &size);

            // �ֽڼ�ǰ���ǹ̶�8���ֽ�, ֱ��-8
            const int binSize = 8;
            ret->arr[ret->count++] = pfn_copyData(p - binSize, size + binSize);
            break;
        }
        default:                // �����������;��˳�ѭ��, �ղ���Ҳ��������������
            i = maxArgCount;    // ֱ�Ӹ�ѭ��������ֵΪһ������ѭ��������
            break;
        }
    }
    return ret;
}
