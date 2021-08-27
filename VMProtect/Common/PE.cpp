#include "PE.h"
#include <iostream>
#include <time.h>
PE::PE()
{
}

PE::~PE()
{
}

//ʱ���ת��Ϊ��׼ʱ��
TCHAR* PE::Stamp_To_Standard(DWORD stampTime)
{
	time_t tick = (time_t)stampTime;
	struct tm tm;
	localtime_s(&tm, &tick);
	_tcsftime(s, sizeof(s), (TCHAR*)_T("%Y-%m-%d %H:%M:%S"), &tm);
	return s;
}

BOOLEAN PE::IsPEFile(UCHAR* pFileBuffer, HWND hwndDlg)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuffer;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		MessageBoxA(hwndDlg, "������Ч��MZ��־!", "��ʾ", NULL);
		return FALSE;
	}

	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((ULONGLONG)pDosHeader + pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		MessageBoxA(hwndDlg, "������Ч��PE��־!", "��ʾ", NULL);
		return FALSE;
	}
#ifdef _WIN64
	if (pNTHeader->OptionalHeader.Magic != 0x20B)
	{
		MessageBoxA(hwndDlg, "������Ч��64λPE����!", "��ʾ", NULL);
		return FALSE;
	}
#else
	if (pNTHeader->OptionalHeader.Magic != 0x10B)
	{
		MessageBoxA(hwndDlg, "������Ч��32λPE����!", "��ʾ", NULL);
		return FALSE;
	}
#endif // _WIN64

	

	return TRUE;
}

/*////////////////////////////////////////////////////////////////
*����*  FullName:	PerformBaseRelocation
*����*  ����	:	�޸��ض�λ��
*����*  Returns:	��
*����*  Parameter:	char* buff,PE�ļ��׵�ַ
*����*  Parameter:	POINTER_TYPE Value��buff�Ļ�ַ�������ڴ��еĵ�ַ�Ĳ�ֵ
*����*  Parameter:
*����*  Parameter:
*����*	Parameter:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
//#ifdef _WIN64
void PE::PerformBaseRelocation(POINTER_TYPE buff, POINTER_TYPE Value)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)buff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(buff + pDosHeader->e_lfanew);

	//��ȡĿ¼��ͷָ��
	PIMAGE_DATA_DIRECTORY pDataDirectory = pNtHeader->OptionalHeader.DataDirectory;
	if (pDataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size > 0)
	{
		PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION)((POINTER_TYPE)buff + pDataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
		while (relocation->VirtualAddress > 0)
		{
			BYTE* dest = (PBYTE)((POINTER_TYPE)buff + relocation->VirtualAddress);
			WORD* relInfo = (PWORD)((POINTER_TYPE)relocation + sizeof(IMAGE_BASE_RELOCATION));
			for (DWORD i = 0; i < ((relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2); ++i, ++relInfo)
			{
				DWORD *patchAddrHL;
#ifdef _WIN64
				ULONGLONG *patchAddr64;//change comlete 64 bit address
#endif
		
				POINTER_TYPE type, offset;

				//the upper 4 bits define the type of relocation
				type = *relInfo >> 12;
				//the lower 12 bits define the offset
				offset = (*relInfo) & 0xFFF;

				switch (type)
				{
				case IMAGE_REL_BASED_ABSOLUTE:
					//skip relocation
					break;
#ifdef _WIN64
				case IMAGE_REL_BASED_DIR64://change comlete 64 bit address
					 patchAddr64 = (ULONGLONG*)(dest + offset);
					 *patchAddr64 -= Value;
					 break;
#endif				
				case IMAGE_REL_BASED_HIGHLOW://change comlete 32 bit address					
					patchAddrHL = (DWORD*)(dest + offset);
					*patchAddrHL -= Value;
					break;								
				default:
					break;
				}
			}

			//advance to next relocation block
			relocation = PIMAGE_BASE_RELOCATION((char*)relocation + relocation->SizeOfBlock);
		}
	}

}

/*////////////////////////////////////////////////////////////////
*����*  FullName:	RebuildImportTable
*����*  ����	:	�޸�IAT��
*����*  Returns:	�ɹ�����1��ʧ�ܷ���0
*����*  Parameter:	char* buff��PE�ļ����ڴ��еĵ�ַ(�����)
*����*  Parameter:
*����*  Parameter:
*����*  Parameter:
*����*	Parameter:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
BOOL PE::RebuildImportTable(POINTER_TYPE buff)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)buff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(buff + pDosHeader->e_lfanew);
	int result = 1;
	//��ȡĿ¼��ͷָ��
	PIMAGE_DATA_DIRECTORY pDataDirectory = pNtHeader->OptionalHeader.DataDirectory;

	if (pDataDirectory[1].Size > 0)
	{
		//��ȡ������ַ
		PIMAGE_IMPORT_DESCRIPTOR ImportAddr = PIMAGE_IMPORT_DESCRIPTOR(pDataDirectory[1].VirtualAddress + (POINTER_TYPE)buff);

		for (; !IsBadReadPtr(ImportAddr, sizeof(PIMAGE_IMPORT_DESCRIPTOR)) && ImportAddr->Name; ++ImportAddr)
		{
			POINTER_TYPE *thunkRef;
			FARPROC *funcRef;
#ifdef UNICODE
			HMODULE hModule = LoadLibraryA(LPCSTR(buff + ImportAddr->Name));
#else
			HMODULE hModule = LoadLibrary(LPCSTR(buff + ImportAddr->Name));
#endif // !UNICODE
			//HMODULE hModule = LoadLibrary((buff + ImportAddr->Name));

			if (ImportAddr->OriginalFirstThunk)
			{
				thunkRef = (POINTER_TYPE *)(buff + ImportAddr->OriginalFirstThunk);
				funcRef = (FARPROC*)(buff + ImportAddr->FirstThunk);
			}
			else
			{
				//no hint table
				thunkRef = (POINTER_TYPE *)(buff + ImportAddr->FirstThunk);
				funcRef = (FARPROC*)(buff + ImportAddr->FirstThunk);
			}

			for (; *thunkRef; ++thunkRef, ++funcRef)
			{
				if (IMAGE_SNAP_BY_ORDINAL(*thunkRef))
				{
					*funcRef = (FARPROC)GetProcAddress(hModule, (LPCSTR)IMAGE_ORDINAL(*thunkRef));
				}
				else
				{
					PIMAGE_IMPORT_BY_NAME pFuncName = (PIMAGE_IMPORT_BY_NAME)(*thunkRef + buff);
  					*funcRef = (FARPROC)GetProcAddress(hModule, (LPCSTR)&pFuncName->Name);
				}
				if (*funcRef == 0)
				{
					result = 0;
					break;
				}
			}
		}
	}
	return result;
}

/*////////////////////////////////////////////////////////////////
*����*  FullName:		StretchFile
*����*  ����	:		�����ļ�
*����*  Returns:		�ɹ������µ�ַ��ʧ�ܷ���0
*����*  Parameter_1:	pFileBuff��ģ���ַ
*����*  Parameter_2:	FileSize,�����С
*����*  Parameter_3:
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
POINTER_TYPE PE::StretchFile(POINTER_TYPE pFileBuff, DWORD FileSize)
{

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFileBuff + pDosHeader->e_lfanew);

	//1.1 �����ڴ��С �����ڴ�
	//char* NewFileBuff = new char[FileSize];
	char* NewFileBuff= m_alloc.auto_malloc<CHAR*>(FileSize);
	if (NewFileBuff == NULL)
	{
		printf("�ڴ�����ʧ��!");
		return 0;
	}
	memset(NewFileBuff, 0, FileSize);

	//1.2 �����ļ�
	 // ����DOSͷ + DOS STUB + PEͷ��headers��ַ��
	memcpy(NewFileBuff, pDosHeader, pNtHeader->OptionalHeader.SizeOfHeaders);

	// ��dll�ļ������п���ÿ��section���ڣ������ݵ��µ��ڴ�����
	PIMAGE_OPTIONAL_HEADER OptionalHeader = (PIMAGE_OPTIONAL_HEADER)((POINTER_TYPE)pFileBuff + pDosHeader->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((POINTER_TYPE)OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);

	for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++, pSectionHeader++)
	{
		char* x = (char*)NewFileBuff + pSectionHeader->VirtualAddress;
		char* y = (char*)pFileBuff + pSectionHeader->PointerToRawData;
		memcpy(x, y, pSectionHeader->SizeOfRawData);
	}

	return (POINTER_TYPE)NewFileBuff;
}

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
char* PE::ImageBuff_To_FileBuff(char* imgbuffer, DWORD length)
{
	char* pFileBuffer = NULL;
	//LPVOID pImageBuffer = NULL;
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)imgbuffer;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)imgbuffer + pDosHeader->e_lfanew);
	PIMAGE_OPTIONAL_HEADER OptionalHeader = (PIMAGE_OPTIONAL_HEADER)((ULONG_PTR)imgbuffer + pDosHeader->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((ULONG_PTR)OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);

	PIMAGE_SECTION_HEADER pSec_temp;

	//���㻹ԭ���ļ��Ĵ�С( = �������һ�ڵ��ļ�ƫ�� + �ļ������Ĵ�С)
	//for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++)
	//{
	//	pSec_temp = pSectionHeader + i;
	//	if (pNtHeader->FileHeader.NumberOfSections - 1 == i) {
	//		int temp_length = pSec_temp->PointerToRawData + pSec_temp->SizeOfRawData;//�����ļ��ĳ���
	//		
	//	}
	//		
	//		
	//}
	
	
	//pFileBuffer = new char[length];
	pFileBuffer = m_alloc.auto_malloc<CHAR*>(length);
	if (pFileBuffer == NULL)
	{
		printf("�ڴ�����ʧ��!");
		return 0;
	}
	memset(pFileBuffer, 0, length);
	memcpy(pFileBuffer, imgbuffer, OptionalHeader->SizeOfHeaders);
	for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++, pSectionHeader++)
	{
		char* x = (char*)pFileBuffer + pSectionHeader->PointerToRawData;
		char* y = (char*)imgbuffer + pSectionHeader->VirtualAddress;
		memcpy(x, y, pSectionHeader->SizeOfRawData);
	}

	return pFileBuffer;

}



/*////////////////////////////////////////////////////////////////
*����*  FullName:		GET_HEADER_DICTIONARY
*����*  ����	:		��ȡĿ¼��ĵ�ַ
*����*  Returns:		�ɹ��򷵻�Ҫ��ѯ������Ŀ¼����ڴ�ƫ��,��Ϊ���򷵻�0
*����*  Parameter_1:	module��ģ����׵�ַ
*����*  Parameter_2:	idx,�����±�
*����*  Parameter_3:
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
DWORD PE::GET_HEADER_DICTIONARY(POINTER_TYPE module, int idx)
{

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(module + pDosHeader->e_lfanew);

	//��ȡĿ¼��ͷָ��
	PIMAGE_DATA_DIRECTORY pDataDirectory = pNtHeader->OptionalHeader.DataDirectory;
	if (pDataDirectory[idx].VirtualAddress==0)
	{
		return 0;
	}
	DWORD res=pDataDirectory[idx].VirtualAddress;

	return res;
}


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
bool PE::GetPEInformation_(TCHAR * FilePath, _Out_ PEInformation * pPEInfor)
{

//------------------------------------------------------------------------------------
	////1.1 ��ȡ�ļ����
	//HANDLE hFile = CreateFile(
	//	FilePath,
	//	GENERIC_READ,
	//	0,
	//	NULL,
	//	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	////1.2 ��ȡ�ļ���С
	//DWORD dwFileSize = GetFileSize(hFile, NULL);
	////CHAR *pFileBuf = new CHAR[dwFileSize];

	//CHAR *pFileBuf = m_alloc.auto_malloc<CHAR*>(dwFileSize);
	//memset(pFileBuf, 0, dwFileSize);

	////1.3 ���ļ���ȡ���ڴ�
	//DWORD ReadSize = 0;
	//ReadFile(hFile, pFileBuf, dwFileSize, (LPDWORD)&ReadSize, NULL);

	////1.4 �رվ��
	//CloseHandle(hFile);
//-----------------------------------------------------------------------------

	FILE* fp;
	char inpath[265];
	memset(inpath, 0, sizeof(inpath) / sizeof(char));;
	//Unicode��ʽת��Ϊ���ַ���
	WideCharToMultiByte(CP_ACP, 0, (wchar_t*)FilePath, -1, inpath, _countof(inpath), NULL, NULL);

	if ((fp = fopen(FilePath, "rb")) == NULL)
	{
		MessageBox(NULL, "�ļ���ʧ��!", NULL, NULL);
		return 0;
	}

	fseek(fp, 0, SEEK_END);//ָ��ָ��β��
	DWORD dwFileSize = ftell(fp);		//��¼�ļ��ĳ���
	fseek(fp, 0, SEEK_SET); //�ָ�ָ�룬ָ��ʼλ��

	//dwFileSize=AlignSize(dwFileSize, 0x1000);

	CHAR* pFileBuf = m_alloc.auto_malloc<CHAR*>(dwFileSize);

	if (!pFileBuf)
	{
		MessageBox(NULL, "there is insufficient memory available!", NULL, NULL);
		fclose(fp);
		return 0;
	}
	//���ļ���ȡ������
	fread(pFileBuf, dwFileSize, 1, fp);
	//�رվ��
	fclose(fp);



	//2.1 �ж��Ƿ�ΪPE�ļ�
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuf;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		MessageBoxA(NULL, "����MZ��ͷ", "��ʾ", MB_OK);
		return 0;
	}
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFileBuf + pDosHeader->e_lfanew);
	PIMAGE_OPTIONAL_HEADER OptionalHeader = (PIMAGE_OPTIONAL_HEADER)((POINTER_TYPE)pFileBuf + pDosHeader->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((POINTER_TYPE)OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);

	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		MessageBoxA(NULL, "����PE�ļ�", "��ʾ", MB_OK);
		return 0;
	}

	//2.2 ��PE��Ϣ����PEInformation�ṹ����
	pPEInfor->FileBuffer = (POINTER_TYPE)pFileBuf;
	pPEInfor->FileSize = dwFileSize;
	pPEInfor->AddressOfEntryPoint = pNtHeader->OptionalHeader.AddressOfEntryPoint;
	// ����������ʼRVA
	pPEInfor->BaseOfCode = pNtHeader->OptionalHeader.BaseOfCode;

#ifdef _WIN64

#else
	pPEInfor->BaseOfData = pNtHeader->OptionalHeader.BaseOfData;
#endif

	pPEInfor->pNtHeader = pNtHeader;
	pPEInfor->OptionalHeader = OptionalHeader;
	pPEInfor->pSectionHeader = pSectionHeader;
	pPEInfor->DataDirectory = pNtHeader->OptionalHeader.DataDirectory;
	pPEInfor->e_lfanes = pDosHeader->e_lfanew;
	pPEInfor->FileAlignment = pNtHeader->OptionalHeader.FileAlignment;
	//����Ĭ���������ַ
	pPEInfor->ImageBase = pNtHeader->OptionalHeader.ImageBase;
	pPEInfor->NumberOfSections = pNtHeader->FileHeader.NumberOfSections;
	pPEInfor->SectionAlignment = pNtHeader->OptionalHeader.SectionAlignment;
	pPEInfor->SizeOfCode = pNtHeader->OptionalHeader.SizeOfCode;
	pPEInfor->SizeOfHeaders = pNtHeader->OptionalHeader.SizeOfHeaders;
	// �ļ������ڴ��Ĵ�С
	pPEInfor->SizeofImage = pNtHeader->OptionalHeader.SizeOfImage;
	pPEInfor->SizeOfOptionHeaders = pNtHeader->FileHeader.SizeOfOptionalHeader;

	return 1;
}

/*////////////////////////////////////////////////////////////////
*����*  FullName:		GetPEInformation_1
*����*  ����	:		�����ڴ�ģ�飬��ȡPE�ļ��ĸ�����Ϣ
*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
*����*  Parameter_1:	pFilebuff��ģ��ĵ�ַ
*����*  Parameter_2:	pPEInfor������������ѵõ���PE��Ϣ��ŵ�pPEInfor�ṹ����
*����*  Parameter_3:	dwFileSize��ģ���С
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
bool PE::GetPEInformation_1(char * pFilebuff, PEInformation * pPEInfor, DWORD dwFileSize)
{
	//2.1 �ж��Ƿ�ΪPE�ļ�
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFilebuff;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		MessageBoxA(NULL, "����MZ��ͷ", "��ʾ", MB_OK);
		return 0;
	}
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFilebuff + pDosHeader->e_lfanew);
	PIMAGE_OPTIONAL_HEADER OptionalHeader = (PIMAGE_OPTIONAL_HEADER)((POINTER_TYPE)pFilebuff + pDosHeader->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((POINTER_TYPE)OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);
	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		MessageBoxA(NULL, "����PE�ļ�", "��ʾ", MB_OK);
		return 0;
	}

	//2.2 ��PE��Ϣ����PEInformation�ṹ����
	pPEInfor->FileBuffer = (POINTER_TYPE)pFilebuff;
	pPEInfor->FileSize = dwFileSize;
	pPEInfor->AddressOfEntryPoint = pNtHeader->OptionalHeader.AddressOfEntryPoint;
	pPEInfor->BaseOfCode = pNtHeader->OptionalHeader.BaseOfCode;

#ifdef _WIN64

#else
	pPEInfor->BaseOfData = pNtHeader->OptionalHeader.BaseOfData;
#endif

	pPEInfor->pNtHeader = pNtHeader;
	pPEInfor->OptionalHeader = OptionalHeader;
	pPEInfor->pSectionHeader = pSectionHeader;
	pPEInfor->DataDirectory = pNtHeader->OptionalHeader.DataDirectory;
	pPEInfor->e_lfanes = pDosHeader->e_lfanew;
	pPEInfor->FileAlignment = pNtHeader->OptionalHeader.FileAlignment;
	pPEInfor->ImageBase = pNtHeader->OptionalHeader.ImageBase;
	pPEInfor->NumberOfSections = pNtHeader->FileHeader.NumberOfSections;
	pPEInfor->SectionAlignment = pNtHeader->OptionalHeader.SectionAlignment;
	pPEInfor->SizeOfCode = pNtHeader->OptionalHeader.SizeOfCode;
	pPEInfor->SizeOfHeaders = pNtHeader->OptionalHeader.SizeOfHeaders;
	pPEInfor->SizeofImage = pNtHeader->OptionalHeader.SizeOfImage;
	pPEInfor->SizeOfOptionHeaders = pNtHeader->FileHeader.SizeOfOptionalHeader;

	return 1;
}



/*////////////////////////////////////////////////////////////////
*����*  FullName:		addSeciton
*����*  ����	:		����½�
*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
*����*  Parameter_1:	pFileBuff��ģ���ַ
*����*  Parameter_2:	AddSize��Ҫ��ӵĴ�С
*����*  Parameter_3:	secname���½����ƣ������ڰ˸��ֽ���
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
bool PE::addSeciton(POINTER_TYPE pFileBuff, DWORD AddSize, char secname[8])
{
	/*if (secname[7] !=0xCC)
	{
		MessageBoxA(NULL, "�½ڵ����Ƴ���8���ֽ�\r\n�½����ʧ��!", "��ʾ", MB_ICONWARNING);
		return false;
	}*/

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFileBuff + pDosHeader->e_lfanew);

	PIMAGE_OPTIONAL_HEADER OptionalHeader = (PIMAGE_OPTIONAL_HEADER)((POINTER_TYPE)pFileBuff + pDosHeader->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((POINTER_TYPE)OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);

	//�жϽڱ�β���Ƿ���80���ֽڵĿ�������
	PIMAGE_SECTION_HEADER pse_temp = pSectionHeader + pNtHeader->FileHeader.NumberOfSections;//�ڱ������ڵ��׵�ַ
	PIMAGE_SECTION_HEADER pse_temp_b = pSectionHeader + pNtHeader->FileHeader.NumberOfSections - 1;//�ڱ����һ�ڵ��׵�ַ
	int space = OptionalHeader->SizeOfHeaders - ((POINTER_TYPE)pse_temp - (POINTER_TYPE)pFileBuff);

	//space�ռ�ָ���ǽڱ������0,û���������ݣ�������������ݣ���ʹ��������80���ֽڵĿռ䣬Ҳ����������ݣ��������ƻ���������
	//����ռ䲻��,��ô��docͷ������������ݲ�Ҫ�ˣ������PEͷ ��׼PEͷ �ͽڱ�����;
	//����Ҫ�ж�pDosHeader->e_lfanew-64��ξ����Ƿ����80����������û������
	if (pDosHeader->e_lfanew - 64 > 80 && space < 80)
	{
		int len = ((POINTER_TYPE*)pse_temp - (POINTER_TYPE*)&(pNtHeader->Signature)) * 4;
		for (int i = 0; i < len; i++)
		{
			*((char*)pFileBuff + 64 + i) = *((char*)pFileBuff + i + pDosHeader->e_lfanew);
		}
		pDosHeader->e_lfanew = 0x40;

		pse_temp = pSectionHeader + pNtHeader->FileHeader.NumberOfSections;
		pse_temp_b = pSectionHeader + pNtHeader->FileHeader.NumberOfSections - 1;
		for (int i = 0; i < 80; i++)//�ѽڱ�β��80���ֽڸ�ʽ��
			*((char*)pFileBuff + 64 + i + len) = 0;
		space = pDosHeader->e_lfanew - 64;//����space�ռ�
	}
	if (space > 80)
	{
		BYTE(*p)[8] = &(pse_temp->Name);

		for (int i = 0; i < strlen(secname); i++)
		{
			p[0][i] = secname[i];
		}

		pse_temp->Misc.VirtualSize = AddSize;

		if (pse_temp_b->Misc.VirtualSize > pse_temp_b->SizeOfRawData)//�Ƚ��ڴ��С���ļ���С
		{
			//����ڴ��С�Ƿ����
			pse_temp->VirtualAddress =
				((pse_temp_b->Misc.VirtualSize % OptionalHeader->SectionAlignment) == 0) ? (pse_temp_b->Misc.VirtualSize+ pse_temp_b->VirtualAddress) :
				(pse_temp_b->Misc.VirtualSize - pse_temp_b->Misc.VirtualSize % OptionalHeader->SectionAlignment + OptionalHeader->SectionAlignment) +
				pse_temp_b->VirtualAddress;
			printf("%X\n", *&(pse_temp->VirtualAddress));
		}
		else
		{
			pse_temp->VirtualAddress =
				((pse_temp_b->SizeOfRawData % OptionalHeader->SectionAlignment) == 0) ? (pse_temp_b->SizeOfRawData+pse_temp_b->VirtualAddress) :
				(pse_temp_b->SizeOfRawData - pse_temp_b->SizeOfRawData % OptionalHeader->SectionAlignment + OptionalHeader->SectionAlignment) +
				pse_temp_b->VirtualAddress;
		}

		pse_temp->SizeOfRawData = AddSize;
		pse_temp->PointerToRawData = pse_temp_b->SizeOfRawData + pse_temp_b->PointerToRawData;
		pse_temp->Characteristics = 0xE0000020;
		pNtHeader->FileHeader.NumberOfSections = pNtHeader->FileHeader.NumberOfSections + 1;
		pNtHeader->OptionalHeader.SizeOfImage = pNtHeader->OptionalHeader.SizeOfImage + AddSize;
	}
	else
	{
		printf("��ӽ�ʧ��,û�пռ�����µĽ�!");
		return false;
	}

	return true;
}

