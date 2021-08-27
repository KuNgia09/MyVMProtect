#include "PreventDebug.h"

//��ȡkernel32.dll��ģ���ַ
UCHAR* PreventDebug::GetKernel32Addr()
{
	UCHAR* dwKernel32Addr;
#ifdef _WIN64

	_asm
	{//64λ��ȡkernel32.dll��ģ���ַ
		push rax
		mov rax, qword ptr gs : [60h] ;		// peb
		mov rax, [rax + 18h];				// LDR
		mov rax, [rax + 30h];				// InLoadOrderModuleList.Blink,
		mov rax, [rax];						// [_LDR_MODULE.InLoadOrderModuleList].Blink kernelbase.dll
		mov rax, [rax];						// [_LDR_MODULE.InLoadOrderModuleList].Blink kernel32.dll
		mov rax, [rax + 10h];				//[_LDR_MODULE.InLoadOrderModuleList]. BaseAddress
		mov dwKernel32Addr, rax
			pop rax
	}
#else

	__asm
	{//32λ��ȡkernel32.dll��ģ���ַ
		push eax
		mov eax, dword ptr fs : [0x30]   // eax = PEB�ĵ�ַ
		mov eax, [eax + 0x0C]            // eax = ָ��PEB_LDR_DATA�ṹ��ָ��
		mov eax, [eax + 0x1C]            // eax = ģ���ʼ�������ͷָ��InInitializationOrderModuleList
		mov eax, [eax]                   // eax = �б��еĵڶ�����Ŀ
		mov eax, [eax]                   // eax = �б��еĵ�������Ŀ
		mov eax, [eax + 0x08]            // eax = ��ȡ����Kernel32.dll��ַ(Win7�µ�������Ŀ��Kernel32.dll)
		mov dwKernel32Addr, eax
		pop eax
	}
#endif

	return dwKernel32Addr;
}

//��ȡGetProcessAddress�����ĵ�ַ
UCHAR* PreventDebug::MyGetProcessAddress()
{
	UCHAR* dwBase = GetKernel32Addr();
	// 1. ��ȡDOSͷ
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)dwBase;
	// 2. ��ȡNTͷ
	PIMAGE_NT_HEADERS  pNt = (PIMAGE_NT_HEADERS)(dwBase + pDos->e_lfanew);
	// 3. ��ȡ����Ŀ¼��
	PIMAGE_DATA_DIRECTORY pExportDir = pNt->OptionalHeader.DataDirectory;
	pExportDir = &(pExportDir[IMAGE_DIRECTORY_ENTRY_EXPORT]);
	DWORD dwOffset = pExportDir->VirtualAddress;
	// 4. ��ȡ��������Ϣ�ṹ
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(dwBase + dwOffset);
	DWORD dwFunCount = pExport->NumberOfFunctions;
	DWORD dwFunNameCount = pExport->NumberOfNames;
	DWORD dwModOffset = pExport->Name;

	// Get Export Address Table
	PDWORD pEAT = (PDWORD)(dwBase + pExport->AddressOfFunctions);
	// Get Export Name Table
	PDWORD pENT = (PDWORD)(dwBase + pExport->AddressOfNames);
	// Get Export Index Table
	PWORD  pEIT = (PWORD)(dwBase + pExport->AddressOfNameOrdinals);

	for (DWORD dwOrdinal = 0; dwOrdinal < dwFunCount; dwOrdinal++)
	{
		if (!pEAT[dwOrdinal]) // Export Address offset
			continue;

		// 1. ��ȡ���
		DWORD dwID = pExport->Base + dwOrdinal;
		// 2. ��ȡ����������ַ
		ULONG_PTR dwFunAddrOffset = pEAT[dwOrdinal];

		for (DWORD dwIndex = 0; dwIndex < dwFunNameCount; dwIndex++)
		{
			// ����ű��в��Һ��������
			if (pEIT[dwIndex] == dwOrdinal)
			{
				// ��������������������Ʊ��е�����
				ULONG_PTR dwNameOffset = pENT[dwIndex];
				char* pFunName = (char*)((ULONG_PTR)dwBase + dwNameOffset);
				if (!strcmp(pFunName, "GetProcAddress"))
				{// ���ݺ������Ʒ��غ�����ַ
					return (dwBase + dwFunAddrOffset);
				}
			}
		}
	}
	return 0;
}

//��ʼ����Ϣ,�õ�����API�ĵ�ַ
void PreventDebug::InitInformation()
{
	//��ȡkernel32��ַ	
	UCHAR* dwBase = GetKernel32Addr();
	
	g_pfnGetProcAddress = (fnGetProcAddress)MyGetProcessAddress();
	//��ȡAPI��ַ
	g_pfnLoadLibraryA = (fnLoadLibraryA)g_pfnGetProcAddress((HMODULE)dwBase, "LoadLibraryA");
	g_pfnGetModuleHandleA = (fnGetModuleHandleA)g_pfnGetProcAddress((HMODULE)dwBase, "GetModuleHandleA");
	g_pfnVirtualProtect = (fnVirtualProtect)g_pfnGetProcAddress((HMODULE)dwBase, "VirtualProtect");
	g_pfnVirtualAlloc = (fnVirtualAlloc)g_pfnGetProcAddress((HMODULE)dwBase, "VirtualAlloc");
	HMODULE hUser32 = (HMODULE)g_pfnLoadLibraryA("user32.dll");
	HMODULE hKernel32 = (HMODULE)g_pfnGetModuleHandleA("kernel32.dll");

	g_fnGetProcessHeap  = (fnGetProcessHeap)g_pfnGetProcAddress(hKernel32, "GetProcessHeap");
	g_HeapFree = (fnHeapFree)g_pfnGetProcAddress(hKernel32, "HeapFree");
	g_HeapAlloc = (fnHeapAlloc)g_pfnGetProcAddress(hKernel32, "HeapAlloc");
	g_pfnExitProcess = (fnExitProcess)g_pfnGetProcAddress(hKernel32, "ExitProcess");
	 g_fnGetTickCount	=(fnGetTickCount)g_pfnGetProcAddress(hKernel32, "GetTickCount");//��ȡ����ʱ��
	g_pfnMessageBox = (fnMessageBox)g_pfnGetProcAddress(hUser32, "MessageBoxW");
	g_fnplstrcmpi = (fnLSTRCMP)g_pfnGetProcAddress(hKernel32, "lstrcmpiA");


}

void PreventDebug::pmemcpy(void* _Dst, const void* _Res, DWORD _Size)
{
	for (int i = 0; i < _Size; i++)
	{
		((char*)_Dst)[i] = ((char*)_Res)[i];
	}
}

//�������(��CPU���к���������ڽ������)
void PreventDebug::XorMachineCode(ULONGLONG cpuId, ULONG_PTR ulCodeOfBase)
{
	//��ȡ���ӿǳ����OEP
	ULONG_PTR uCodeBase = ulCodeOfBase;
	
	//�����ν������
	DWORD dwOldProtect = 0;
	g_pfnVirtualProtect((LPVOID)ulCodeOfBase, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	*(ULONGLONG*)uCodeBase ^= cpuId;
	g_pfnVirtualProtect((LPVOID)ulCodeOfBase, 1, dwOldProtect, &dwOldProtect);
}

//ȡCPU���к�
ULONGLONG PreventDebug::GetCPUID()
{
	ULONGLONG cpuId = 0;
	__asm {
		mov eax, 01h   //eax=1:ȡCPU���к�
		xor edx, edx
		cpuid
		mov dword ptr cpuId[0], eax
		mov dword ptr cpuId[4], edx
	}
	return cpuId;
}

//�����Ƿ񱻵���(�����Է���1��û�����Է���0)
ULONG_PTR PreventDebug::IsBebug()
{
	ULONG_PTR isBebug = 0;
#ifdef _WIN64
	_asm {
		push rax
		push rbx
		mov rax, qword ptr gs : [60h]
		xor rbx, rbx
		mov bl, byte ptr[rax + 2]
		mov isBebug, rbx
		pop rbx
		pop	rax
	}
#else
	_asm {
		pushad
		mov eax, fs: [30h]
		xor ebx, ebx
		mov bl, byte ptr[eax + 2]
		mov isBebug, ebx
		popad
	}
#endif // _WIN64
	return isBebug;
}

//�������Ƿ񱻵���(���ֱ����ԣ�����ֱ�ӿ���)
BOOL PreventDebug::Check_ZwSetInformationObject()
{
	/*-----------------------------------------------------------------------------------------------------------*/
	/*	��32λ�����У����˰ѹ��ӹҵ�64λ��"ntdll.dll"�ϣ�Ȼ�����������ԣ�������crcУ����߼��ؼ��ֽ������������� */
	/*-----------------------------------------------------------------------------------------------------------*/
	HANDLE v3;
	HANDLE TargetHandle;

	typedef NTSTATUS(__stdcall* NTSETINFORMATIONOBJECT)(HANDLE objhandle, int objinforClass, PVOID objinfo, ULONG Length);
	NTSETINFORMATIONOBJECT pZwSetInformationObject;

	typedef BOOL(__stdcall* SETHANDLEINFORMATION)(_In_ HANDLE hObject, _In_ DWORD dwMask, _In_ DWORD dwFlags);
	SETHANDLEINFORMATION pSetHandleInformation;

	typedef BOOL(__stdcall* DUPLICATEHANDLE)(
			_In_ HANDLE hSourceProcessHandle,
			_In_ HANDLE hSourceHandle,
			_In_ HANDLE hTargetProcessHandle,
			_Outptr_ LPHANDLE lpTargetHandle,
			_In_ DWORD dwDesiredAccess,
			_In_ BOOL bInheritHandle,
			_In_ DWORD dwOptions
			);
	DUPLICATEHANDLE pDuplicateHandle;

	HMODULE hModule_1 = g_pfnLoadLibraryA("kernel32.dll");
	pSetHandleInformation = (SETHANDLEINFORMATION)g_pfnGetProcAddress(hModule_1, "SetHandleInformation");
	pDuplicateHandle = (DUPLICATEHANDLE)g_pfnGetProcAddress(hModule_1, "DuplicateHandle");

	HMODULE hModule = g_pfnLoadLibraryA("ntdll.dll");
	pZwSetInformationObject = (NTSETINFORMATIONOBJECT)g_pfnGetProcAddress(hModule, "ZwSetInformationObject");

	pDuplicateHandle((HANDLE)-1, (HANDLE)-1, (HANDLE)-1, &TargetHandle, 0, 0, 0);
	pZwSetInformationObject(TargetHandle, 4, &TargetHandle, 2);
	pSetHandleInformation(TargetHandle, 2, 2);
	pDuplicateHandle((HANDLE)-1, TargetHandle, (HANDLE)-1, &v3, 0, 0, 1);
#ifdef _WIN64
	return !v3 || v3 == (HANDLE)0xCCCCCCCCCCCCCCCC;
#endif // _WIN64

	return !v3 || v3 == (HANDLE)0xCCCCCCCC;
}


//typedef struct tagPROCESSENTRY32W
//{
//	DWORD   dwSize;
//	DWORD   cntUsage;
//	DWORD   th32ProcessID;          // this process
//	ULONG_PTR th32DefaultHeapID;
//	DWORD   th32ModuleID;           // associated exe
//	DWORD   cntThreads;
//	DWORD   th32ParentProcessID;    // this process's parent process
//	LONG    pcPriClassBase;         // Base priority of process's threads
//	DWORD   dwFlags;
//	WCHAR   szExeFile[MAX_PATH];    // Path
//} PROCESSENTRY32W,* LPPROCESSENTRY32W;
//#define TH32CS_SNAPPROCESS  0x00000002
//
////�������
//bool PreventDebug::GetProcessIdByName(TCHAR* szProcessName)
//{
//	typedef WINBASEAPI	int	(WINAPI* LSTRCMP)(_In_ LPCWSTR lpString1, _In_ LPCWSTR lpString2);
//	LSTRCMP plstrcmp;
//	typedef HANDLE(WINAPI* CREATETOOLHELP32SNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);
//	CREATETOOLHELP32SNAPSHOT pCreateToolhelp32Snapshot;
//	typedef BOOL(WINAPI* PROCESS32FIRST)(HANDLE hSnapshot, LPPROCESSENTRY32W lppe);
//	PROCESS32FIRST pProcess32First;
//	typedef BOOL(WINAPI* PROCESS32NEXT)(HANDLE hSnapshot, LPPROCESSENTRY32W lppe);
//	PROCESS32NEXT pProcess32Next;
//
//	HMODULE hModule_1 = g_pfnLoadLibraryA("kernel32.dll");
//	plstrcmp = (LSTRCMP)g_pfnGetProcAddress(hModule_1, "lstrcmpW");
//	pCreateToolhelp32Snapshot = (CREATETOOLHELP32SNAPSHOT)g_pfnGetProcAddress(hModule_1, "CreateToolhelp32Snapshot");
//	pProcess32First = (PROCESS32FIRST)g_pfnGetProcAddress(hModule_1, "Process32FirstW");
//	pProcess32Next = (PROCESS32NEXT)g_pfnGetProcAddress(hModule_1, "Process32NextW");
//
//	HANDLE hSnapProcess = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (hSnapProcess == NULL)
//	{
//		return FALSE;
//	}
//	PROCESSENTRY32W pe32 = { 0 };
//	pe32.dwSize = sizeof(pe32);
//	BOOL bRet = pProcess32First(hSnapProcess, &pe32);
//	while (bRet)
//	{
//		if (plstrcmp(pe32.szExeFile, szProcessName) == 0)
//		{
//			//g_pfnMessageBox(NULL, L"���������", L"Hello PEDIY", MB_OK);
//			return TRUE;
//		}
//		bRet = pProcess32Next(hSnapProcess, &pe32);
//	}
//	return FALSE;
//}

typedef struct tagPROCESSENTRY32or64
{
	DWORD   dwSize;
	DWORD   cntUsage;
	DWORD   th32ProcessID;          // this process
	ULONG_PTR th32DefaultHeapID;
	DWORD   th32ModuleID;           // associated exe
	DWORD   cntThreads;
	DWORD   th32ParentProcessID;    // this process's parent process
	LONG    pcPriClassBase;         // Base priority of process's threads
	DWORD   dwFlags;
#ifdef UNICODE
	WCHAR   szExeFile[MAX_PATH];    // Path
#else
	CHAR    szExeFile[MAX_PATH];    // Path
#endif // UNICODE

} PROCESSENTRY32or64, * LPPROCESSENTRY32or64;
#define TH32CS_SNAPPROCESS  0x00000002


//�������(Ѱ��Ŀ����̣��ɹ�����true,ʧ�ܷ���false)
bool PreventDebug::GetProcessIdByName(TCHAR* szProcessName)
{

	typedef int(__stdcall* LSTRCMP_)(
#ifdef UNICODE
		_In_ LPCWSTR lpString1, _In_ LPCWSTR lpString2
#else
		_In_ LPCSTR lpString1, _In_ LPCSTR lpString2
#endif // UNICODE
		);
	LSTRCMP_ plstrcmpi;

	typedef HANDLE(__stdcall* CREATETOOLHELP32SNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);
	CREATETOOLHELP32SNAPSHOT pCreateToolhelp32Snapshot;
	typedef BOOL(__stdcall* PROCESS32FIRST)(HANDLE hSnapshot, LPPROCESSENTRY32or64 lppe);
	PROCESS32FIRST pProcess32First;
	typedef BOOL(__stdcall* PROCESS32NEXT)(HANDLE hSnapshot, LPPROCESSENTRY32or64 lppe);
	PROCESS32NEXT pProcess32Next;

	HMODULE hModule_1 = g_pfnLoadLibraryA("kernel32.dll");
	pCreateToolhelp32Snapshot = (CREATETOOLHELP32SNAPSHOT)g_pfnGetProcAddress(hModule_1, "CreateToolhelp32Snapshot");

#ifdef UINCODE
	plstrcmpi = (LSTRCMP_)g_pfnGetProcAddress(hModule_1, "lstrcmpiW");
	pProcess32First = (PROCESS32FIRST)g_pfnGetProcAddress(hModule_1, "Process32FirstW");
	pProcess32Next = (PROCESS32NEXT)g_pfnGetProcAddress(hModule_1, "Process32NextW");

#else
	plstrcmpi = (LSTRCMP_)g_pfnGetProcAddress(hModule_1, "lstrcmpiA");
	pProcess32First = (PROCESS32FIRST)g_pfnGetProcAddress(hModule_1, "Process32First");
	pProcess32Next = (PROCESS32NEXT)g_pfnGetProcAddress(hModule_1, "Process32Next");
#endif // UINCODE

	HANDLE hSnapProcess = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapProcess == NULL)
	{
		return FALSE;
	}
	PROCESSENTRY32or64 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	BOOL bRet = pProcess32First(hSnapProcess, &pe32);
	while (bRet)
	{
		if (plstrcmpi(pe32.szExeFile, szProcessName) == 0)
		{
			//g_pfnMessageBox(NULL, L"���������", L"Hello PEDIY", MB_OK);
			return TRUE;
		}
		bRet = pProcess32Next(hSnapProcess, &pe32);
	}
	return FALSE;
}

