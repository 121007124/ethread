#ifndef __INCLUDE_ETHREAD_HEADER_H__
#define __INCLUDE_ETHREAD_HEADER_H__
#include "elib/lib2.h"
#include "elib/lang.h"
#include "elib/krnllib.h"

#include "ethread_cmd_typedef.h"

#define ETHREAD_EXTERN_C EXTERN_C

#ifndef __E_STATIC_LIB
extern LIB_CONST_INFO g_ConstInfo_ethread_global_var[];
extern int g_ConstInfo_ethread_global_var_count;
extern CMD_INFO g_cmdInfo_ethread_global_var[];
extern PFN_EXECUTE_CMD g_cmdInfo_ethread_global_var_fun[];
extern int g_cmdInfo_ethread_global_var_count;
extern ARG_INFO g_argumentInfo_ethread_global_var[];
extern LIB_DATA_TYPE_INFO g_DataType_ethread_global_var[];
extern int g_DataType_ethread_global_var_count;

#define SDT_TYPE_POOLINFO           MAKELONG (2, 0) // �̳߳���Ϣ�ṹ

#endif

#define ETHREAD_DEF_CMD(_index, _szName, _szEgName, _szExplain, _shtCategory, _wState, _dtRetValType, _wReserved, _shtUserLevel, _shtBitmapIndex, _shtBitmapCount, _nArgCount, _pBeginArgInfo) \
    EXTERN_C void ETHREAD_NAME(_index, _szEgName)(PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf);
ETHREAD_DEF(ETHREAD_DEF_CMD) // ����ʵ�����������


#define __query(_num, _n)    (   ( (_num) & (_n) ) == (_n)   )


// ��װ��ôһ��������Ϊ�����������Ǳ߻ص������������Ե�ʱ�����ڻ��ջ��ƽ��
inline __declspec( naked ) int _threadpool_call(LPVOID pfn, LPINT arr, int count)
{
    __asm
    {
        push ebp                // ����ͷ
        mov ebp, esp

        mov ecx, count          // ��������
        mov eax, arr            // ��������

    _loop:
        dec ecx
        cmp ecx, 0
        jl  _call               // ecxС��0����ת��call����
        push[eax + ecx * 4]     // ecx�ǵ����С, ��Ҫ�����һ������push
        jmp _loop               // ����ѭ����ʼ��λ�ü���ѭ��

    _call :
        call pfn                // ��������ߵ��ӳ���

        mov esp, ebp
        pop ebp
        ret                     // __cdecl����Լ��������ƽջ
    }
}

typedef struct LIST_NODE
{
    struct LIST_NODE* next;
    LPVOID data;
} *PLIST_NODE;

typedef struct THREAD_ARG_STRUCT
{
    int         count;  // ���ݵ��������ǱߵĲ�������
    LPINT       arr;    // ���ݵ��������ǱߵĲ�������
    LPVOID      pfn;    // �������Ǳߵ��ӳ���
    HANDLE      hEvent; // �¼�����, �����ж��Ƿ�ȴ��߳�ִ��
    PLIST_NODE  node;   // ����ͷ�ڵ�, ����Ҫ�ͷŵ��ڴ�ŵ���������, �������ͷ�
    
    THREAD_ARG_STRUCT()
    {
        memset(this, 0, sizeof(*this));
    }
    ~THREAD_ARG_STRUCT()
    {
        if ( arr )
        {
            delete[] arr;
            arr = NULL;
        }
        while ( node )
        {
            // �ͷ�����, ����Ľṹ�� ǰ4���ֽڼ�¼��һ������ڵ��ַ, ���������ݾ���ʵ�ʴ�ŵ�����, malloc������ڴ�
            PLIST_NODE next = node->next;
            free(node);
            node = next;
        }
    }
}* PTHREAD_ARG_STRUCT;

PTHREAD_ARG_STRUCT _thread_GetArgs(INT nArgCount, PMDATA_INF pArgInf, int argStart, LPVOID pfn);

#endif
