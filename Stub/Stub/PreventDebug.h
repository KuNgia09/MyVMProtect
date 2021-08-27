#pragma once
#include <Windows.h>


//�����Ե���
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
	
	fnGetTickCount		g_fnGetTickCount;//��ȡ����ʱ��
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
	//��ȡkernel32.dll��ģ���ַ
	UCHAR* GetKernel32Addr();

	//��ȡGetProcessAddress�����ĵ�ַ
	UCHAR* MyGetProcessAddress();

	//��ʼ����Ϣ,�õ�����API�ĵ�ַ
	void InitInformation();

	//�ڴ濽��
	void pmemcpy(void* _Dst, const void* _Res, DWORD _Size);
public:

	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		XorMachineCode - �������(��CPU���к���������ڽ������)
	*����*  Returns:		��
	*����*  Parameter_1:	cpuId��CPU���к�
	*����*  Parameter_2:	ulCodeOfBase,�������OEP
	*����*  Parameter_3:
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	void XorMachineCode(ULONGLONG cpuId, ULONG_PTR ulCodeOfBase);
	
	//ȡCPU���к�
	ULONGLONG GetCPUID();

	//�����Ƿ񱻵���(�����Է���1��û�����Է���0)
	ULONG_PTR IsBebug();

	//�������Ƿ񱻵���(���ֱ����ԣ�����ֱ�ӿ���)
	BOOL Check_ZwSetInformationObject();

	//�������
	bool GetProcessIdByName(TCHAR* szProcessName);
};

