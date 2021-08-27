#pragma once
#include <Windows.h>


//反调试的类
class PreventDebug
{
public:
	typedef ULONG_PTR(WINAPI* fnGetProcAddress)(_In_ HMODULE hModule, _In_ LPCSTR lpProcName);
	typedef HMODULE(WINAPI* fnLoadLibraryA)(_In_ LPCSTR lpLibFileName);
	typedef int(WINAPI* fnMessageBox)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
	typedef HMODULE(WINAPI* fnGetModuleHandleA)(_In_opt_ LPCSTR lpModuleName);
	typedef BOOL(WINAPI* fnVirtualProtect)(_In_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD flNewProtect, _Out_ PDWORD lpflOldProtect);
	typedef void(WINAPI* fnExitProcess)(_In_ UINT uExitCode);
	typedef LPVOID(WINAPI* fnVirtualAlloc)(_In_opt_ LPVOID lpAddress, _In_ SIZE_T dwSize, _In_ DWORD flAllocationType, _In_ DWORD flProtect);
	typedef LPVOID (WINAPI* fnHeapAlloc)(_In_ HANDLE hHeap,_In_ DWORD dwFlags,	_In_ SIZE_T dwBytes	);
	typedef BOOL (WINAPI*fnHeapFree)(_Inout_ HANDLE hHeap,	_In_ DWORD dwFlags,	__drv_freesMem(Mem) _Frees_ptr_opt_ LPVOID lpMem);
	typedef HANDLE	(WINAPI* fnGetProcessHeap)(VOID);
	typedef DWORD	(WINAPI*fnGetTickCount)(VOID);
	typedef int(__stdcall* fnLSTRCMP)(_In_ LPCSTR lpString1, _In_ LPCSTR lpString2);
	
	fnLSTRCMP		g_fnplstrcmpi;
	
	fnGetTickCount		g_fnGetTickCount;//获取开机时间
	fnGetProcessHeap	g_fnGetProcessHeap = NULL;
	fnHeapFree			g_HeapFree = NULL;
	fnHeapAlloc			g_HeapAlloc = NULL;
	fnGetProcAddress	g_pfnGetProcAddress = NULL;
	fnLoadLibraryA		g_pfnLoadLibraryA = NULL;
	fnGetModuleHandleA	g_pfnGetModuleHandleA = NULL;
	fnVirtualProtect	g_pfnVirtualProtect = NULL;
	fnVirtualAlloc		g_pfnVirtualAlloc = NULL;
	fnExitProcess		g_pfnExitProcess = NULL;
	fnMessageBox		g_pfnMessageBox = NULL;
public:
	//获取kernel32.dll的模块基址
	UCHAR* GetKernel32Addr();

	//获取GetProcessAddress函数的地址
	UCHAR* MyGetProcessAddress();

	//初始化信息,得到常用API的地址
	void InitInformation();

	//内存拷贝
	void pmemcpy(void* _Dst, const void* _Res, DWORD _Size);
public:

	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		XorMachineCode - 机器码绑定(将CPU序列号主程序入口进行亦或)
	*※※*  Returns:		无
	*※※*  Parameter_1:	cpuId，CPU序列号
	*※※*  Parameter_2:	ulCodeOfBase,主程序的OEP
	*※※*  Parameter_3:
	*※※*  Parameter_4:
	*※※*	Parameter_5:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	void XorMachineCode(ULONGLONG cpuId, ULONG_PTR ulCodeOfBase);
	
	//取CPU序列号
	ULONGLONG GetCPUID();

	//程序是否被调试(被调试返回1，没被调试返回0)
	ULONG_PTR IsBebug();

	//检查程序是否被调试(发现被调试，程序直接卡死)
	BOOL Check_ZwSetInformationObject();

	//反虚拟机
	bool GetProcessIdByName(TCHAR* szProcessName);
};

