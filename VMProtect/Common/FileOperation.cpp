
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
����* ���ܣ�		��Ŀ¼����ȡĿ¼��·��
����* @outPath[]:	����������ѵõ���Ŀ¼�ַ�����ֵ���������
*/
void FileOperation::GetDirPath_(TCHAR * outPath)
{
	BROWSEINFO bi;
	bi.hwndOwner = NULL;
	bi.pidlRoot = CSIDL_DESKTOP;//�ļ��еĸ�Ŀ¼���˴�Ϊ����
	bi.pszDisplayName = NULL;
	bi.lpszTitle = NULL;//��ʾλ�ڶԻ������ϲ�����ʾ��Ϣ
	bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;//���½��ļ��а�ť
	bi.lpfn = NULL;
	bi.iImage = 0;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);//����ѡ��Ի���
	if (pidl == NULL)
	{
		MessageBox(NULL, _T("û��ѡ��Ŀ¼"), _T(""), MB_OK);
		return;
	}
	TCHAR strFolder[MAX_PATH];
	SHGetPathFromIDList(pidl, strFolder);

	memcpy(outPath, strFolder, sizeof(strFolder));
}


/*
����* ���ܣ�		���ļ�����ȡ�ļ���·��
����* @outPath[]:	����������ѵõ���Ŀ¼�ַ�����ֵ���������
*/
void FileOperation::GetFilePath_(TCHAR * outPath)
{
	OPENFILENAME stOpenFile;
	//Ҫ���ļ�������
	TCHAR szPeFileExt[100] = _T("*.exe;*.dll;*.scr;*.drv;*.sys;*.*");
	TCHAR szFileName[MAX_PATH];

	//�ļ�������·���еĿ�ʼλ��
	DWORD StartFileName = 0;

	//��ʼ��
	memset(szFileName, 0, MAX_PATH);
	memset(&stOpenFile, 0, sizeof(OPENFILENAME));

	//����stOpenFile�ṹ�����������������
	stOpenFile.lStructSize = sizeof(OPENFILENAME);
	stOpenFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	stOpenFile.lpstrFilter = szPeFileExt;
	stOpenFile.lpstrFile = szFileName; //���
	stOpenFile.nMaxFile = MAX_PATH;

	//��ȡ�ļ�·��
	GetOpenFileName(&stOpenFile);
	memcpy(outPath, stOpenFile.lpstrFile, stOpenFile.nMaxFile);
}

/*
����* ����:		��ȡ�ڴ��еĶ����ƣ�����Ϊ�ļ�
����* @buffer[]:	�ڴ��ַ
����* @nLength[]:	��С
����* @OutPath[]:	���·��
*/
bool FileOperation::SaveFile_(char* buffer, POINTER_TYPE nLength, TCHAR * OutPath)
{

	FILE* fp;
	errno_t err = 0;
	err = _tfopen_s(&fp, OutPath, _T("wb"));
	if (err != 0)
	{
		//MessageBox(NULL, _T("�ļ���ʧ�ܣ�����"), _T("��ʾ"), MB_OK);
		return FALSE;
	}

	if (fwrite((char*)buffer, nLength, 1, fp))
	{
		//MessageBox(NULL, "����ɹ�", "��ʾ", MB_OK);
	}

	else
	{
		//MessageBox(NULL, _T("����ʧ�ܣ�����"), _T("��ʾ"), MB_OK);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}



/*////////////////////////////////////////////////////////////////
*����*  FullName:	ö��Ŀ¼�µ��ļ���,�ŵ�vector������(���������ļ�����ļ�)
*����*  Returns:	�ɹ�����1��ʧ�ܷ���0
*����*  Parameter:	dir������Ŀ¼·��
*����*  Parameter:	obj����������ѱ��������ļ�����ŵ�vector������
*����*  Parameter:
*����*  Parameter:
*����*	Parameter:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
bool FileOperation::EnumFiles(const TCHAR * dir, vector<TCHAR*>* obj)
{
	intptr_t handle;
	_tfinddata_t findData;
	CString str = dir;
	str += _T("\\*.*");
	
	handle = _tfindfirst(str.GetString(), &findData);    // ����Ŀ¼�еĵ�һ���ļ�
	if (handle == -1)
	{
		MessageBoxA(NULL, "Ŀ¼�µ��ļ�Ϊ��!!!", "����", MB_OK);
		return FALSE;
	}

	do
	{
		if (findData.attrib & _A_SUBDIR
			|| _tcscmp(findData.name, _T(".")) == 0
			|| _tcscmp(findData.name, _T("..")) == 0
			)
		{
			// �Ƿ�����Ŀ¼���Ҳ�Ϊ"."��".."
			//cout << findData.name << "\t<dir>\n";
		}
		else
		{
			//cout << findData.name << "\t" << findData.size << endl;
			obj->push_back(findData.name);
		}

	} while (_tfindnext(handle, &findData) == 0);    // ����Ŀ¼�е���һ���ļ�


	_findclose(handle);    // �ر��������
	return TRUE;
}


/*////////////////////////////////////////////////////////////////
*����*  FullName:	    �����ӽ���
*����*  Returns:	    �ɹ�����1��ʧ�ܷ���0
*����*  Parameter:	    szFuffer�������ļ�·��(�������������в���)
*����*  Parameter:	    flag��ѡ��������1Ϊ�����ӽ��̣�Ĭ�ϲ�����
*����*  Parameter:	    ppi��ѡ�������������ӽ��̻�����Ϣ
*����*  Parameter:
*����*	Parameter:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
bool FileOperation::CreateSubProcess(TCHAR * szBuffer, int flag, PROCESS_INFORMATION * ppi)
{

	//��Ҫ�����ܼ̳еľ���������̱���ָ��һ��SECURITY_ATTRIBUTES�ṹ���������г�ʼ��									
	//������Ա�����壺��С��Ĭ�ϰ�ȫ���ԡ��Ƿ���Լ̳�									
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	//���崴��������Ҫ�õĽṹ��									
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);

	if (flag == 1)
	{
		//���ؽ��̴���
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}

	//�����ӽ���									
	BOOL res = CreateProcess(
		NULL,
		szBuffer,
		NULL,
		NULL,
		FALSE, 				//TRUE��ʱ��˵���ӽ��̿��Լ̳и����̵ľ����				
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
*����*  FullName:	�Զ����Ƶķ�ʽ���ļ������ڴ���
*����*  Returns:	�ɹ�����ָ������ڴ����ݵ��׵�ַ��ʧ�ܷ���0
*����*  Parameter:	path�������Ǵ򿪵��ļ�·��
*����*  Parameter:	Length����������ļ��Ĵ�С
*����*  Parameter:
*����*  Parameter:
*����*	Parameter:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
LPVOID FileOperation::FileReadMomery(TCHAR * path, int * Length)
{
	FILE* fp;
	DWORD filesize = 0;
	LPVOID pFileBuffer = NULL;
	errno_t err = 0;
	if ((err = _tfopen_s(&fp, path, _T("rb"))) != NULL)
	{
		MessageBoxA(NULL, "�ļ���ʧ��", "", MB_OK);
		return 0;
	}

	fseek(fp, 0, SEEK_END);	    //ָ��ָ��β��
	filesize = ftell(fp);	    //��¼�ļ��ĳ���
	fseek(fp, 0, SEEK_SET);	    //�ָ�ָ�룬ָ��ʼλ��

	pFileBuffer = m_alloc.auto_malloc<char*>(filesize);
	if (!pFileBuffer)
	{
		MessageBoxA(NULL, "�ڴ�����ʧ��!", "", MB_OK);
		fclose(fp);
		return NULL;
	}
	//���ļ���ȡ������
	fread(pFileBuffer, filesize, 1, fp);
	*Length = filesize;

	fclose(fp);

	return pFileBuffer;
}


/*////////////////////////////////////////////////////////////////
*����*  FullName:	ɾ���ļ�������ļ��к��ļ�
*����*  Returns:
*����*  Parameter:	szDir�������ļ��еľ���·��
*����*  Parameter:	pfilename��ѡ�����������ļ�(��ɾ��)���ļ����ľ���·��
*����*  Parameter:
*����*  Parameter:
*����*	Parameter:
*����*	Author:		    LCH
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
*����*  FullName:	�����ļ��µ������ļ�(�������ļ����µ��ļ�)
*����*  Returns:	�ɹ�����1��ʧ�ܷ���0
*����*  Parameter:	v_szOutFile��������������������е��ļ���·�����浽v_szOutFile
*����*  Parameter:
*����*  Parameter:
*����*  Parameter:
*����*	Parameter:
*����*	Author:		    LCH
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
*����*  FullName:	�����ļ��µ����ļ��У����浽v_szOutFile(�������ļ��µ��ļ���)
*����*  Returns:	�ɹ�����1��ʧ�ܷ���0
*����*  Parameter:	v_szOutFile����������ѱ��������������ļ��е�·�����浽v_szOutFile
*����*  Parameter:
*����*  Parameter:
*����*  Parameter:
*����*	Parameter:
*����*	Author:		    LCH
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
*����*  FullName:		GetAllThreadIdByProcessId
*����*  ����	:		���ݽ���PID����ȡ�����߳�Id
*����*  Returns:		�ɹ�����TRUE��ʧ�ܷ���FALSE
*����*  Parameter_1:	ppThreadIdList���ѻ�ȡ���߳�Id�ŵ�ppThreadIdList����
*����*  Parameter_2:
*����*  Parameter_3:
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
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
*����*  FullName:		GetModuleInf
*����*  ����	:		����PID��ȡ������ģ��Ļ�ַ�ʹ�С
*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
*����*  Parameter_1:	PID������PID
*����*  Parameter_2:	BaseAddr�������������ģ��Ļ�ַ
*����*  Parameter_3:	BaseSize�������������ģ��Ĵ�С
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
void FileOperation::GetModuleInf(DWORD PID, ULONG_PTR& BaseAddr, ULONG_PTR& BaseSize)
{
	//1 ��ȡ������ģ��Ļ�ַ�ʹ�С
	HANDLE        hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
	//1. ����һ��ģ����صĿ��վ��
	hModuleSnap = CreateToolhelp32Snapshot(
		TH32CS_SNAPMODULE,  // ָ�����յ�����
		PID);            // ָ������
	if (hModuleSnap == INVALID_HANDLE_VALUE)
		return;
	//2. ͨ��ģ����վ����ȡ��һ��ģ����Ϣ
	if (!Module32First(hModuleSnap, &me32)) {
		CloseHandle(hModuleSnap);
		return;
	}
	//3. ѭ����ȡģ����Ϣ
	do {
		/*printf("ģ���ַ:%d,ģ���С��%d,ģ������:%s\n",
			me32.modBaseAddr, me32.modBaseSize, me32.szModule);*/
		CString str = me32.szModule;
		
		if (_tcsicmp(str.FromCutRight(3).GetString(), _T("exe")) == 0)
		{
			BaseAddr = (ULONG_PTR)me32.modBaseAddr;
			BaseSize = me32.modBaseSize;
			break;
		}

	} while (Module32Next(hModuleSnap, &me32));
	//4. �رվ�����˳�����
	CloseHandle(hModuleSnap);
}



