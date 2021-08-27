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

//PE结构的信息结构体
typedef struct PEInformation
{
	POINTER_TYPE FileBuffer;			//文件在内存里的地址
	POINTER_TYPE FileSize;				//文件大小
	POINTER_TYPE e_lfanes;				//PE文件头地址
	POINTER_TYPE NumberOfSections;		//区段数目
	POINTER_TYPE SizeOfOptionHeaders;	//可选头大小
	POINTER_TYPE SizeOfCode;			//代码节大小
	POINTER_TYPE AddressOfEntryPoint;	//OEP(RVA)入口点
	POINTER_TYPE BaseOfCode;			//代码基址
	POINTER_TYPE BaseOfData;			//数据基址
	POINTER_TYPE ImageBase;				//镜像基址
	POINTER_TYPE SectionAlignment;		//内存对齐
	POINTER_TYPE FileAlignment;			//文件对齐
	POINTER_TYPE SizeofImage;			//镜像大小
	POINTER_TYPE SizeOfHeaders;			//头大小
	IMAGE_DATA_DIRECTORY* DataDirectory;	//目录
	PIMAGE_NT_HEADERS pNtHeader;			//Nt头
	PIMAGE_SECTION_HEADER pSectionHeader;	//节头
	PIMAGE_OPTIONAL_HEADER OptionalHeader;	//可选PE头

}PEInfo, *pPEInfo;


class PE:public AllocMemory
{
	AllocMemory m_alloc;
	TCHAR s[100];//时间戳
public:
	PE();
	~PE();
public:
	//时间戳转换为标准时间
	TCHAR* Stamp_To_Standard(DWORD stampTime);
public:
	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		AlignSize － 取整对齐函数
	*※※*  Returns:		返回对齐后的数值
	*※※*  Parameter_1:	uSize,输入的数值
	*※※*  Parameter_2:	uSecAlignment，文件对齐或者内存对齐数值
	*※※*  Parameter_3:
	*※※*  Parameter_4:
	*※※*	Parameter_5:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	ULONG_PTR AlignSize(_In_ ULONG_PTR uSize, ULONG_PTR uSecAlignment)
	{
		//return (uSize % uSecAlignment == 0) ? uSize : (uSize - (uSize % uSecAlignment) + uSecAlignment);
		return ((uSize + uSecAlignment - 1) / uSecAlignment * uSecAlignment);
	};


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		IsPEFile - 判断是否是64位或者32位的PE文件
	*※※*  Returns:		成功返回1，失败返回0
	*※※*  Parameter_1:	pFileBuffer,模块文件在内存中的地址
	*※※*  Parameter_2:	hwndDlg，可选参数，是窗口句柄
	*※※*  Parameter_3:
	*※※*  Parameter_4:
	*※※*	Parameter_5:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOLEAN IsPEFile(UCHAR* pFileBuffer, HWND hwndDlg = 0);

public:

	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:	PerformBaseRelocation - 修复重定位表	
	*※※*  Returns:	无
	*※※*  Parameter:	char* buff,PE文件首地址(拉伸后)
	*※※*  Parameter:	DWORD Value，PE基址与当前在内存中的地址的差值
	*※※*  Parameter:
	*※※*  Parameter:
	*※※*	Parameter:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	void PerformBaseRelocation(POINTER_TYPE buff, POINTER_TYPE Value);


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:	RebuildImportTable - 修复IAT表
	*※※*  Returns:	成功返回1，失败返回0
	*※※*  Parameter:	char* buff，PE文件在内存中的地址(拉伸后)
	*※※*  Parameter:
	*※※*  Parameter:
	*※※*  Parameter:
	*※※*	Parameter:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL RebuildImportTable(POINTER_TYPE buff);


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		StretchFile - 拉伸文件	
	*※※*  Returns:		成功返回新地址，失败返回0
	*※※*  Parameter_1:	pFileBuff，模块地址
	*※※*  Parameter_2:	FileSize,镜像大小
	*※※*  Parameter_3:
	*※※*  Parameter_4:
	*※※*	Parameter_5:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	POINTER_TYPE StretchFile(POINTER_TYPE pFileBuff, DWORD FileSize);

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
	char* ImageBuff_To_FileBuff(char* imgbuffer, DWORD length);



	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		GET_HEADER_DICTIONARY
	*※※*  功能	:		获取目录表的地址
	*※※*  Returns:		成功则返回要查询的那张目录表的内存偏移
	*※※*  Parameter_1:	module，模块的地址
	*※※*  Parameter_2:	idx,要查询哪张表
	*※※*  Parameter_3:
	*※※*  Parameter_4:
	*※※*	Parameter_5:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	DWORD GET_HEADER_DICTIONARY(POINTER_TYPE module,int idx);


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
	bool GetPEInformation_(TCHAR * FilePath, _Out_ PEInformation * pPEInfor);


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		GetPEInformation_1
	*※※*  功能	:		根据内存模块，获取PE文件的各种信息
	*※※*  Returns:		成功返回1，失败返回0
	*※※*  Parameter_1:	pFilebuff，模块的地址
	*※※*  Parameter_2:	pPEInfor，输出参数，把得到的PE信息存放到pPEInfor结构体里
	*※※*  Parameter_3:	dwFileSize，模块的文件大小
	*※※*  Parameter_4:
	*※※*	Parameter_5:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	bool GetPEInformation_1(char* pFilebuff, _Out_ PEInformation * pPEInfor, _In_ DWORD dwFileSize=0);


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		addSeciton
	*※※*  功能	:		添加新节
	*※※*  Returns:		成功返回1，失败返回0
	*※※*  Parameter_1:	pFileBuff，模块地址
	*※※*  Parameter_2:	AddSize，要添加的大小
	*※※*  Parameter_3:	secname[8]，新节名称，限制在八个字节内
	*※※*  Parameter_4:
	*※※*	Parameter_5:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	bool addSeciton(POINTER_TYPE pFileBuff, DWORD AddSize, char secname[8] = { 0 });

};

