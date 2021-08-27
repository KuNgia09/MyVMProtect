#include "PE.h"
#include <iostream>
#include <time.h>
PE::PE()
{
}

PE::~PE()
{
}

//时间戳转换为标准时间
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
		MessageBoxA(hwndDlg, "不是有效的MZ标志!", "提示", NULL);
		return FALSE;
	}

	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((ULONGLONG)pDosHeader + pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		MessageBoxA(hwndDlg, "不是有效的PE标志!", "提示", NULL);
		return FALSE;
	}
#ifdef _WIN64
	if (pNTHeader->OptionalHeader.Magic != 0x20B)
	{
		MessageBoxA(hwndDlg, "不是有效的64位PE程序!", "提示", NULL);
		return FALSE;
	}
#else
	if (pNTHeader->OptionalHeader.Magic != 0x10B)
	{
		MessageBoxA(hwndDlg, "不是有效的32位PE程序!", "提示", NULL);
		return FALSE;
	}
#endif // _WIN64

	

	return TRUE;
}

/*////////////////////////////////////////////////////////////////
*※※*  FullName:	PerformBaseRelocation
*※※*  功能	:	修复重定位表
*※※*  Returns:	无
*※※*  Parameter:	char* buff,PE文件首地址
*※※*  Parameter:	POINTER_TYPE Value，buff的基址与贴在内存中的地址的差值
*※※*  Parameter:
*※※*  Parameter:
*※※*	Parameter:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
//#ifdef _WIN64
void PE::PerformBaseRelocation(POINTER_TYPE buff, POINTER_TYPE Value)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)buff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(buff + pDosHeader->e_lfanew);

	//获取目录表头指针
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
*※※*  FullName:	RebuildImportTable
*※※*  功能	:	修复IAT表
*※※*  Returns:	成功返回1，失败返回0
*※※*  Parameter:	char* buff，PE文件在内存中的地址(拉伸后)
*※※*  Parameter:
*※※*  Parameter:
*※※*  Parameter:
*※※*	Parameter:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
BOOL PE::RebuildImportTable(POINTER_TYPE buff)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)buff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(buff + pDosHeader->e_lfanew);
	int result = 1;
	//获取目录表头指针
	PIMAGE_DATA_DIRECTORY pDataDirectory = pNtHeader->OptionalHeader.DataDirectory;

	if (pDataDirectory[1].Size > 0)
	{
		//获取导入表地址
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
*※※*  FullName:		StretchFile
*※※*  功能	:		拉伸文件
*※※*  Returns:		成功返回新地址，失败返回0
*※※*  Parameter_1:	pFileBuff，模块地址
*※※*  Parameter_2:	FileSize,镜像大小
*※※*  Parameter_3:
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
POINTER_TYPE PE::StretchFile(POINTER_TYPE pFileBuff, DWORD FileSize)
{

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFileBuff + pDosHeader->e_lfanew);

	//1.1 根据内存大小 申请内存
	//char* NewFileBuff = new char[FileSize];
	char* NewFileBuff= m_alloc.auto_malloc<CHAR*>(FileSize);
	if (NewFileBuff == NULL)
	{
		printf("内存申请失败!");
		return 0;
	}
	memset(NewFileBuff, 0, FileSize);

	//1.2 拉伸文件
	 // 拷贝DOS头 + DOS STUB + PE头到headers地址处
	memcpy(NewFileBuff, pDosHeader, pNtHeader->OptionalHeader.SizeOfHeaders);

	// 从dll文件内容中拷贝每个section（节）的数据到新的内存区域
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
*※※*  FullName:		ImageBuff_To_FileBuff - 把PE文件还原成文件磁盘大小
*※※*  Returns:		成功返回新地址，失败返回0
*※※*  Parameter_1:	imgbuffer，模块地址
*※※*  Parameter_2:	length，文件大小
*※※*  Parameter_3:
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
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

	//计算还原后文件的大小( = 计算最后一节的文件偏移 + 文件对齐后的大小)
	//for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++)
	//{
	//	pSec_temp = pSectionHeader + i;
	//	if (pNtHeader->FileHeader.NumberOfSections - 1 == i) {
	//		int temp_length = pSec_temp->PointerToRawData + pSec_temp->SizeOfRawData;//更新文件的长度
	//		
	//	}
	//		
	//		
	//}
	
	
	//pFileBuffer = new char[length];
	pFileBuffer = m_alloc.auto_malloc<CHAR*>(length);
	if (pFileBuffer == NULL)
	{
		printf("内存申请失败!");
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
*※※*  FullName:		GET_HEADER_DICTIONARY
*※※*  功能	:		获取目录表的地址
*※※*  Returns:		成功则返回要查询的那张目录表的内存偏移,表为空则返回0
*※※*  Parameter_1:	module，模块的首地址
*※※*  Parameter_2:	idx,查表的下标
*※※*  Parameter_3:
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
DWORD PE::GET_HEADER_DICTIONARY(POINTER_TYPE module, int idx)
{

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(module + pDosHeader->e_lfanew);

	//获取目录表头指针
	PIMAGE_DATA_DIRECTORY pDataDirectory = pNtHeader->OptionalHeader.DataDirectory;
	if (pDataDirectory[idx].VirtualAddress==0)
	{
		return 0;
	}
	DWORD res=pDataDirectory[idx].VirtualAddress;

	return res;
}


/*////////////////////////////////////////////////////////////////
*※※*  FullName:		GetPEInformation_
*※※*  功能	:		打开一个文件，拷贝进内存，获取PE文件的各种信息
*※※*  Returns:		成功返回1，失败返回0
*※※*  Parameter_1:	FilePath,文件路径
*※※*  Parameter_2:	pPEInfor，输出参数,把得到的PE信息存放到pPEInfor结构体里
*※※*  Parameter_3:
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
bool PE::GetPEInformation_(TCHAR * FilePath, _Out_ PEInformation * pPEInfor)
{

//------------------------------------------------------------------------------------
	////1.1 获取文件句柄
	//HANDLE hFile = CreateFile(
	//	FilePath,
	//	GENERIC_READ,
	//	0,
	//	NULL,
	//	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	////1.2 获取文件大小
	//DWORD dwFileSize = GetFileSize(hFile, NULL);
	////CHAR *pFileBuf = new CHAR[dwFileSize];

	//CHAR *pFileBuf = m_alloc.auto_malloc<CHAR*>(dwFileSize);
	//memset(pFileBuf, 0, dwFileSize);

	////1.3 将文件读取到内存
	//DWORD ReadSize = 0;
	//ReadFile(hFile, pFileBuf, dwFileSize, (LPDWORD)&ReadSize, NULL);

	////1.4 关闭句柄
	//CloseHandle(hFile);
//-----------------------------------------------------------------------------

	FILE* fp;
	char inpath[265];
	memset(inpath, 0, sizeof(inpath) / sizeof(char));;
	//Unicode格式转化为多字符集
	WideCharToMultiByte(CP_ACP, 0, (wchar_t*)FilePath, -1, inpath, _countof(inpath), NULL, NULL);

	if ((fp = fopen(FilePath, "rb")) == NULL)
	{
		MessageBox(NULL, "文件打开失败!", NULL, NULL);
		return 0;
	}

	fseek(fp, 0, SEEK_END);//指针指向尾部
	DWORD dwFileSize = ftell(fp);		//记录文件的长度
	fseek(fp, 0, SEEK_SET); //恢复指针，指向开始位置

	//dwFileSize=AlignSize(dwFileSize, 0x1000);

	CHAR* pFileBuf = m_alloc.auto_malloc<CHAR*>(dwFileSize);

	if (!pFileBuf)
	{
		MessageBox(NULL, "there is insufficient memory available!", NULL, NULL);
		fclose(fp);
		return 0;
	}
	//将文件读取缓冲区
	fread(pFileBuf, dwFileSize, 1, fp);
	//关闭句柄
	fclose(fp);



	//2.1 判断是否为PE文件
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuf;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		MessageBoxA(NULL, "不是MZ开头", "提示", MB_OK);
		return 0;
	}
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFileBuf + pDosHeader->e_lfanew);
	PIMAGE_OPTIONAL_HEADER OptionalHeader = (PIMAGE_OPTIONAL_HEADER)((POINTER_TYPE)pFileBuf + pDosHeader->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((POINTER_TYPE)OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);

	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		MessageBoxA(NULL, "不是PE文件", "提示", MB_OK);
		return 0;
	}

	//2.2 把PE信息存入PEInformation结构体里
	pPEInfor->FileBuffer = (POINTER_TYPE)pFileBuf;
	pPEInfor->FileSize = dwFileSize;
	pPEInfor->AddressOfEntryPoint = pNtHeader->OptionalHeader.AddressOfEntryPoint;
	// 代码区块起始RVA
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
	//程序默认载入基地址
	pPEInfor->ImageBase = pNtHeader->OptionalHeader.ImageBase;
	pPEInfor->NumberOfSections = pNtHeader->FileHeader.NumberOfSections;
	pPEInfor->SectionAlignment = pNtHeader->OptionalHeader.SectionAlignment;
	pPEInfor->SizeOfCode = pNtHeader->OptionalHeader.SizeOfCode;
	pPEInfor->SizeOfHeaders = pNtHeader->OptionalHeader.SizeOfHeaders;
	// 文件载入内存后的大小
	pPEInfor->SizeofImage = pNtHeader->OptionalHeader.SizeOfImage;
	pPEInfor->SizeOfOptionHeaders = pNtHeader->FileHeader.SizeOfOptionalHeader;

	return 1;
}

/*////////////////////////////////////////////////////////////////
*※※*  FullName:		GetPEInformation_1
*※※*  功能	:		根据内存模块，获取PE文件的各种信息
*※※*  Returns:		成功返回1，失败返回0
*※※*  Parameter_1:	pFilebuff，模块的地址
*※※*  Parameter_2:	pPEInfor，输出参数，把得到的PE信息存放到pPEInfor结构体里
*※※*  Parameter_3:	dwFileSize，模块大小
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
bool PE::GetPEInformation_1(char * pFilebuff, PEInformation * pPEInfor, DWORD dwFileSize)
{
	//2.1 判断是否为PE文件
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFilebuff;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		MessageBoxA(NULL, "不是MZ开头", "提示", MB_OK);
		return 0;
	}
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFilebuff + pDosHeader->e_lfanew);
	PIMAGE_OPTIONAL_HEADER OptionalHeader = (PIMAGE_OPTIONAL_HEADER)((POINTER_TYPE)pFilebuff + pDosHeader->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((POINTER_TYPE)OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);
	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		MessageBoxA(NULL, "不是PE文件", "提示", MB_OK);
		return 0;
	}

	//2.2 把PE信息存入PEInformation结构体里
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
*※※*  FullName:		addSeciton
*※※*  功能	:		添加新节
*※※*  Returns:		成功返回1，失败返回0
*※※*  Parameter_1:	pFileBuff，模块地址
*※※*  Parameter_2:	AddSize，要添加的大小
*※※*  Parameter_3:	secname，新节名称，限制在八个字节内
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
bool PE::addSeciton(POINTER_TYPE pFileBuff, DWORD AddSize, char secname[8])
{
	/*if (secname[7] !=0xCC)
	{
		MessageBoxA(NULL, "新节的名称超出8个字节\r\n新节添加失败!", "提示", MB_ICONWARNING);
		return false;
	}*/

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pFileBuff + pDosHeader->e_lfanew);

	PIMAGE_OPTIONAL_HEADER OptionalHeader = (PIMAGE_OPTIONAL_HEADER)((POINTER_TYPE)pFileBuff + pDosHeader->e_lfanew + 4 + IMAGE_SIZEOF_FILE_HEADER);
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((POINTER_TYPE)OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);

	//判断节表尾部是否有80个字节的空闲区域
	PIMAGE_SECTION_HEADER pse_temp = pSectionHeader + pNtHeader->FileHeader.NumberOfSections;//节表新增节的首地址
	PIMAGE_SECTION_HEADER pse_temp_b = pSectionHeader + pNtHeader->FileHeader.NumberOfSections - 1;//节表最后一节的首地址
	int space = OptionalHeader->SizeOfHeaders - ((POINTER_TYPE)pse_temp - (POINTER_TYPE)pFileBuff);

	//space空间指的是节表最后都是0,没有其他数据，如果有其他数据，则即使满足了有80个字节的空间，也不能添加数据，这样会破坏其他数据
	//如果空间不够,那么，doc头下面的垃圾数据不要了，后面的PE头 标准PE头 和节表上提;
	//并且要判断pDosHeader->e_lfanew-64这段距离是否大于80，否则上提没有意义
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
		for (int i = 0; i < 80; i++)//把节表尾部80个字节格式化
			*((char*)pFileBuff + 64 + i + len) = 0;
		space = pDosHeader->e_lfanew - 64;//更新space空间
	}
	if (space > 80)
	{
		BYTE(*p)[8] = &(pse_temp->Name);

		for (int i = 0; i < strlen(secname); i++)
		{
			p[0][i] = secname[i];
		}

		pse_temp->Misc.VirtualSize = AddSize;

		if (pse_temp_b->Misc.VirtualSize > pse_temp_b->SizeOfRawData)//比较内存大小和文件大小
		{
			//检测内存大小是否对齐
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
		printf("添加节失败,没有空间添加新的节!");
		return false;
	}

	return true;
}

