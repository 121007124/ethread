#ifndef __E_STATIC_LIB
#include "include_ethread_header.h"


static INT s_dtCmdIndex_ethread_ThreadPool_static_var_00[] =
{
    8,9,10,11,13,12,14,15,16,17,18,19,20,
};

static const int s_dtCmdIndex_ethread_ThreadPool_static_var_count_00 = sizeof(s_dtCmdIndex_ethread_ThreadPool_static_var_00) / sizeof(s_dtCmdIndex_ethread_ThreadPool_static_var_00[0]);


//1=中文名字,2=英文名字,3=详细解释,4=命令数量,5=索引值,6=标志 LDT_
//类型为窗口或菜单组件时有效 7=资源ID,8=事件数量,9=组件事件数组,10=属性数  11=属性数组 12=组件交互子程序
//不为窗口、菜单组件或为枚举数据类型时才有效 13=成员数量,14=成员数据数组
LIB_DATA_TYPE_INFO g_DataType_ethread_global_var[] =
{
/*000*/ { "线程池", "ThreadPool", "线程池, 设计理念是: 你有一次使用到了这么多条线程, 那就会使用到第二次, 所以不会自动释放空闲的线程",
            s_dtCmdIndex_ethread_ThreadPool_static_var_count_00, s_dtCmdIndex_ethread_ThreadPool_static_var_00, _DT_OS(__OS_WIN),
            0, 0, 0,
            0, 0,
            0,
            NULL, NULL
        },

};

int g_DataType_ethread_global_var_count = sizeof(g_DataType_ethread_global_var) / sizeof(g_DataType_ethread_global_var[0]);

#endif



