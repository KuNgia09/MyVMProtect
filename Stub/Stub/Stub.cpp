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

//��ʼ����Ϣ
ULONG_PTR  InitInformation();
//���ܴ���
void XorCode();
//�����޸�IAT��
void RebuildImportTable();
//����IAT��
void DecodeIAT();

//�����Ժ���
void preventdebug();

//����TLS
void DealTLS();



/*----------------------------------------------------------------------------------*/
/*		   �ӿǳ���ʼִ�еĵط�   												*/
/*----------------------------------------------------------------------------------*/
void __declspec(naked) Start()
{
#ifdef _WIN64	
	_asm {
		pushfq
		call InitInformation
		xor rax, rax
		mov al, byte ptr[g_stcParam.pProctect]
		and rax, 2 //�Ƿ������˷�����
		je predebug
		call preventdebug
predebug:
		xor rax,rax
		mov al,byte ptr[g_stcParam.pProctect]
		and rax, 1 //�Ƿ�������IAT����
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
		and eax, 2	//�Ƿ������˷�����
		je predebug
		call preventdebug
predebug :
		xor eax, eax
		mov al, byte ptr[g_stcParam.pProctect]
		and eax, 1	//�Ƿ�������IAT����

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

//��������ָ��
void Out_Of_Order_RubInstruction()
{
	//Preventdebug.g_pfnMessageBox(NULL, L"����ִ��", L"Hello PEDIY", MB_YESNO);

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

		//����˳��
		temp_arr.outoforder(temp_arr);
		//Preventdebug.g_pfnMessageBox(NULL, L"����ִ��", L"Hello PEDIY", MB_YESNO);
		//����䵽��ת����
		vector_<DWORD>::iterator iter_v = temp_arr.begin();
		for (int i = 0; i < length; i++)
		{
			*(DWORD*)(jmptbladdr + 4 * i) = *(iter_v + i);
		}
	}

}


//�����Ժ���
void preventdebug()
{
	//��ȡ���ӿǳ���Ĵ����ַ
	ULONG_PTR uCodeBase = g_stcParam.dwImageBase + g_stcParam.dwOEP;

	//ULONGLONG ji = 0xbfebfbff000a0653;
	////���ܰ󶨵Ļ�����
	//Preventdebug.XorMachineCode(ji, uCodeBase);

	//���ܰ󶨵Ļ�����
	Preventdebug.XorMachineCode(Preventdebug.GetCPUID(), uCodeBase);

	if ( Preventdebug.IsBebug() ||
		Preventdebug.Check_ZwSetInformationObject() ||
		Preventdebug.GetProcessIdByName((TCHAR*)_T("vmtoolsd.exe"))//�������
		)
	{
		//�˳�����
		Preventdebug.g_pfnExitProcess(0);
	}
}


//���ܴ���
void XorCode()
{
	//��ȡ�����ַ
	PBYTE pBase = (PBYTE)(g_stcParam.dwImageBase + g_stcParam.lpStartVA);
	//������
	for (DWORD i = 0; i < g_stcParam.dwCodeSize; i++)
	{
		pBase[i] ^= g_stcParam.byXor;
	}
}

//��ʼ����Ϣ
ULONG_PTR  InitInformation()
{		
	//��ȡIAP�ĵ�ַ
	Preventdebug.InitInformation();

	//��������ָ��
	Out_Of_Order_RubInstruction();

#ifdef _WIN64
	//������Ϣ��			
	int nRet = Preventdebug.g_pfnMessageBox(NULL, L"��ӭʹ�����64λ�ӿǳ����Ƿ�����������", L"Hello PEDIY", MB_YESNO);
#else
	//������Ϣ��			
	int nRet = Preventdebug.g_pfnMessageBox(NULL, L"��ӭʹ�����32λ�ӿǳ����Ƿ�����������", L"Hello PEDIY", MB_YESNO);
#endif
	if (nRet == IDYES)
	{
		//�޸Ĵ��������
		ULONG_PTR dwCodeBase = g_stcParam.dwImageBase + (DWORD)g_stcParam.lpStartVA;
		DWORD dwOldProtect = 0;
		Preventdebug.g_pfnVirtualProtect((LPVOID)dwCodeBase, g_stcParam.dwCodeSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		XorCode();//���ܴ���
		Preventdebug.g_pfnVirtualProtect((LPVOID)dwCodeBase, g_stcParam.dwCodeSize, dwOldProtect, &dwOldProtect);

		g_oep = (FUN)(g_stcParam.dwImageBase + g_stcParam.dwOEP);
		//����ԭʼOEP
		return (ULONG_PTR)g_oep;
	}
	//�˳�����
	Preventdebug.g_pfnExitProcess(0);
	return 0;
}

//�����޸�IAT��
void RebuildImportTable()
{

	//1.��ȡ�����ṹ��ָ��
	PIMAGE_IMPORT_DESCRIPTOR pPEImport =
		(PIMAGE_IMPORT_DESCRIPTOR)(g_stcParam.dwImageBase + g_stcParam.stcPEImportDir.VirtualAddress);

	//2.�޸��ڴ�����Ϊ��д
	DWORD dwOldProtect = 0;
	Preventdebug.g_pfnVirtualProtect(
		(LPBYTE)(g_stcParam.dwImageBase + g_stcParam.dwIATSectionBase), g_stcParam.dwIATSectionSize,
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	//3.��ʼ�޸�IAT
	while (pPEImport->Name)
	{
		//��ȡģ����
		DWORD dwModNameRVA = pPEImport->Name;
		char* pModName = (char*)(g_stcParam.dwImageBase + dwModNameRVA);
		HMODULE hMod = Preventdebug.g_pfnLoadLibraryA(pModName);

		//��ȡIAT��Ϣ(��Щ�ļ�INT�ǿյģ������IAT������Ҳ���������������Ա�)
		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(g_stcParam.dwImageBase + pPEImport->FirstThunk);

		//��ȡINT��Ϣ(ͬIATһ�����ɽ�INT������IAT��һ������)
		//PIMAGE_THUNK_DATA pINT = (PIMAGE_THUNK_DATA)(dwImageBase + pPEImport->OriginalFirstThunk);

		//ͨ��IATѭ����ȡ��ģ���µ����к�����Ϣ(����֮��ȡ�˺�����)
		while (pIAT->u1.AddressOfData)
		{
			//�ж�������������������
			if (IMAGE_SNAP_BY_ORDINAL(pIAT->u1.Ordinal))
			{
				//������
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
				//���������
				ULONG_PTR dwFunNameRVA = pIAT->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pstcFunName = (PIMAGE_IMPORT_BY_NAME)(g_stcParam.dwImageBase + dwFunNameRVA);
				ULONG_PTR dwFunAddr = Preventdebug.g_pfnGetProcAddress(hMod, pstcFunName->Name);
				*(ULONG_PTR*)pIAT = (ULONG_PTR)dwFunAddr;
			}
			pIAT++;
		}
		//������һ��ģ��
		pPEImport++;
	}

	//4.�ָ��ڴ�����
	Preventdebug.g_pfnVirtualProtect(
		(LPBYTE)(g_stcParam.dwImageBase + g_stcParam.dwIATSectionBase), g_stcParam.dwIATSectionSize,
		dwOldProtect, &dwOldProtect);
}

//������
int gg_calc = 0;

//����IAT��
void DecodeIAT()
{
	//Preventdebug.g_pfnMessageBox(NULL, L"����IAT��", L"Hello PEDIY", MB_OK);
	//��IAT��Ľ�,����λ�ɶ���д��ִ��
	DWORD dwOldProtect = 0;
	Preventdebug.g_pfnVirtualProtect(
		(LPBYTE)(g_stcParam.dwImageBase + g_stcParam.dwIATSectionBase), g_stcParam.dwIATSectionSize,
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	//��ʼ��ָ����Ϣ
	g_stcParam.pMyImport = (PMYIMPORT)(g_stcParam.dwImageBase + g_stcParam.dwIATBaseRVA);
	g_stcParam.pModNameBuf = (CHAR*)g_stcParam.pMyImport + g_stcParam.dwNumOfIATFuns * sizeof(MYIMPORT);
	g_stcParam.pFunNameBuf = (CHAR*)g_stcParam.pModNameBuf + g_stcParam.dwSizeOfModBuf;


	PBYTE pNewAddr = (PBYTE)Preventdebug.g_pfnVirtualAlloc(0, (g_stcParam.dwNumOfIATFuns)*31, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	//����ģ����
	for (DWORD i = 0; i < g_stcParam.dwSizeOfModBuf; i++)
	{
		if (((char*)g_stcParam.pModNameBuf)[i] != 0)
		{
			((char*)g_stcParam.pModNameBuf)[i] ^= g_stcParam.byXor;
		}
	}


	/*----------------------------------------------------------------------------------*/
	/*	�������(ȡGetTickCount()����)	      				     */
	/*----------------------------------------------------------------------------------*/
	JunkCode_ jkcode;
	jkcode.srand_v(Preventdebug.g_fnGetTickCount());

	//��ʼ����
	for (DWORD i = 0; i < g_stcParam.dwNumOfIATFuns; i++)
	{
		HMODULE hMod = NULL;
		//��ȡģ������
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
		//Preventdebug.g_pfnMessageBox(NULL, L"���Ի�ָ��!", L"Hello PEDIY", MB_OK);
		jkcode.SrandJunkCode(); //���������ָ��
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
			//*********************************��ŵ�������********************************

			ULONG_PTR dwFunOrdinal = g_stcParam.pMyImport[i].m_Ordinal;
			ULONG_PTR dwFunAddr = Preventdebug.g_pfnGetProcAddress(hMod, (char*)dwFunOrdinal);
			//*(ULONG_PTR*)((ULONG_PTR)g_stcParam.pMyImport[i].m_dwIATAddr + g_stcParam.dwImageBase) = (ULONG_PTR)dwFunAddr;

			//"MSVCP140.dll"��ʱ��Ҫ���������ԭ��δ�ҵ�
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
		else//********************************��������������*************************************
		{			
			//����IAT������
			ULONG_PTR dwFunNameRVA = g_stcParam.pMyImport[i].m_dwFunNameRVA;
			char* pFunName = (char*)g_stcParam.pFunNameBuf + dwFunNameRVA;
			DWORD j = 0;
			
			while (pFunName[j])
			{
				((char*)pFunName)[j] ^= g_stcParam.byXor;
				j++;
			}
			
			ULONG_PTR dwFunAddr = Preventdebug.g_pfnGetProcAddress(hMod, pFunName);

			
			//"MSVCP140.dll"��ʱ��Ҫ���������ԭ��δ�ҵ�
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

			//ĨȥIAT������
			while (j--)
			{				
				pFunName[j] = 0;				
			}

			//�ͷ��ڴ�
			Preventdebug.g_HeapFree(Preventdebug.g_fnGetProcessHeap(), 1, tempbuff);
		}
	}
	//Ĩȥģ����
	for (DWORD i = 0; i < g_stcParam.dwSizeOfModBuf; i++)
	{
		((char*)g_stcParam.pModNameBuf)[i] = 0;
	}
}

typedef void (NTAPI *pfn_tlsCallback)(PVOID handle, DWORD reason, PVOID reserved);


void NTAPI StubTlsCallback(PVOID handle, DWORD reason, PVOID reserved) {
	Preventdebug.g_fnOutputDebugStringA("call StubTlsCallback\n");
	if (g_stcParam.isDecrypt) {
		//ѭ�����ñ��ӿǳ����TLS�ص�����
	}
}

void DealTLS() {
	ULONG_PTR stlCallback=g_stcParam.originalStlAddressOfCallback;
	
	//�����ص�����
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

	//�ָ�stl
	tlsDir->AddressOfCallBacks = g_stcParam.originalStlAddressOfCallback;

	Preventdebug.g_pfnVirtualProtect(tlsDir, sizeof(IMAGE_TLS_DIRECTORY), oldProtect,NULL);
}
