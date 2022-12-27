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

typedef struct THREAD_ARG_STRUCT
{
    int     count;  // ���ݵ��������ǱߵĲ�������
    LPINT   arr;    // ���ݵ��������ǱߵĲ�������
    LPVOID  pfn;    // �������Ǳߵ��ӳ���
    HANDLE  hEvent; // �¼�����, �����ж��Ƿ�ȴ��߳�ִ��
}* PTHREAD_ARG_STRUCT;

inline PTHREAD_ARG_STRUCT _thread_GetArgs(INT nArgCount, PMDATA_INF pArgInf, int argStart, LPVOID pfn)
{
    const int maxArgCount = nArgCount - argStart;
    if ( maxArgCount > 256 )
        return 0; // ��һ������, ���֧��256������, һ���˲����õ���ô�����, ����õ���ô�����Ӧ�ô�������

    PTHREAD_ARG_STRUCT ret = new THREAD_ARG_STRUCT;
    ret->arr = 0;
    ret->count = 0;
    ret->pfn = pfn;
    ret->hEvent = 0;
    if ( maxArgCount > 0 )
    {
        ret->arr = new int[maxArgCount];
        for ( int i = 0; i < maxArgCount; i++ )
        {
            MDATA_INF& data = pArgInf[i + argStart];
            if ( data.m_dtDataType == _SDT_NULL )
                break;

            ret->arr[i] = data.m_int;
            ret->count++;
        }
    }
    return ret;
}

#endif