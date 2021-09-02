#pragma once
#include <Windows.h>

extern "C"
{
	//����vm�������������Ϣ�ṹ��
	typedef struct VMINFORMATION_
	{
		int length;//����ָ���
		int jmptbladdr;//��ת���ַ
	}MVINFOR,*PMVINFOR;

	typedef struct _MYIMPORT
	{

		ULONG_PTR	m_dwIATAddr;			//IAT��ַ
		ULONG_PTR	m_dwModNameRVA;			//ģ����ƫ��
		ULONG_PTR	m_dwFunNameRVA;			//������ƫ��
		BOOL		m_bIsOrdinal;			//�Ƿ�Ϊ��ŵ�������
		ULONG_PTR	m_Ordinal;				//���
	}MYIMPORT, * PMYIMPORT;


	typedef struct _PROTECTOR
	{
		unsigned  EncryptionIAT : 1;		//�Ƿ�IAT����		
		unsigned  AddReDebug : 1;			//�Ƿ����ӷ�����
	}PROTECTOR, * pPROTECTOR;


	typedef struct _GLOBAL_PARAM
	{
		ULONG_PTR dwStart;		// Stub�е�OEP

		ULONG_PTR dwStubTlsCallback; //Stub��TLS callback������ַ

		//TLS callback������ַ����
		ULONG_PTR StubTLSCallbackArray[2] = { 0 };

		BOOL bShowMessage;		// �Ƿ���ʾ������Ϣ
		ULONG_PTR dwImageBase;	// ӳ���ַ
		ULONG_PTR ulBaseOfCode;	//�����ַ
		ULONG_PTR dwOEP;		// ������ڵ�
		PBYTE lpStartVA;		// ��ʼ�����ַ��������������
		DWORD dwCodeSize;		// �����ܴ�С
		BYTE byXor;				// ��������ֵ

		BOOL isDecrypt;//�Ǵ����Ƿ�������

		BOOL isUseSTl;
		// ָ��stl�ص���������
		ULONG_PTR originalStlAddressOfCallback;
		
		IMAGE_DATA_DIRECTORY	stcPERelocDir;		//�ض�λ����Ϣ
		IMAGE_DATA_DIRECTORY	stcPEImportDir;		//�������Ϣ

		ULONG_PTR					dwIATSectionBase;	//IAT���ڶλ�ַ
		DWORD					dwIATSectionSize;	//IAT���ڶδ�С

		//����IAT���õ��ĵı���
		PMYIMPORT				pMyImport;
		PVOID					pModNameBuf;
		PVOID					pFunNameBuf;
		ULONG_PTR					dwNumOfIATFuns;
		ULONG_PTR					dwSizeOfModBuf;
		ULONG_PTR					dwSizeOfFunBuf;
		ULONG_PTR					dwIATBaseRVA;


		DWORD				m_vminformation;

		PROTECTOR	pProctect;
	}GLOBAL_PARAM, * PGLOBAL_PARAM;

	void Start();
}


