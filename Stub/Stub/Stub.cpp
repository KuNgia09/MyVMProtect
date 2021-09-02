#include "Stub.h"
#include "PreventDebug.h"
#include <tchar.h>
#include "JunkCode.h"
//#include <algorithm>
#include "Vector_.h"
EXTERN_C __declspec(dllexport) GLOBAL_PARAM g_stcParam = { (ULONG_PTR)Start,(ULONG_PTR)StubTlsCallback};

PreventDebug Preventdebug;
typedef void(*FUN)();
FUN g_oep;

//初始化信息
ULONG_PTR  InitInformation();
//解密代码
void XorCode();
//正常修复IAT表
void RebuildImportTable();
//解密IAT表
void DecodeIAT();

//反调试函数
void preventdebug();

//处理TLS
void DealTLS();



/*----------------------------------------------------------------------------------*/
/*		   加壳程序开始执行的地方   												*/
/*----------------------------------------------------------------------------------*/
void __declspec(naked) Start()
{
#ifdef _WIN64	
	_asm {
		pushfq
		call InitInformation
		xor rax, rax
		mov al, byte ptr[g_stcParam.pProctect]
		and rax, 2 //是否增加了反调试
		je predebug
		call preventdebug
predebug:
		xor rax,rax
		mov al,byte ptr[g_stcParam.pProctect]
		and rax, 1 //是否增加了IAT加密
		je rebulidiat

		call DecodeIAT
		jmp decodeiat
rebulidiat :

		call RebuildImportTable
decodeiat :
		popfq
		jmp g_oep
	}
#else
	_asm {
		pushfd
		call InitInformation
		xor eax, eax
		mov al, byte ptr[g_stcParam.pProctect]
		and eax, 2	//是否增加了反调试
		je predebug
		call preventdebug
predebug :
		xor eax, eax
		mov al, byte ptr[g_stcParam.pProctect]
		and eax, 1	//是否增加了IAT加密

		je rebulidiat

		call DecodeIAT
		jmp decodeiat
	rebulidiat :
		call RebuildImportTable
	decodeiat :
		xor eax,eax
		mov eax,dword ptr[g_stcParam.stlAddressOfCallback]
		cmp eax,0
		je jmpoep
		call DealTLS
	jmpoep:
		popfd
		jmp g_oep
	}
#endif
}

//乱序垃圾指令
void Out_Of_Order_RubInstruction()
{
	//Preventdebug.g_pfnMessageBox(NULL, L"乱序执行", L"Hello PEDIY", MB_YESNO);

	for (int i = 0; ; i += 2)
	{
		DWORD jmptbladdr = *(DWORD*)(g_stcParam.m_vminformation + i * 4);
		int length = *(DWORD*)(g_stcParam.m_vminformation + 4 * (i + 1));

		if (jmptbladdr == 0)
		{
			break;
		}
		vector_<DWORD>temp_arr;

		for (int i = 0; i < length; i++)
		{
			temp_arr.push_back(*(DWORD*)(jmptbladdr + i * 4));
		}

		//打乱顺序
		temp_arr.outoforder(temp_arr);
		//Preventdebug.g_pfnMessageBox(NULL, L"乱序执行", L"Hello PEDIY", MB_YESNO);
		//再填充到跳转表中
		vector_<DWORD>::iterator iter_v = temp_arr.begin();
		for (int i = 0; i < length; i++)
		{
			*(DWORD*)(jmptbladdr + 4 * i) = *(iter_v + i);
		}
	}

}


//反调试函数
void preventdebug()
{
	//获取被加壳程序的代码基址
	ULONG_PTR uCodeBase = g_stcParam.dwImageBase + g_stcParam.dwOEP;

	//ULONGLONG ji = 0xbfebfbff000a0653;
	////解密绑定的机器码
	//Preventdebug.XorMachineCode(ji, uCodeBase);

	//解密绑定的机器码
	Preventdebug.XorMachineCode(Preventdebug.GetCPUID(), uCodeBase);

	if ( Preventdebug.IsBebug() ||
		Preventdebug.Check_ZwSetInformationObject() ||
		Preventdebug.GetProcessIdByName((TCHAR*)_T("vmtoolsd.exe"))//反虚拟机
		)
	{
		//退出程序
		Preventdebug.g_pfnExitProcess(0);
	}
}


//解密代码
void XorCode()
{
	//获取代码基址
	PBYTE pBase = (PBYTE)(g_stcParam.dwImageBase + g_stcParam.lpStartVA);
	//异或操作
	for (DWORD i = 0; i < g_stcParam.dwCodeSize; i++)
	{
		pBase[i] ^= g_stcParam.byXor;
	}
}

//初始化信息
ULONG_PTR  InitInformation()
{		
	//获取IAP的地址
	Preventdebug.InitInformation();

	//乱序垃圾指令
	Out_Of_Order_RubInstruction();

#ifdef _WIN64
	//弹出信息框			
	int nRet = Preventdebug.g_pfnMessageBox(NULL, L"欢迎使用免费64位加壳程序，是否运行主程序？", L"Hello PEDIY", MB_YESNO);
#else
	//弹出信息框			
	int nRet = Preventdebug.g_pfnMessageBox(NULL, L"欢迎使用免费32位加壳程序，是否运行主程序？", L"Hello PEDIY", MB_YESNO);
#endif
	if (nRet == IDYES)
	{
		//修改代码段属性
		ULONG_PTR dwCodeBase = g_stcParam.dwImageBase + (DWORD)g_stcParam.lpStartVA;
		DWORD dwOldProtect = 0;
		Preventdebug.g_pfnVirtualProtect((LPVOID)dwCodeBase, g_stcParam.dwCodeSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		XorCode();//解密代码
		Preventdebug.g_pfnVirtualProtect((LPVOID)dwCodeBase, g_stcParam.dwCodeSize, dwOldProtect, &dwOldProtect);

		g_oep = (FUN)(g_stcParam.dwImageBase + g_stcParam.dwOEP);
		//跳回原始OEP
		return (ULONG_PTR)g_oep;
	}
	//退出程序
	Preventdebug.g_pfnExitProcess(0);
	return 0;
}

//正常修复IAT表
void RebuildImportTable()
{

	//1.获取导入表结构体指针
	PIMAGE_IMPORT_DESCRIPTOR pPEImport =
		(PIMAGE_IMPORT_DESCRIPTOR)(g_stcParam.dwImageBase + g_stcParam.stcPEImportDir.VirtualAddress);

	//2.修改内存属性为可写
	DWORD dwOldProtect = 0;
	Preventdebug.g_pfnVirtualProtect(
		(LPBYTE)(g_stcParam.dwImageBase + g_stcParam.dwIATSectionBase), g_stcParam.dwIATSectionSize,
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	//3.开始修复IAT
	while (pPEImport->Name)
	{
		//获取模块名
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(g_stcParam.dwImageBase + dwModNameRVA);
		HMODULE hMod = Preventdebug.g_pfnLoadLibraryA(pModName);

		//获取IAT信息(有些文件INT是空的，最好用IAT解析，也可两个都解析作对比)
		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(g_stcParam.dwImageBase + pPEImport->FirstThunk);

		//获取INT信息(同IAT一样，可将INT看作是IAT的一个备份)
		//PIMAGE_THUNK_DATA pINT = (PIMAGE_THUNK_DATA)(dwImageBase + pPEImport->OriginalFirstThunk);

		//通过IAT循环获取该模块下的所有函数信息(这里之获取了函数名)
		while (pIAT->u1.AddressOfData)
		{
			//判断是输出函数名还是序号
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				//输出序号
#ifdef _WIN64
				ULONG_PTR dwFunOrdinal = (pIAT->u1.Ordinal) & 0x7FFFFFFFFFFFFFFF;
#else
				DWORD dwFunOrdinal = (pIAT->u1.Ordinal) & 0x7FFFFFFF;
#endif // DEBUG


				ULONG_PTR dwFunAddr = Preventdebug.g_pfnGetProcAddress(hMod, (char*)dwFunOrdinal);
				*(ULONG_PTR*)pIAT = (ULONG_PTR)dwFunAddr;
			}
			else
			{
				//输出函数名
				ULONG_PTR dwFunNameRVA = pIAT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(g_stcParam.dwImageBase + dwFunNameRVA);
				ULONG_PTR dwFunAddr = Preventdebug.g_pfnGetProcAddress(hMod, pstcFunName->Name);
				*(ULONG_PTR*)pIAT = (ULONG_PTR)dwFunAddr;
			}
			pIAT++;
		}
		//遍历下一个模块
		pPEImport++;
	}

	//4.恢复内存属性
	Preventdebug.g_pfnVirtualProtect(
		(LPBYTE)(g_stcParam.dwImageBase + g_stcParam.dwIATSectionBase), g_stcParam.dwIATSectionSize,
		dwOldProtect, &dwOldProtect);
}

//计数器
int gg_calc = 0;

//解密IAT表
void DecodeIAT()
{
	//Preventdebug.g_pfnMessageBox(NULL, L"解密IAT表", L"Hello PEDIY", MB_OK);
	//把IAT表的节,设置位可读可写可执行
	DWORD dwOldProtect = 0;
	Preventdebug.g_pfnVirtualProtect(
		(LPBYTE)(g_stcParam.dwImageBase + g_stcParam.dwIATSectionBase), g_stcParam.dwIATSectionSize,
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	//初始化指针信息
	g_stcParam.pMyImport = (PMYIMPORT)(g_stcParam.dwImageBase + g_stcParam.dwIATBaseRVA);
	g_stcParam.pModNameBuf = (CHAR*)g_stcParam.pMyImport + g_stcParam.dwNumOfIATFuns * sizeof(MYIMPORT);
	g_stcParam.pFunNameBuf = (CHAR*)g_stcParam.pModNameBuf + g_stcParam.dwSizeOfModBuf;


	PBYTE pNewAddr = (PBYTE)Preventdebug.g_pfnVirtualAlloc(0, (g_stcParam.dwNumOfIATFuns)*31, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	//解密模块名
	for (DWORD i = 0; i < g_stcParam.dwSizeOfModBuf; i++)
	{
		if (((char*)g_stcParam.pModNameBuf)[i] != 0)
		{
			((char*)g_stcParam.pModNameBuf)[i] ^= g_stcParam.byXor;
		}
	}


	/*----------------------------------------------------------------------------------*/
	/*	随机因子(取GetTickCount()函数)	      				     */
	/*----------------------------------------------------------------------------------*/
	JunkCode_ jkcode;
	jkcode.srand_v(Preventdebug.g_fnGetTickCount());

	//开始解密
	for (DWORD i = 0; i < g_stcParam.dwNumOfIATFuns; i++)
	{
		HMODULE hMod = NULL;
		//获取模块名称
		char* pModName = (char*)g_stcParam.pModNameBuf + g_stcParam.pMyImport[i].m_dwModNameRVA;
		hMod = Preventdebug.g_pfnGetModuleHandleA(pModName);
		if (hMod == NULL)
		{
			hMod = Preventdebug.g_pfnLoadLibraryA(pModName);
		}


#ifdef _WIN64


#else		
		JunkCode_ jkcode;
		jkcode.srand_v(Preventdebug.g_fnGetTickCount());
		//Preventdebug.g_pfnMessageBox(NULL, L"测试花指令!", L"Hello PEDIY", MB_OK);
		jkcode.SrandJunkCode(); //构造随机花指令
		vector_< BUFFERSTRUCT_>::iterator iter_buff = jkcode.g_buffer.begin();
		int recodemodify = 0;
		int bytelength = jkcode.g_buffer.size();
		
		char* tempbuff = (char*)Preventdebug.g_HeapAlloc(Preventdebug.g_fnGetProcessHeap(), 8, bytelength);
		for (int i = 0; i < bytelength; i++)
		{
			*(char*)(tempbuff + i) = (*iter_buff).value;
			if ((*iter_buff).recodemodify == 1)
			{
				recodemodify = i;
			}
			++iter_buff;
		}
#endif // _WIN64
		
		if (g_stcParam.pMyImport[i].m_bIsOrdinal)
		{
			//*********************************序号导出函数********************************

			ULONG_PTR dwFunOrdinal = g_stcParam.pMyImport[i].m_Ordinal;
			ULONG_PTR dwFunAddr = Preventdebug.g_pfnGetProcAddress(hMod, (char*)dwFunOrdinal);
			//*(ULONG_PTR*)((ULONG_PTR)g_stcParam.pMyImport[i].m_dwIATAddr + g_stcParam.dwImageBase) = (ULONG_PTR)dwFunAddr;

			//"MSVCP140.dll"暂时不要处理，会出错，原因还未找到
			if (Preventdebug.g_fnplstrcmpi(pModName, "MSVCP140.dll") == 0)
			{
#ifdef _WIN64
				BYTE byByte[]{ 0xEB ,0x02 ,0xE8 ,0x32 ,0x50 ,0x48 ,0xC7 ,0xC0 ,0x66 ,0x66 ,0x88	,0x88 ,0x58,
					0x48,0xb8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x35,0x15,0x15,0x15,0x15,0x50,0xc3
				};
				*(ULONG_PTR*)(byByte + 15) = (ULONG_PTR)dwFunAddr ^ 0x15151515;
				memcpy(pNewAddr + sizeof(byByte) * i, byByte, sizeof(byByte));
				*(ULONG_PTR*)((ULONG_PTR)g_stcParam.pMyImport[i].m_dwIATAddr + g_stcParam.dwImageBase) = (ULONG_PTR)(pNewAddr + sizeof(byByte) * i);
#else
				
				*(ULONG_PTR*)(tempbuff + recodemodify+1) = (ULONG_PTR)dwFunAddr ^ 0x68686868;
				Preventdebug.pmemcpy(pNewAddr + gg_calc, tempbuff, bytelength);

				*(ULONG_PTR*)((ULONG_PTR)g_stcParam.pMyImport[i].m_dwIATAddr + g_stcParam.dwImageBase) = (ULONG_PTR)(pNewAddr + gg_calc);
				gg_calc += bytelength;
							
#endif // _WIN64				
			}
			else
			{
				*(ULONG_PTR*)((ULONG_PTR)g_stcParam.pMyImport[i].m_dwIATAddr + g_stcParam.dwImageBase) = (ULONG_PTR)dwFunAddr;
			}						
		}
		else//********************************函数名导出函数*************************************
		{			
			//解密IAT函数名
			ULONG_PTR dwFunNameRVA = g_stcParam.pMyImport[i].m_dwFunNameRVA;
			char* pFunName = (char*)g_stcParam.pFunNameBuf + dwFunNameRVA;
			DWORD j = 0;
			
			while (pFunName[j])
			{
				((char*)pFunName)[j] ^= g_stcParam.byXor;
				j++;
			}
			
			ULONG_PTR dwFunAddr = Preventdebug.g_pfnGetProcAddress(hMod, pFunName);

			
			//"MSVCP140.dll"暂时不要处理，会出错，原因还未找到
			if (Preventdebug.g_fnplstrcmpi(pModName,"MSVCP140.dll")==0)
			{
#ifdef _WIN64
				BYTE byByte[]{ 0xEB ,0x02 ,0xE8 ,0x32 ,0x50 ,0x48 ,0xC7 ,0xC0 ,0x66 ,0x66 ,0x88	,0x88 ,0x58,
					0x48,0xb8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x35,0x68,0x68,0x68,0x68,0x50,0xc3
			};
				*(ULONG_PTR*)(byByte + 15) = (ULONG_PTR)dwFunAddr ^ 0x68686868;
				memcpy(pNewAddr + sizeof(byByte) * i, byByte, sizeof(byByte));

				*(ULONG_PTR*)((ULONG_PTR)g_stcParam.pMyImport[i].m_dwIATAddr + g_stcParam.dwImageBase) = (ULONG_PTR)(pNewAddr + sizeof(byByte) * i);

#else			
				*(ULONG_PTR*)(tempbuff + recodemodify+1) = (ULONG_PTR)dwFunAddr ^ 0x68686868;
				Preventdebug.pmemcpy(pNewAddr + gg_calc, tempbuff, bytelength);

				*(ULONG_PTR*)((ULONG_PTR)g_stcParam.pMyImport[i].m_dwIATAddr + g_stcParam.dwImageBase) = (ULONG_PTR)(pNewAddr + gg_calc);
				gg_calc += bytelength;
#endif // _WIN64

			}
			else
			{				
				*(ULONG_PTR*)((ULONG_PTR)g_stcParam.pMyImport[i].m_dwIATAddr + g_stcParam.dwImageBase) = (ULONG_PTR)dwFunAddr;
			}

			//抹去IAT函数名
			while (j--)
			{				
				pFunName[j] = 0;				
			}

			//释放内存
			Preventdebug.g_HeapFree(Preventdebug.g_fnGetProcessHeap(), 1, tempbuff);
		}
	}
	//抹去模块名
	for (DWORD i = 0; i < g_stcParam.dwSizeOfModBuf; i++)
	{
		((char*)g_stcParam.pModNameBuf)[i] = 0;
	}
}

typedef void (NTAPI *pfn_tlsCallback)(PVOID handle, DWORD reason, PVOID reserved);


void NTAPI StubTlsCallback(PVOID handle, DWORD reason, PVOID reserved) {
	Preventdebug.g_fnOutputDebugStringA("call StubTlsCallback\n");
	if (g_stcParam.isDecrypt) {
		//循环调用被加壳程序的TLS回调函数
	}
}

void DealTLS() {
	ULONG_PTR stlCallback=g_stcParam.originalStlAddressOfCallback;
	
	//遍历回调数组
	while (*(ULONG_PTR*)stlCallback) {
		pfn_tlsCallback addr = (pfn_tlsCallback)(*(ULONG_PTR*)stlCallback);
		HANDLE handle=Preventdebug.g_pfnGetModuleHandleA(NULL);
		addr(handle,DLL_PROCESS_ATTACH,0);
		stlCallback+=sizeof(ULONG_PTR);

	}
	ULONG_PTR imgBuff = g_stcParam.dwImageBase;
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)imgBuff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)imgBuff + pDosHeader->e_lfanew);

	IMAGE_DATA_DIRECTORY tlsDataDirectory = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
	PIMAGE_TLS_DIRECTORY tlsDir = (PIMAGE_TLS_DIRECTORY)(tlsDataDirectory.VirtualAddress + (DWORD)imgBuff);

	DWORD oldProtect;
	Preventdebug.g_pfnVirtualProtect(tlsDir, sizeof(IMAGE_TLS_DIRECTORY), PAGE_READWRITE, &oldProtect);

	//恢复stl
	tlsDir->AddressOfCallBacks = g_stcParam.originalStlAddressOfCallback;

	Preventdebug.g_pfnVirtualProtect(tlsDir, sizeof(IMAGE_TLS_DIRECTORY), oldProtect,NULL);
}
