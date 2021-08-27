#pragma once
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <CommCtrl.h>
#include "resource.h"
#include <GdiPlus.h> 
#pragma comment(lib, "gdiplus.lib") 


#include <time.h>
#include <vector>
#include <algorithm>
#include "disasm.h"
#include "XEDParse.h"
#include"Common/header.h"


using namespace Gdiplus;
using namespace std;

#define STRINGLENGTH 0x38
#define BSTRINGLENGTH 0x1000

#define VMSTARTADDR  0x200          //从真实cpu切换到VM虚拟机
#define OUTOFORDERADDR 0x5000       //开始的垃圾指令数据地址，用来传递给Stub.dll动态库的
#define CODETABLEADDR  0x5500        //esi跳转表存放的相对地址
#define DISPATCHERTABLEADDR 0x1000  //数据表相对地址
#define VCHECKESPADDR 0x2000        //vCheckESP函数的地址
#define VRETNOTADDR  0x2500         //vRetnNOT函数地址
#define VRESUMESTARTADDR 0x3000     //vResumeStart函数的地址
#define ESISTATEADDR 0x3500         //保存esi状态的地址
#define EBPSTATEADDR 0x3600         //保存ebp状态的地址

#define BEA_ENGINE_STATIC  // 指明使用静态Lib库
#define BEA_USE_STDCALL    // 指明使用stdcall调用约定

//反汇编引擎
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN64)
#include "beaengine-win64/headers/BeaEngine.h"
#pragma comment(lib, "beaengine-win64\\Win64\\Lib\\BeaEngine64.lib")
#else
#include "beaengine-win32/headers/BeaEngine.h"
#pragma comment(lib, "beaengine-win32\\Win32\\Lib\\BeaEngine.lib")
#endif

#ifdef __cplusplus
};
#endif

//汇编引擎
#ifdef _WIN64
#pragma comment (lib,"XEDParse_x64.lib")
#else
#pragma comment (lib,"XEDParse_x86.lib")
#endif // _WIN64

// 防止编译错误。
#pragma comment(linker, "/NODEFAULTLIB:\"crt.lib\"")
#pragma comment(linker, "/NODEFAULTLIB:\"crt64.lib\"")

//跳转表
struct JMPTABLE
{

    int index = -1;
    DWORD origin = 0;;
    DWORD addr = 0;
};

//数据表结构体
struct DATATABLE
{
    DWORD data = 0;
    DWORD originaddr = 0;
    DWORD recodeOaddr = 0;
};

struct VMPStruct
{
    DWORD vmp1_startaddr;//vmp1节的首地址
    DWORD originaddr;//指令的原始地址(即点击启动在内存的虚拟地址)
    DWORD jmptbladdr;//跳转表地址
    DWORD datatbladdr;//数据表地址
    DWORD dispatcheraddr;//调度器地址
    DWORD startaddr;//需要保护的指令的地址(以二进制方式读取到内存)
    int length;//保护的长度
    DWORD instbuffaddr;//存放指令的实际缓存
    DWORD vcheckespaddr;//vCheckESP函数的地址
    DWORD vmstartaddr;//保护后的指令在vmp所在节开始的地址
    DWORD vretnotaddr;//退出虚拟机,执行不可模拟指令
    DWORD vresumestartaddr;//执行不可模拟指令后,再次进入虚拟机
    DWORD esistateaddr;//在vmp节申请一块内存保存esi的状态
    DWORD ebpstateaddr;//在vmp节申请一块内存保存ebp的状态
    DWORD resume_esiandebpaddr;//记录恢复ebp和esi再vm里的数据的地址

};

//申明全局变量
extern HINSTANCE g_hInstance;
extern LPVOID g_pMainDialog;
extern LPVOID g_pTabDialog;

extern int VMStart(char* path_in,vector<DWORD>temp_addr);
extern VMPStruct g_vmstruct;