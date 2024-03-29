#if !defined(__E_STATIC_LIB)
#include "include_ethread_header.h"

//TODO 静态库需要的部分,会记录所有的函数名到数组里,静态编译的时候需要取回命令名
static ARG_INFO g_argumentInfo_ethread_global_var[] = 
{
    // 1=参数名称, 2=参数详细解释, 3=指定图像索引,从1开始,0表示无, 4=图像数目(用作动画)
    // 5=参数类型, 6=参数默认值,   7=参数标志 AS_ 开头常量
    // AS_HAS_DEFAULT_VALUE         有默认值,倒数第二个参数是默认值
    // AS_DEFAULT_VALUE_IS_EMPTY    默认值为空,有可空标志
    // AS_RECEIVE_VAR               只能传递变量,相当于传引用,传递过来的肯定不是数组
    // AS_RECEIVE_VAR_ARRAY         传递过来的肯定是数组变量引用
    // AS_RECEIVE_VAR_OR_ARRAY      传递变量或者数组变量引用
    // AS_RECEIVE_ARRAY_DATA        传递数组
    // AS_RECEIVE_ALL_TYPE_DATA     传递数组/非数组
    // AS_RECEIVE_VAR_OR_OTHER      可以传递 变量/数据/返回值数据

    /*000*/ {"欲执行的子程序", "本参数提供创建线程时欲启动的子程序，根据是否需要传递参数数据，该子程序必须没有或具有一个整数型参数，否则将出错", 0, 0, _SDT_ALL, 0, NULL},
    /*001*/ {"参数数据", "本参数提供线程被启动时传递到欲启动子程序的整数数据。如果本参数未被省略，此时被启动子程序必须接收一个整数型参数，否则将出错", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*002*/ {"线程句柄", "如果提供了本参数，将向参数变量写入线程句柄（Windows下为HANDLE，Linux下为pthread_t），请在适当的时机关闭该句柄。如果不提供本参数接收线程句柄，内部将自动处理线程句柄。", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY | AS_RECEIVE_VAR},

    /*003*/ {"进入许可证", "本参数应提供由“创建进入许可证”命令所创建并返回的数值", 0, 0, SDT_INT, 0, NULL},

    /*004*/ {"线程句柄", "可通过“启动线程”的第三个参数获取线程句柄。", 0, 0, SDT_INT, 0, NULL},
    /*005*/ {"等待时间", "指定等待时间，单位为毫秒。如果为0或负数，或被省略，表示不限定等待时间。", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},

    /*006*/ {"最小线程数", "线程池最小线程数, 小于等于0则默认为5, 创建的时候就分配的线程数", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*007*/ {"最大线程数", "线程池最大线程数, 小于等于0则默认为10, 最大32767条线程, 一般用不到这么多线程, 如果用到这么多线程, 那应该考虑优化程序了", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},

    /*008*/ {"欲执行的子程序", "本参数提供创建线程时欲启动的子程序, 只支持传递整数型的子程序地址或者子程序指针类型, 其他类型直接调用失败", 0, 0, _SDT_ALL, 0, NULL},
    /*009*/ {"线程参数", "传递到线程里的参数, 支持11个易语言的基础数据类型, 遇到空参数或者不是基础数据类型则视为结束传参", 0, 0, _SDT_ALL, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    
    /*010*/ {"销毁模式", "1=让任务线程自生自灭, 2=挂起所有任务, 永远不唤醒, 其他=等待任务结束", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},

    /*011*/ {"欲执行的子程序", "本参数提供创建线程时欲启动的子程序, 只支持传递整数型的子程序地址或者子程序指针类型, 其他类型直接调用失败", 0, 0, _SDT_ALL, 0, NULL},
    /*012*/ {"创建标志", "请参考 #启动线程_ 开头常量, 默认为 #启动线程_默认, 多个标志请使用 位或(值1, 值2, 值3, ...)", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*013*/ {"线程句柄", "如果提供了本参数，将向参数变量写入线程句柄（Windows下为HANDLE，Linux下为pthread_t），请在适当的时机关闭该句柄。如果不提供本参数接收线程句柄，内部将自动处理线程句柄。", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY | AS_RECEIVE_VAR},
    /*014*/ {"线程参数", "传递到线程里的参数, 支持11个易语言的基础数据类型, 遇到空参数或者不是基础数据类型则视为结束传参", 0, 0, _SDT_ALL, 0, AS_DEFAULT_VALUE_IS_EMPTY},

    /*015*/ {"线程句柄", "可通过\"启动线程\" 或者 \"启动线程Ex\" 或者 \"打开线程\" 获得的线程句柄", 0, 0, SDT_INT, 0, NULL},
    /*016*/ {"等待时间", "指定等待时间, 单位为毫秒, 如果为负数, 表示不限定等待时间, 如果为0则立马返回, 默认为0", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*017*/ {"处理事件", "是否处理窗口的消息事件, 如果是窗口线程调用, 可以设置为真, 这样窗口就不会卡死, 默认为假", 0, 0, SDT_BOOL, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    
    /*018*/ {"线程ID", "启动线程Ex/创建线程消息循环 返回的值", 0, 0, SDT_INT, 0, 0},
    
    /*019*/ {"消息回调", "接收线程消息回调的函数, 整数型 线程消息回调(整数型 消息值, 整数型 参数1, 整数型 参数2), 返回值保留给后续版本升级使用", 0, 0, SDT_SUB_PTR, 0, 0},
    
    /*020*/ {"线程ID", "创建线程消息循环 返回的值", 0, 0, SDT_INT, 0, 0},
    /*021*/ {"消息值", "消息值的限制请查看MSDN, 大于65535的消息会投递失败", 0, 0, SDT_INT, 0, 0},
    /*022*/ {"参数1", "投递到消息队列里的参数1", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*023*/ {"参数2", "投递到消息队列里的参数2", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    
    /*024*/ {"接收信息变量", "接收线程池信息的结构变量", 0, 0, SDT_TYPE_POOLINFO, 0, AS_RECEIVE_VAR},
    
    /*025*/ {"参数指针", "线程池初始化时填写的回调函数的第二个参数, 或者 多线程_生成无限参数() 的返回值", 0, 0, SDT_INT, 0, 0 },
    /*026*/ {"参数", "接收参数, 投递任务/生成无限参数时填写什么参数这里就可以填写什么参数, 这里不会校验参数是否正确, 传递什么类型就按什么类型参考返回\r\n"
            "    类型不正确的时候, 大概率会崩... 所以不确定自己是什么参数类型就不要填写或者写整数型, 整数型在这里是通用型\r\n"
            "    如果投递的任务没有参数, 那就留空, 遇到空参数则视为获取参数结束"
            , 0, 0, _SDT_ALL, 0, AS_RECEIVE_VAR | AS_DEFAULT_VALUE_IS_EMPTY },

    /*027*/ {"最小线程数", "线程池最小线程数, 小于等于0则默认为5, 创建的时候就分配的线程数", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*028*/ {"最大线程数", "线程池最大线程数, 小于等于0则默认为10, 最大32767条线程, 一般用不到这么多线程, 如果用到这么多线程, 那应该考虑优化程序了", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*029*/ {"任务完成回调", "投递的任务执行完毕后会调用这个回调函数, 回调函数原型  无返回值 回调(任务返回值, 投递任务填写的参数指针, 备用参数)\r\n"
            "    这个参数只支持传递 子程序指针/整数型的 子程序指针类型, 传递其他类型视为不使用这个参数\r\n"
            "    如果传递整数型, 并且是个非法的地址, 那么任务结束后程序就会崩溃", 0, 0, _SDT_ALL, 0, AS_DEFAULT_VALUE_IS_EMPTY},

    /*030*/ {"参数", "支持11个易语言的基础数据类型, 遇到空参数或者不是基础数据类型则视为结束传参", 0, 0, _SDT_ALL, 0, 0},
    /*031*/ {"参数指针", "多线程_生成无限参数() 返回的值", 0, 0, _SDT_ALL, 0, 0},

    /*032*/ {"销毁模式", "\r\n"
                        "    1=让任务线程自生自灭, 线程池销毁后任务还会执行, 直到任务自动结束\r\n"
                        //"    2=挂起所有任务, 永远不唤醒, 使用这个标志销毁后这个线程池不可再使用\r\n"
                        //"    3=销毁所有任务, 使用这个标志销毁后这个线程池不可再使用\r\n"
                        "    4=等待任务结束, 并处理窗口消息, 如果等待期间关闭了窗口, 那么进程可能不能正常退出\r\n"
                        "    其他=等待任务结束, 会阻塞线程, 直到所有任务执行完毕后返回, 销毁后可再次使用这个线程池", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},
    /*033*/ {"等待时长", "单位为毫秒, 如果等待超时任务仍然没有返回, 则", 0, 0, SDT_INT, 0, AS_DEFAULT_VALUE_IS_EMPTY},

};

#ifdef _DEBUG    // 这里是为了确认参数序号是否正确, 成员数比最后一个序号大1就是正确
const int dbg_cmd_arg_count__ = sizeof(g_argumentInfo_ethread_global_var) / sizeof(g_argumentInfo_ethread_global_var[0]);
#endif


#define ETHREAD_DEF_CMDINFO(_index, _szName, _szEgName, _szExplain, _shtCategory, _wState, _dtRetValType, _wReserved, _shtUserLevel, _shtBitmapIndex, _shtBitmapCount, _nArgCount, _pBeginArgInfo) \
    { _szName, ______E_FNENAME(_szEgName), _szExplain, _shtCategory, _wState, _dtRetValType, _wReserved, _shtUserLevel, _shtBitmapIndex, _shtBitmapCount, _nArgCount, _pBeginArgInfo },


// 易语言每个命令的定义, 会把信息显示在支持库列表里, 这里每个成员都是详细的描述一个命令的信息
CMD_INFO g_cmdInfo_ethread_global_var[] = 
{
    ETHREAD_DEF(ETHREAD_DEF_CMDINFO)
};

int g_cmdInfo_ethread_global_var_count = sizeof(g_cmdInfo_ethread_global_var) / sizeof(g_cmdInfo_ethread_global_var[0]);

#endif

