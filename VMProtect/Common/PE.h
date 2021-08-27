#pragma once
#include <Windows.h>
#include <tchar.h>
#include "AllocMemory.h"

//#define DWORD unsigned int
#ifdef _WIN64
#define POINTER_TYPE ULONGLONG
#else
#define POINTER_TYPE DWORD
#endif

//PE�ṹ����Ϣ�ṹ��
typedef struct PEInformation
{
	POINTER_TYPE FileBuffer;			//�ļ����ڴ���ĵ�ַ
	POINTER_TYPE FileSize;				//�ļ���С
	POINTER_TYPE e_lfanes;				//PE�ļ�ͷ��ַ
	POINTER_TYPE NumberOfSections;		//������Ŀ
	POINTER_TYPE SizeOfOptionHeaders;	//��ѡͷ��С
	POINTER_TYPE SizeOfCode;			//����ڴ�С
	POINTER_TYPE AddressOfEntryPoint;	//OEP(RVA)��ڵ�
	POINTER_TYPE BaseOfCode;			//�����ַ
	POINTER_TYPE BaseOfData;			//���ݻ�ַ
	POINTER_TYPE ImageBase;				//�����ַ
	POINTER_TYPE SectionAlignment;		//�ڴ����
	POINTER_TYPE FileAlignment;			//�ļ�����
	POINTER_TYPE SizeofImage;			//�����С
	POINTER_TYPE SizeOfHeaders;			//ͷ��С
	IMAGE_DATA_DIRECTORY* DataDirectory;	//Ŀ¼
	PIMAGE_NT_HEADERS pNtHeader;			//Ntͷ
	PIMAGE_SECTION_HEADER pSectionHeader;	//��ͷ
	PIMAGE_OPTIONAL_HEADER OptionalHeader;	//��ѡPEͷ

}PEInfo, *pPEInfo;


class PE:public AllocMemory
{
	AllocMemory m_alloc;
	TCHAR s[100];//ʱ���
public:
	PE();
	~PE();
public:
	//ʱ���ת��Ϊ��׼ʱ��
	TCHAR* Stamp_To_Standard(DWORD stampTime);
public:
	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		AlignSize �� ȡ�����뺯��
	*����*  Returns:		���ض�������ֵ
	*����*  Parameter_1:	uSize,�������ֵ
	*����*  Parameter_2:	uSecAlignment���ļ���������ڴ������ֵ
	*����*  Parameter_3:
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	ULONG_PTR AlignSize(_In_ ULONG_PTR uSize, ULONG_PTR uSecAlignment)
	{
		//return (uSize % uSecAlignment == 0) ? uSize : (uSize - (uSize % uSecAlignment) + uSecAlignment);
		return ((uSize + uSecAlignment - 1) / uSecAlignment * uSecAlignment);
	};


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		IsPEFile - �ж��Ƿ���64λ����32λ��PE�ļ�
	*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
	*����*  Parameter_1:	pFileBuffer,ģ���ļ����ڴ��еĵ�ַ
	*����*  Parameter_2:	hwndDlg����ѡ�������Ǵ��ھ��
	*����*  Parameter_3:
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOLEAN IsPEFile(UCHAR* pFileBuffer, HWND hwndDlg = 0);

public:

	/*////////////////////////////////////////////////////////////////
	*����*  FullName:	PerformBaseRelocation - �޸��ض�λ��	
	*����*  Returns:	��
	*����*  Parameter:	char* buff,PE�ļ��׵�ַ(�����)
	*����*  Parameter:	DWORD Value��PE��ַ�뵱ǰ���ڴ��еĵ�ַ�Ĳ�ֵ
	*����*  Parameter:
	*����*  Parameter:
	*����*	Parameter:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	void PerformBaseRelocation(POINTER_TYPE buff, POINTER_TYPE Value);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:	RebuildImportTable - �޸�IAT��
	*����*  Returns:	�ɹ�����1��ʧ�ܷ���0
	*����*  Parameter:	char* buff��PE�ļ����ڴ��еĵ�ַ(�����)
	*����*  Parameter:
	*����*  Parameter:
	*����*  Parameter:
	*����*	Parameter:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL RebuildImportTable(POINTER_TYPE buff);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		StretchFile - �����ļ�	
	*����*  Returns:		�ɹ������µ�ַ��ʧ�ܷ���0
	*����*  Parameter_1:	pFileBuff��ģ���ַ
	*����*  Parameter_2:	FileSize,�����С
	*����*  Parameter_3:
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	POINTER_TYPE StretchFile(POINTER_TYPE pFileBuff, DWORD FileSize);

	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		ImageBuff_To_FileBuff - ��PE�ļ���ԭ���ļ����̴�С
	*����*  Returns:		�ɹ������µ�ַ��ʧ�ܷ���0
	*����*  Parameter_1:	imgbuffer��ģ���ַ
	*����*  Parameter_2:	length���ļ���С
	*����*  Parameter_3:
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	char* ImageBuff_To_FileBuff(char* imgbuffer, DWORD length);



	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		GET_HEADER_DICTIONARY
	*����*  ����	:		��ȡĿ¼��ĵ�ַ
	*����*  Returns:		�ɹ��򷵻�Ҫ��ѯ������Ŀ¼����ڴ�ƫ��
	*����*  Parameter_1:	module��ģ��ĵ�ַ
	*����*  Parameter_2:	idx,Ҫ��ѯ���ű�
	*����*  Parameter_3:
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	DWORD GET_HEADER_DICTIONARY(POINTER_TYPE module,int idx);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		GetPEInformation_
	*����*  ����	:		��һ���ļ����������ڴ棬��ȡPE�ļ��ĸ�����Ϣ
	*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
	*����*  Parameter_1:	FilePath,�ļ�·��
	*����*  Parameter_2:	pPEInfor���������,�ѵõ���PE��Ϣ��ŵ�pPEInfor�ṹ����
	*����*  Parameter_3:
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	bool GetPEInformation_(TCHAR * FilePath, _Out_ PEInformation * pPEInfor);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		GetPEInformation_1
	*����*  ����	:		�����ڴ�ģ�飬��ȡPE�ļ��ĸ�����Ϣ
	*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
	*����*  Parameter_1:	pFilebuff��ģ��ĵ�ַ
	*����*  Parameter_2:	pPEInfor������������ѵõ���PE��Ϣ��ŵ�pPEInfor�ṹ����
	*����*  Parameter_3:	dwFileSize��ģ����ļ���С
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	bool GetPEInformation_1(char* pFilebuff, _Out_ PEInformation * pPEInfor, _In_ DWORD dwFileSize=0);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		addSeciton
	*����*  ����	:		����½�
	*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
	*����*  Parameter_1:	pFileBuff��ģ���ַ
	*����*  Parameter_2:	AddSize��Ҫ��ӵĴ�С
	*����*  Parameter_3:	secname[8]���½����ƣ������ڰ˸��ֽ���
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	bool addSeciton(POINTER_TYPE pFileBuff, DWORD AddSize, char secname[8] = { 0 });

};

