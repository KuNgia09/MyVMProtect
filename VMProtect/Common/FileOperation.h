#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <io.h>
#include "AllocMemory.h"
using namespace std;

#ifdef _WIN64
#define POINTER_TYPE ULONGLONG
#else
#define POINTER_TYPE DWORD
#endif

class FileOperation:public AllocMemory
{
public:
	FileOperation();
	virtual ~FileOperation();
public:
	AllocMemory m_alloc;

public:

	/*
	※※* 功能：		打开目录，获取目录的路径
	※※* @outPath[]:	输出参数，把得到的目录字符串赋值给这个参数
	*/
	void GetDirPath_(_Out_ TCHAR* outPath);


	/*
	※※* 功能：		打开文件，获取文件的路径
	※※* @outPath[]:	输出参数，把得到的目录字符串赋值给这个参数
	*/
	void GetFilePath_(_Out_ TCHAR* outPath);


	/*
	※※* 功能:			扣取内存中的二进制，保存为文件
	※※* @buffer[]:	内存地址
	※※* @nLength[]:	大小
	※※* @OutPath[]:	文件的输出路径
	*/
	bool SaveFile_(char* buffer, POINTER_TYPE nLength, TCHAR* OutPath);


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		枚举目录下的文件,放到vector容器里(不包含子文件里的文件)
	*※※*  Returns:		成功返回1，失败返回0
	*※※*  Parameter:		dir参数，目录路径
	*※※*  Parameter:		obj输出参数，把遍历到的文件存放到vector容器里
	*※※*  Parameter:
	*※※*  Parameter:
	*※※*	Parameter:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	bool EnumFiles(const TCHAR * dir,_Out_ vector<TCHAR*> *obj);

	/*
	把string转换为char*
	例子：
	vector<string>obj;
	std::string str = obj[i]; //obj[i]  i是数组下标 表示的是vector容器里的第几个字符串
	char* p1 = str.c_str();
	或者char* p2 = str.data();
 */


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:	    创建子进程
	*※※*  Returns:	    成功返回1，失败返回0
	*※※*  Parameter:	    szFuffer参数，文件路径(可以输入命令行参数)
	*※※*  Parameter:	    flag可选参数，填1为隐藏子进程，默认不隐藏
	*※※*  Parameter:	    ppi可选输出参数，输出子进程基本信息
	*※※*  Parameter:
	*※※*	Parameter:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	bool CreateSubProcess(TCHAR* szBuffer, int flag = 0, PROCESS_INFORMATION * ppi = 0);


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		把文件以二进制的方式读到内存里
	*※※*  Returns:		成功返回指向这段内存数据的首地址，失败返回0
	*※※*  Parameter:		path参数，是打开的文件路径
	*※※*  Parameter:		Length输出参数，文件的大小
	*※※*  Parameter:
	*※※*  Parameter:
	*※※*	Parameter:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	LPVOID FileReadMomery(TCHAR* path, int* Length);


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		删除文件里的子文件夹和文件
	*※※*  Returns:
	*※※*  Parameter:		szDir参数，文件夹的绝对路径
	*※※*  Parameter:		pfilename可选参数，过滤文件(不删除)，文件名的绝对路径
	*※※*  Parameter:
	*※※*  Parameter:
	*※※*	Parameter:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL DeleteDirectory(LPCTSTR szDir, TCHAR* pfilename = 0);


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		EnumFile_Total - 遍历文件下的所有文件(包括子文件夹下的文件)
	*※※*  Returns:		成功返回1，失败返回0
	*※※*  Parameter:		v_szOutFile输出参数，遍历到的所有的文件的路径保存在v_szOutFile
	*※※*  Parameter:
	*※※*  Parameter:
	*※※*  Parameter:
	*※※*	Parameter:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL EnumFile_Total(LPCTSTR szDir, _Out_ vector<TCHAR*>*v_szOutFile);


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		EnumSubFolder - 遍历文件下的子文件夹，保存到v_szOutFile(包括子文件下的文件夹)
	*※※*  Returns:		成功返回1，失败返回0
	*※※*  Parameter:		v_szOutFile输出参数，把遍历到的所有子文件夹的路径保存到v_szOutFile
	*※※*  Parameter:
	*※※*  Parameter:
	*※※*  Parameter:
	*※※*	Parameter:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL EnumSubFolder(LPCTSTR szDir, _Out_ vector<TCHAR*>*v_szOutFile);


	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		GetProcessIdByName - 根据进程名称获取进程PID
	*※※*  Returns:		成功返回1，失败返回0
	*※※*  Parameter_1:	szProcessName,进程的名称
	*※※*  Parameter_2:	dwPid，输出参数，进程的PID
	*※※*  Parameter_3:
	*※※*  Parameter_4:
	*※※*	Parameter_5:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL GetProcessIdByName(TCHAR *szProcessName, _Out_ DWORD& dwPid);

	/*////////////////////////////////////////////////////////////////
	*※※*  FullName:		GetAllThreadIdByProcessId - 根据进程PID，获取所有线程Id	
	*※※*  Returns:		成功返回TRUE，失败返回FALSE
	*※※*  Parameter_1:	ppThreadIdList，输出参数，把获取的线程Id放到ppThreadIdList里面
	*※※*  Parameter_2:
	*※※*  Parameter_3:
	*※※*  Parameter_4:
	*※※*	Parameter_5:
	*※※*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL GetAllThreadIdByProcessId(DWORD dwPid, _Out_ vector<DWORD>*ppThreadIdList);


/*////////////////////////////////////////////////////////////////
*※※*  FullName:		GetModuleInf - 根据PID获取进程主模块的基址和大小		
*※※*  Returns:		成功返回1，失败返回0
*※※*  Parameter_1:	PID，进程PID
*※※*  Parameter_2:	BaseAddr，输出参数，主模块的基址
*※※*  Parameter_3:	BaseSize，输出参数，主模块的大小
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
	void GetModuleInf(DWORD PID, _Out_ ULONG_PTR& BaseAddr, _Out_ ULONG_PTR& BaseSize);
};
