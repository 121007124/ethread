#ifndef __E_STATIC_LIB
//#define    CT_NULL 0
//#define    CT_NUM  1      // value sample: 3.1415926
//#define    CT_BOOL 2      // value sample: 1
//#define    CT_TEXT 3      // value sample: "abc"
//LPCSTR m_szName;          // 常量名
//LPCSTR m_szEgName;        // 英文名
//LPCSTR m_szExplain;       // 说明
//SHORT  m_shtLayout;       // 必须为 1
//SHORT  m_shtType;         // 常量类型, CT_NULL=空常量, CT_NUM=数值型,double保存值, CT_BOOL=逻辑型, CT_TEXT=文本型
//LPCSTR m_szText;          // CT_TEXT
//DOUBLE m_dbValue;         // CT_NUM、CT_BOOL
#include "include_ethread_header.h"

LIB_CONST_INFO g_ConstInfo_ethread_global_var[] =
{
    //#define    CT_NULL 0
    //#define    CT_NUM  1      // value sample: 3.1415926
    //#define    CT_BOOL 2      // value sample: 1
    //#define    CT_TEXT 3      // value sample: "abc"
    //LPCSTR m_szName;          // 常量名
    //LPCSTR m_szEgName;        // 英文名
    //LPCSTR m_szExplain;       // 说明
    //SHORT  m_shtLayout;       // 必须为 1
    //SHORT  m_shtType;         // 常量类型, CT_NULL=空常量, CT_NUM=数值型,double保存值, CT_BOOL=逻辑型, CT_TEXT=文本型
    //LPCSTR m_szText;          // CT_TEXT
    //DOUBLE m_dbValue;         // CT_NUM、CT_BOOL
    /*000*/ { "启动线程_默认", "", "默认方式, 创建线程后就返回", 1, CT_NUM, NULL, 0 },
    /*001*/ { "启动线程_启动后挂起", NULL, "有这个标志时, 创建线程后会挂起线程, 需要调用 唤醒线程() 才能让线程工作, 有这个标志时忽略其他标志, 创建后就返回", 1, CT_NUM, NULL, 1 },
    /*002*/ { "启动线程_等待执行", NULL, "有这个标志时, 创建线程后会等待线程开始工作后才返回", 1, CT_NUM, NULL, 2 },
    /*003*/ { "启动线程_等待执行完毕", NULL, "有这个标志时, 创建线程后会等待线程工作完毕后才返回", 1, CT_NUM, NULL, 4 },
    /*004*/ { "启动线程_处理事件", NULL, "有这个标志时, 等待线程执行完毕前会处理窗口消息, 如果没有这个标志", 1, CT_NUM, NULL, 8 },
    /*005*/ { "线程池销毁模式_等待任务", NULL, "等待所有任务执行完毕后返回", 1, CT_NUM, NULL, 0 },
    /*006*/ { "线程池销毁模式_自生自灭", NULL, "销毁后立即返回, 让任务线程自生自灭, 线程池销毁后任务还会执行, 直到任务自动结束", 1, CT_NUM, NULL, 1 },
    /*007*/ { "线程池销毁模式_等待任务并处理消息", NULL, "等待任务结束, 并处理窗口消息, 如果等待期间关闭了窗口, 那么进程可能不能正常退出", 1, CT_NUM, NULL, 4 },
};
int g_ConstInfo_ethread_global_var_count = sizeof(g_ConstInfo_ethread_global_var) / sizeof(g_ConstInfo_ethread_global_var[0]);

#endif
