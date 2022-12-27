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
ETHREAD_DEF(ETHREAD_DEF_CMD) // 所有实现命令的声明


#define __query(_num, _n)    (   ( (_num) & (_n) ) == (_n)   )


// 封装这么一个函数是为了在易语言那边回调函数参数不对的时候不至于会堆栈不平衡
inline __declspec( naked ) int _threadpool_call(LPVOID pfn, LPINT arr, int count)
{
    __asm
    {
        push ebp                // 函数头
        mov ebp, esp

        mov ecx, count          // 参数数量
        mov eax, arr            // 参数数组

    _loop:
        dec ecx
        cmp ecx, 0
        jl  _call               // ecx小于0就跳转到call那里
        push[eax + ecx * 4]     // ecx是倒序减小, 需要从最后一个参数push
        jmp _loop               // 跳到循环开始的位置继续循环

    _call :
        call pfn                // 易语言这边的子程序

        mov esp, ebp
        pop ebp
        ret                     // __cdecl调用约定是外面平栈
    }
}

typedef struct THREAD_ARG_STRUCT
{
    int     count;  // 传递到易语言那边的参数数量
    LPINT   arr;    // 传递到易语言那边的参数数组
    LPVOID  pfn;    // 易语言那边的子程序
    HANDLE  hEvent; // 事件对象, 用来判断是否等待线程执行
}* PTHREAD_ARG_STRUCT;

inline PTHREAD_ARG_STRUCT _thread_GetArgs(INT nArgCount, PMDATA_INF pArgInf, int argStart, LPVOID pfn)
{
    const int maxArgCount = nArgCount - argStart;
    if ( maxArgCount > 256 )
        return 0; // 做一个限制, 最大支持256个参数, 一般人不会用到这么多参数, 如果用到这么多参数应该传递数组

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
