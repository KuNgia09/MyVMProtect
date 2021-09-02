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

	//��ʼ����
	void StartProtect(HWND hwndDlg, TCHAR* strPath, TCHAR* NewFileBuff, PEInfo peinfo, BYTE byXor, DWORD vminformation);

	//�������(��CPU���к�ͬ��������ڽ������)
	void XorMachineCode(ULONGLONG cpuId, PEInfo& peinfo);

	//��ȡCPU���к�(64λ����32λͨ��)
	ULONGLONG GetCPUID();

	//���ܴ����
	DWORD XorCode(BYTE byXOR, PEInfo peinfo);



	//����TLS
	void DealTLSCallback(DWORD imgBuff);

	//��ȡIAT����Ϣ���ض�λ��Ϣ
	void getinfo(char* cbuff);

	//����IAT��
	void SaveImportTab(char* m_pFileBuf);

	//Ĩȥ�����������Ϣ
	void ClearImportTab(char* m_pFileBuf);

	//�������Ҫ��Ŀ¼��Ϣ
	void ClearDirTable(char* buff);

	//�ϲ�Ŀ��PE��Stub.dll
	PBYTE MergeSection(PEInfo peinfo, PEInfo stubpeinfo, PBYTE lMod, BYTE byXor);

	//�����ļ�
	void SaveFile_pack(TCHAR* strPath, char* NewBuffer, ULONG_PTR m_uTotalSize);

	//RVAתFile offset
	DWORD RvaToOffset(DWORD peBuff, DWORD Rva);

	void FixupTLSAddressCallback(DWORD peFileBuffer, DWORD newAddressCallback);
public:

	//typedef struct _PROTECTOR
	//{
	//	unsigned  EncryptionIAT : 1;		//�Ƿ�IAT����		
	//	unsigned  AddReDebug : 1;		//�Ƿ����ӷ�����
	//}PROTECTOR, * pPROTECTOR;

	////����IAT��Ľṹ��
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
	//	ULONG_PTR dwStart;					// Stub�е�OEP

	//	BOOL bShowMessage;					// �Ƿ���ʾ������Ϣ
	//	ULONG_PTR dwImageBase;				// ӳ���ַ
	//	ULONG_PTR ulBaseOfCode;				//�����ַ
	//	ULONG_PTR dwOEP;					// ������ڵ�
	//	PBYTE lpStartVA;					// ��ʼ�����ַ��������������
	//	DWORD dwCodeSize;					// �����ܴ�С
	//	BYTE byXor;							// ��������ֵ

	//	IMAGE_DATA_DIRECTORY	stcPERelocDir;		//�ض�λ����Ϣ
	//	IMAGE_DATA_DIRECTORY	stcPEImportDir;		//�������Ϣ

	//	ULONG_PTR					dwIATSectionBase;	//IAT���ڶλ�ַ
	//	DWORD					dwIATSectionSize;	//IAT���ڶδ�С

	//	//����IAT���õ��ĵı���
	//	PMYIMPORT				pMyImport;
	//	PVOID					pModNameBuf;
	//	PVOID					pFunNameBuf;
	//	ULONG_PTR					dwNumOfIATFuns;
	//	ULONG_PTR					dwSizeOfModBuf;
	//	ULONG_PTR					dwSizeOfFunBuf;
	//	ULONG_PTR					dwIATBaseRVA;//IAT�Ļ�ַ

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
	ULONG_PTR					m_IATSectionBase = 0;	//IAT���ڶλ�ַ
	DWORD						m_IATSectionSize = 0;	//IAT���ڶδ�С

	IMAGE_DATA_DIRECTORY		m_PERelocDir;		//�ض�λ����Ϣ
	IMAGE_DATA_DIRECTORY		m_PEImportDir;		//�������Ϣ


	ULONG_PTR m_uTotalSize = 0;//��¼�ϲ�����ܴ�С

};

