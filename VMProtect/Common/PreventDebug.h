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

