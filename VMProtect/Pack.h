#pragma once
#pragma once
#include "Common/header.h"
#include <Psapi.h>
#include <direct.h>
#include "../Stub/Stub/Stub.h"
class Pack
{
	AllocMemory m_allocMemory;
public:

	//开始保护
	void StartProtect(HWND hwndDlg, TCHAR* strPath, TCHAR* NewFileBuff, PEInfo peinfo, BYTE byXor, DWORD vminformation);

	//机器码绑定(将CPU序列号同主程序入口进行亦或)
	void XorMachineCode(ULONGLONG cpuId, PEInfo& peinfo);

	//获取CPU序列号(64位或者32位通用)
	ULONGLONG GetCPUID();

	//加密代码段
	DWORD XorCode(BYTE byXOR, PEInfo peinfo);



	//处理TLS
	void DealTLSCallback(DWORD imgBuff);

	//获取IAT表信息和重定位信息
	void getinfo(char* cbuff);

	//保存IAT表
	void SaveImportTab(char* m_pFileBuf);

	//抹去导入表所有信息
	void ClearImportTab(char* m_pFileBuf);

	//清除不需要的目录信息
	void ClearDirTable(char* buff);

	//合并目标PE和Stub.dll
	PBYTE MergeSection(PEInfo peinfo, PEInfo stubpeinfo, PBYTE lMod, BYTE byXor);

	//保存文件
	void SaveFile_pack(TCHAR* strPath, char* NewBuffer, ULONG_PTR m_uTotalSize);

	//RVA转File offset
	DWORD RvaToOffset(DWORD peBuff, DWORD Rva);

	void FixupTLSAddressCallback(DWORD peFileBuffer, DWORD newAddressCallback);
public:

	//typedef struct _PROTECTOR
	//{
	//	unsigned  EncryptionIAT : 1;		//是否IAT加密		
	//	unsigned  AddReDebug : 1;		//是否增加反调试
	//}PROTECTOR, * pPROTECTOR;

	////保存IAT表的结构体
	//typedef struct _MYIMPORT
	//{
	//	 
	//	ULONG_PTR	m_dwIATAddr;
	//	ULONG_PTR	m_dwModNameRVA;
	//	ULONG_PTR	m_dwFunNameRVA;
	//	BOOL		m_bIsOrdinal;
	//	ULONG_PTR	m_Ordinal;
	//}MYIMPORT, * PMYIMPORT;

	//typedef struct _GLOBAL_PARAM
	//{
	//	ULONG_PTR dwStart;					// Stub中的OEP

	//	BOOL bShowMessage;					// 是否显示解密信息
	//	ULONG_PTR dwImageBase;				// 映像基址
	//	ULONG_PTR ulBaseOfCode;				//代码基址
	//	ULONG_PTR dwOEP;					// 程序入口点
	//	PBYTE lpStartVA;					// 起始虚拟地址（被异或加密区）
	//	DWORD dwCodeSize;					// 被加密大小
	//	BYTE byXor;							// 加密异或的值

	//	IMAGE_DATA_DIRECTORY	stcPERelocDir;		//重定位表信息
	//	IMAGE_DATA_DIRECTORY	stcPEImportDir;		//导入表信息

	//	ULONG_PTR					dwIATSectionBase;	//IAT所在段基址
	//	DWORD					dwIATSectionSize;	//IAT所在段大小

	//	//解密IAT所用到的的变量
	//	PMYIMPORT				pMyImport;
	//	PVOID					pModNameBuf;
	//	PVOID					pFunNameBuf;
	//	ULONG_PTR					dwNumOfIATFuns;
	//	ULONG_PTR					dwSizeOfModBuf;
	//	ULONG_PTR					dwSizeOfFunBuf;
	//	ULONG_PTR					dwIATBaseRVA;//IAT的基址

	//	PROTECTOR	pProctect;

	//}GLOBAL_PARAM, * PGLOBAL_PARAM;


	PMYIMPORT		m_pMyImport;
	PVOID			m_pModNameBuf;
	PVOID			m_pFunNameBuf;
	ULONG_PTR		m_dwNumOfIATFuns = 0;
	ULONG_PTR		m_dwSizeOfModBuf = 0;
	ULONG_PTR		m_dwSizeOfFunBuf = 0;

	BOOL m_isUseTLS = false;
	ULONG_PTR m_TlsAddressOfCallback = NULL;

	DWORD m_encryptCodeSize;
	ULONG_PTR					m_IATSectionBase = 0;	//IAT所在段基址
	DWORD						m_IATSectionSize = 0;	//IAT所在段大小

	IMAGE_DATA_DIRECTORY		m_PERelocDir;		//重定位表信息
	IMAGE_DATA_DIRECTORY		m_PEImportDir;		//导入表信息


	ULONG_PTR m_uTotalSize = 0;//记录合并后的总大小

};

