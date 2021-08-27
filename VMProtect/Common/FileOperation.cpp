
#include "FileOperation.h"
#include <commdlg.h>
#include <Shlobj.h>
#include <stdio.h>
#include "CString.h"
#include <tchar.h>
#include <TlHelp32.h>

FileOperation::FileOperation()
{

}

FileOperation::~FileOperation()
{

}

/*
※※* 功能：		打开目录，获取目录的路径
※※* @outPath[]:	输出参数，把得到的目录字符串赋值给这个参数
*/
void FileOperation::GetDirPath_(TCHAR * outPath)
{
	BROWSEINFO bi;
	bi.hwndOwner = NULL;
	bi.pidlRoot = CSIDL_DESKTOP;//文件夹的根目录，此处为桌面
	bi.pszDisplayName = NULL;
	bi.lpszTitle = NULL;//显示位于对话框左上部的提示信息
	bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;//有新建文件夹按钮
	bi.lpfn = NULL;
	bi.iImage = 0;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);//调用选择对话框
	if (pidl == NULL)
	{
		MessageBox(NULL, _T("没有选择目录"), _T(""), MB_OK);
		return;
	}
	TCHAR strFolder[MAX_PATH];
	SHGetPathFromIDList(pidl, strFolder);

	memcpy(outPath, strFolder, sizeof(strFolder));
}


/*
※※* 功能：		打开文件，获取文件的路径
※※* @outPath[]:	输出参数，把得到的目录字符串赋值给这个参数
*/
void FileOperation::GetFilePath_(TCHAR * outPath)
{
	OPENFILENAME stOpenFile;
	//要打开文件的类型
	TCHAR szPeFileExt[100] = _T("*.exe;*.dll;*.scr;*.drv;*.sys;*.*");
	TCHAR szFileName[MAX_PATH];

	//文件名称在路径中的开始位置
	DWORD StartFileName = 0;

	//初始化
	memset(szFileName, 0, MAX_PATH);
	memset(&stOpenFile, 0, sizeof(OPENFILENAME));

	//设置stOpenFile结构体里的输入和输出参数
	stOpenFile.lStructSize = sizeof(OPENFILENAME);
	stOpenFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	stOpenFile.lpstrFilter = szPeFileExt;
	stOpenFile.lpstrFile = szFileName; //输出
	stOpenFile.nMaxFile = MAX_PATH;

	//获取文件路径
	GetOpenFileName(&stOpenFile);
	memcpy(outPath, stOpenFile.lpstrFile, stOpenFile.nMaxFile);
}

/*
※※* 功能:		扣取内存中的二进制，保存为文件
※※* @buffer[]:	内存地址
※※* @nLength[]:	大小
※※* @OutPath[]:	输出路径
*/
bool FileOperation::SaveFile_(char* buffer, POINTER_TYPE nLength, TCHAR * OutPath)
{

	FILE* fp;
	errno_t err = 0;
	err = _tfopen_s(&fp, OutPath, _T("wb"));
	if (err != 0)
	{
		//MessageBox(NULL, _T("文件打开失败！！！"), _T("提示"), MB_OK);
		return FALSE;
	}

	if (fwrite((char*)buffer, nLength, 1, fp))
	{
		//MessageBox(NULL, "保存成功", "提示", MB_OK);
	}

	else
	{
		//MessageBox(NULL, _T("保存失败！！！"), _T("提示"), MB_OK);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}



/*////////////////////////////////////////////////////////////////
*※※*  FullName:	枚举目录下的文件名,放到vector容器里(不包含子文件里的文件)
*※※*  Returns:	成功返回1，失败返回0
*※※*  Parameter:	dir参数，目录路径
*※※*  Parameter:	obj输出参数，把遍历到的文件名存放到vector容器里
*※※*  Parameter:
*※※*  Parameter:
*※※*	Parameter:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
bool FileOperation::EnumFiles(const TCHAR * dir, vector<TCHAR*>* obj)
{
	intptr_t handle;
	_tfinddata_t findData;
	CString str = dir;
	str += _T("\\*.*");
	
	handle = _tfindfirst(str.GetString(), &findData);    // 查找目录中的第一个文件
	if (handle == -1)
	{
		MessageBoxA(NULL, "目录下的文件为空!!!", "错误", MB_OK);
		return FALSE;
	}

	do
	{
		if (findData.attrib & _A_SUBDIR
			|| _tcscmp(findData.name, _T(".")) == 0
			|| _tcscmp(findData.name, _T("..")) == 0
			)
		{
			// 是否是子目录并且不为"."或".."
			//cout << findData.name << "\t<dir>\n";
		}
		else
		{
			//cout << findData.name << "\t" << findData.size << endl;
			obj->push_back(findData.name);
		}

	} while (_tfindnext(handle, &findData) == 0);    // 查找目录中的下一个文件


	_findclose(handle);    // 关闭搜索句柄
	return TRUE;
}


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
bool FileOperation::CreateSubProcess(TCHAR * szBuffer, int flag, PROCESS_INFORMATION * ppi)
{

	//若要创建能继承的句柄，父进程必须指定一个SECURITY_ATTRIBUTES结构并对它进行初始化									
	//三个成员的意义：大小、默认安全属性、是否可以继承									
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	//定义创建进程需要用的结构体									
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);

	if (flag == 1)
	{
		//隐藏进程窗口
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}

	//创建子进程									
	BOOL res = CreateProcess(
		NULL,
		szBuffer,
		NULL,
		NULL,
		FALSE, 				//TRUE的时候，说明子进程可以继承父进程的句柄表				
		CREATE_NEW_CONSOLE,
		NULL,
		NULL, &si, &pi);
	if (ppi != 0)
	{
		*ppi = pi;
	}

	return res;
}



/*////////////////////////////////////////////////////////////////
*※※*  FullName:	以二进制的方式把文件读到内存里
*※※*  Returns:	成功返回指向这段内存数据的首地址，失败返回0
*※※*  Parameter:	path参数，是打开的文件路径
*※※*  Parameter:	Length输出参数，文件的大小
*※※*  Parameter:
*※※*  Parameter:
*※※*	Parameter:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
LPVOID FileOperation::FileReadMomery(TCHAR * path, int * Length)
{
	FILE* fp;
	DWORD filesize = 0;
	LPVOID pFileBuffer = NULL;
	errno_t err = 0;
	if ((err = _tfopen_s(&fp, path, _T("rb"))) != NULL)
	{
		MessageBoxA(NULL, "文件打开失败", "", MB_OK);
		return 0;
	}

	fseek(fp, 0, SEEK_END);	    //指针指向尾部
	filesize = ftell(fp);	    //记录文件的长度
	fseek(fp, 0, SEEK_SET);	    //恢复指针，指向开始位置

	pFileBuffer = m_alloc.auto_malloc<char*>(filesize);
	if (!pFileBuffer)
	{
		MessageBoxA(NULL, "内存申请失败!", "", MB_OK);
		fclose(fp);
		return NULL;
	}
	//将文件读取缓冲区
	fread(pFileBuffer, filesize, 1, fp);
	*Length = filesize;

	fclose(fp);

	return pFileBuffer;
}


/*////////////////////////////////////////////////////////////////
*※※*  FullName:	删除文件里的子文件夹和文件
*※※*  Returns:
*※※*  Parameter:	szDir参数，文件夹的绝对路径
*※※*  Parameter:	pfilename可选参数，过滤文件(不删除)，文件名的绝对路径
*※※*  Parameter:
*※※*  Parameter:
*※※*	Parameter:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
BOOL FileOperation::DeleteDirectory(LPCTSTR szDir, TCHAR * pfilename)
{
	WIN32_FIND_DATA FindFileData;
	CString strTempFileFind = szDir;
	strTempFileFind += _T("\\*.*");
	HANDLE hFind = FindFirstFile(strTempFileFind.GetString(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		return FALSE;
	}
	else
	{
		do {
			if (lstrcmp(FindFileData.cFileName, _T(".")) != 0 &&
				lstrcmp(FindFileData.cFileName,_T("..")) != 0)
			{
				CString strFindFile = szDir;
				strFindFile += _T("\\");
				strFindFile += FindFileData.cFileName;
				if (pfilename != NULL)
				{
					CString str = pfilename;
					if (_tcscmp(strFindFile.GetString(), str.GetString()) == 0)
					{
						continue;
					}
				}
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					SetFileAttributes(strFindFile.GetString(), FindFileData.dwFileAttributes&(~FILE_ATTRIBUTE_READONLY));
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (!DeleteDirectory(strFindFile.GetString()))
					{
						FindClose(hFind);
						return FALSE;
					}
				}
				else
				{
					if (!DeleteFile(strFindFile.GetString()))
					{
						FindClose(hFind);
						return FALSE;
					}
				}
			}
		} while (FindNextFile(hFind, &FindFileData));
	}
	FindClose(hFind);
	return RemoveDirectory(szDir);
}


/*////////////////////////////////////////////////////////////////
*※※*  FullName:	遍历文件下的所有文件(包括子文件夹下的文件)
*※※*  Returns:	成功返回1，失败返回0
*※※*  Parameter:	v_szOutFile输出参数，遍历到的所有的文件的路径保存到v_szOutFile
*※※*  Parameter:
*※※*  Parameter:
*※※*  Parameter:
*※※*	Parameter:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
BOOL FileOperation::EnumFile_Total(LPCTSTR szDir, vector<TCHAR*>* v_szOutFile)
{
	WIN32_FIND_DATA FindFileData;
	CString strTempFileFind = szDir;
	strTempFileFind += _T("\\*.*");
	HANDLE hFind = FindFirstFile(strTempFileFind.GetString(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		return FALSE;
	}
	else
	{
		do {
			if (lstrcmp(FindFileData.cFileName,_T(".")) != 0 &&
				lstrcmp(FindFileData.cFileName, _T("..")) != 0)
			{
				CString strFindFile = szDir;
				strFindFile += _T("\\");
				strFindFile += FindFileData.cFileName;

				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					SetFileAttributes(strFindFile.GetString(), FindFileData.dwFileAttributes&(~FILE_ATTRIBUTE_READONLY));
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					vector<TCHAR*>*mv_szOutFile = v_szOutFile;
					EnumFile_Total(strFindFile.GetString(), *&mv_szOutFile);

				}
				else
				{
					v_szOutFile->push_back(strFindFile.GetString());
				}
			}
		} while (FindNextFile(hFind, &FindFileData));
	}
	FindClose(hFind);
	return RemoveDirectory(szDir);
}

/*////////////////////////////////////////////////////////////////
*※※*  FullName:	遍历文件下的子文件夹，保存到v_szOutFile(包括子文件下的文件夹)
*※※*  Returns:	成功返回1，失败返回0
*※※*  Parameter:	v_szOutFile输出参数，把遍历到的所有子文件夹的路径保存到v_szOutFile
*※※*  Parameter:
*※※*  Parameter:
*※※*  Parameter:
*※※*	Parameter:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
BOOL FileOperation::EnumSubFolder(LPCTSTR szDir, vector<TCHAR*>* v_szOutFile)
{
	WIN32_FIND_DATA FindFileData;
	CString strTempFileFind = szDir;
	strTempFileFind += _T("\\*.*");
	HANDLE hFind = FindFirstFile(strTempFileFind.GetString(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		DWORD err = GetLastError();
		return FALSE;
	}
	else
	{
		do {
			if (lstrcmp(FindFileData.cFileName, _T(".")) != 0 &&
				lstrcmp(FindFileData.cFileName, _T("..")) != 0)
			{
				CString strFindFile = szDir;

				strFindFile += _T("\\");
				strFindFile += FindFileData.cFileName;

				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					SetFileAttributes(strFindFile.GetString(), FindFileData.dwFileAttributes&(~FILE_ATTRIBUTE_READONLY));
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					v_szOutFile->push_back(strFindFile.GetString());
					vector<TCHAR*>*mv_szOutFile = v_szOutFile;
					EnumSubFolder(strFindFile.GetString(), *&mv_szOutFile);

				}
				else
				{
					//v_szOutFile->push_back(strFindFile.GetString());
				}
			}
		} while (FindNextFile(hFind, &FindFileData));
	}
	FindClose(hFind);
	return RemoveDirectory(szDir);
}

BOOL FileOperation::GetProcessIdByName(TCHAR * szProcessName, DWORD & dwPid)
{
	HANDLE hSnapProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapProcess == NULL)
	{
		printf("[*] Create Process Snap Error!\n");
		return FALSE;
	}
	PROCESSENTRY32 pe32 = { 0 };
	RtlZeroMemory(&pe32, sizeof(pe32));
	pe32.dwSize = sizeof(pe32);
	BOOL bRet = Process32First(hSnapProcess, &pe32);
	while (bRet)
	{
		if (_tcsicmp(pe32.szExeFile, szProcessName) == 0)
		{
			dwPid = pe32.th32ProcessID;
			return TRUE;
		}
		bRet = Process32Next(hSnapProcess, &pe32);
	}
	return FALSE;
}

/*////////////////////////////////////////////////////////////////
*※※*  FullName:		GetAllThreadIdByProcessId
*※※*  功能	:		根据进程PID，获取所有线程Id
*※※*  Returns:		成功返回TRUE，失败返回FALSE
*※※*  Parameter_1:	ppThreadIdList，把获取的线程Id放到ppThreadIdList里面
*※※*  Parameter_2:
*※※*  Parameter_3:
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
BOOL FileOperation::GetAllThreadIdByProcessId(DWORD dwPid, vector<DWORD>* ppThreadIdList)
{
	DWORD dwThreadIdListLength = 0;
	DWORD dwThreadIdListMaxCount = 2000;

	THREADENTRY32 te32 = { 0 };
	RtlZeroMemory(&te32, sizeof(te32));
	te32.dwSize = sizeof(te32);
	HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnapshot == NULL)
	{
		printf("[*] Create Thread Snap Error!\n");
		return FALSE;
	}
	BOOL bRet = Thread32First(hThreadSnapshot, &te32);
	while (bRet)
	{
		if (te32.th32OwnerProcessID == dwPid)
		{
			if (dwThreadIdListLength >= dwThreadIdListMaxCount)
			{
				break;
			}
			++dwThreadIdListLength;
			ppThreadIdList->push_back(te32.th32ThreadID);
		}
		bRet = Thread32Next(hThreadSnapshot, &te32);
	}
	return TRUE;
}
/*////////////////////////////////////////////////////////////////
*※※*  FullName:		GetModuleInf
*※※*  功能	:		根据PID获取进程主模块的基址和大小
*※※*  Returns:		成功返回1，失败返回0
*※※*  Parameter_1:	PID，进程PID
*※※*  Parameter_2:	BaseAddr，输出参数，主模块的基址
*※※*  Parameter_3:	BaseSize，输出参数，主模块的大小
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
void FileOperation::GetModuleInf(DWORD PID, ULONG_PTR& BaseAddr, ULONG_PTR& BaseSize)
{
	//1 获取进程主模块的基址和大小
	HANDLE        hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
	//1. 创建一个模块相关的快照句柄
	hModuleSnap = CreateToolhelp32Snapshot(
		TH32CS_SNAPMODULE,  // 指定快照的类型
		PID);            // 指定进程
	if (hModuleSnap == INVALID_HANDLE_VALUE)
		return;
	//2. 通过模块快照句柄获取第一个模块信息
	if (!Module32First(hModuleSnap, &me32)) {
		CloseHandle(hModuleSnap);
		return;
	}
	//3. 循环获取模块信息
	do {
		/*printf("模块基址:%d,模块大小：%d,模块名称:%s\n",
			me32.modBaseAddr, me32.modBaseSize, me32.szModule);*/
		CString str = me32.szModule;
		
		if (_tcsicmp(str.FromCutRight(3).GetString(), _T("exe")) == 0)
		{
			BaseAddr = (ULONG_PTR)me32.modBaseAddr;
			BaseSize = me32.modBaseSize;
			break;
		}

	} while (Module32Next(hModuleSnap, &me32));
	//4. 关闭句柄并退出函数
	CloseHandle(hModuleSnap);
}



