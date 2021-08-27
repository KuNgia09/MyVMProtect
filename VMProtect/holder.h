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

#define VMSTARTADDR  0x200          //����ʵcpu�л���VM�����
#define OUTOFORDERADDR 0x5000       //��ʼ������ָ�����ݵ�ַ���������ݸ�Stub.dll��̬���
#define CODETABLEADDR  0x5500        //esi��ת���ŵ���Ե�ַ
#define DISPATCHERTABLEADDR 0x1000  //���ݱ���Ե�ַ
#define VCHECKESPADDR 0x2000        //vCheckESP�����ĵ�ַ
#define VRETNOTADDR  0x2500         //vRetnNOT������ַ
#define VRESUMESTARTADDR 0x3000     //vResumeStart�����ĵ�ַ
#define ESISTATEADDR 0x3500         //����esi״̬�ĵ�ַ
#define EBPSTATEADDR 0x3600         //����ebp״̬�ĵ�ַ

#define BEA_ENGINE_STATIC  // ָ��ʹ�þ�̬Lib��
#define BEA_USE_STDCALL    // ָ��ʹ��stdcall����Լ��

//���������
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

//�������
#ifdef _WIN64
#pragma comment (lib,"XEDParse_x64.lib")
#else
#pragma comment (lib,"XEDParse_x86.lib")
#endif // _WIN64

// ��ֹ�������
#pragma comment(linker, "/NODEFAULTLIB:\"crt.lib\"")
#pragma comment(linker, "/NODEFAULTLIB:\"crt64.lib\"")

//��ת��
struct JMPTABLE
{

    int index = -1;
    DWORD origin = 0;;
    DWORD addr = 0;
};

//���ݱ�ṹ��
struct DATATABLE
{
    DWORD data = 0;
    DWORD originaddr = 0;
    DWORD recodeOaddr = 0;
};

struct VMPStruct
{
    DWORD vmp1_startaddr;//vmp1�ڵ��׵�ַ
    DWORD originaddr;//ָ���ԭʼ��ַ(������������ڴ�������ַ)
    DWORD jmptbladdr;//��ת���ַ
    DWORD datatbladdr;//���ݱ��ַ
    DWORD dispatcheraddr;//��������ַ
    DWORD startaddr;//��Ҫ������ָ��ĵ�ַ(�Զ����Ʒ�ʽ��ȡ���ڴ�)
    int length;//�����ĳ���
    DWORD instbuffaddr;//���ָ���ʵ�ʻ���
    DWORD vcheckespaddr;//vCheckESP�����ĵ�ַ
    DWORD vmstartaddr;//�������ָ����vmp���ڽڿ�ʼ�ĵ�ַ
    DWORD vretnotaddr;//�˳������,ִ�в���ģ��ָ��
    DWORD vresumestartaddr;//ִ�в���ģ��ָ���,�ٴν��������
    DWORD esistateaddr;//��vmp������һ���ڴ汣��esi��״̬
    DWORD ebpstateaddr;//��vmp������һ���ڴ汣��ebp��״̬
    DWORD resume_esiandebpaddr;//��¼�ָ�ebp��esi��vm������ݵĵ�ַ

};

//����ȫ�ֱ���
extern HINSTANCE g_hInstance;
extern LPVOID g_pMainDialog;
extern LPVOID g_pTabDialog;

extern int VMStart(char* path_in,vector<DWORD>temp_addr);
extern VMPStruct g_vmstruct;