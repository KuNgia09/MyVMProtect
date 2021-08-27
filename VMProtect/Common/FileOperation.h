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
	����* ���ܣ�		��Ŀ¼����ȡĿ¼��·��
	����* @outPath[]:	����������ѵõ���Ŀ¼�ַ�����ֵ���������
	*/
	void GetDirPath_(_Out_ TCHAR* outPath);


	/*
	����* ���ܣ�		���ļ�����ȡ�ļ���·��
	����* @outPath[]:	����������ѵõ���Ŀ¼�ַ�����ֵ���������
	*/
	void GetFilePath_(_Out_ TCHAR* outPath);


	/*
	����* ����:			��ȡ�ڴ��еĶ����ƣ�����Ϊ�ļ�
	����* @buffer[]:	�ڴ��ַ
	����* @nLength[]:	��С
	����* @OutPath[]:	�ļ������·��
	*/
	bool SaveFile_(char* buffer, POINTER_TYPE nLength, TCHAR* OutPath);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		ö��Ŀ¼�µ��ļ�,�ŵ�vector������(���������ļ�����ļ�)
	*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
	*����*  Parameter:		dir������Ŀ¼·��
	*����*  Parameter:		obj����������ѱ��������ļ���ŵ�vector������
	*����*  Parameter:
	*����*  Parameter:
	*����*	Parameter:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	bool EnumFiles(const TCHAR * dir,_Out_ vector<TCHAR*> *obj);

	/*
	��stringת��Ϊchar*
	���ӣ�
	vector<string>obj;
	std::string str = obj[i]; //obj[i]  i�������±� ��ʾ����vector������ĵڼ����ַ���
	char* p1 = str.c_str();
	����char* p2 = str.data();
 */


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
	bool CreateSubProcess(TCHAR* szBuffer, int flag = 0, PROCESS_INFORMATION * ppi = 0);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		���ļ��Զ����Ƶķ�ʽ�����ڴ���
	*����*  Returns:		�ɹ�����ָ������ڴ����ݵ��׵�ַ��ʧ�ܷ���0
	*����*  Parameter:		path�������Ǵ򿪵��ļ�·��
	*����*  Parameter:		Length����������ļ��Ĵ�С
	*����*  Parameter:
	*����*  Parameter:
	*����*	Parameter:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	LPVOID FileReadMomery(TCHAR* path, int* Length);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		ɾ���ļ�������ļ��к��ļ�
	*����*  Returns:
	*����*  Parameter:		szDir�������ļ��еľ���·��
	*����*  Parameter:		pfilename��ѡ�����������ļ�(��ɾ��)���ļ����ľ���·��
	*����*  Parameter:
	*����*  Parameter:
	*����*	Parameter:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL DeleteDirectory(LPCTSTR szDir, TCHAR* pfilename = 0);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		EnumFile_Total - �����ļ��µ������ļ�(�������ļ����µ��ļ�)
	*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
	*����*  Parameter:		v_szOutFile��������������������е��ļ���·��������v_szOutFile
	*����*  Parameter:
	*����*  Parameter:
	*����*  Parameter:
	*����*	Parameter:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL EnumFile_Total(LPCTSTR szDir, _Out_ vector<TCHAR*>*v_szOutFile);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		EnumSubFolder - �����ļ��µ����ļ��У����浽v_szOutFile(�������ļ��µ��ļ���)
	*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
	*����*  Parameter:		v_szOutFile����������ѱ��������������ļ��е�·�����浽v_szOutFile
	*����*  Parameter:
	*����*  Parameter:
	*����*  Parameter:
	*����*	Parameter:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL EnumSubFolder(LPCTSTR szDir, _Out_ vector<TCHAR*>*v_szOutFile);


	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		GetProcessIdByName - ���ݽ������ƻ�ȡ����PID
	*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
	*����*  Parameter_1:	szProcessName,���̵�����
	*����*  Parameter_2:	dwPid��������������̵�PID
	*����*  Parameter_3:
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL GetProcessIdByName(TCHAR *szProcessName, _Out_ DWORD& dwPid);

	/*////////////////////////////////////////////////////////////////
	*����*  FullName:		GetAllThreadIdByProcessId - ���ݽ���PID����ȡ�����߳�Id	
	*����*  Returns:		�ɹ�����TRUE��ʧ�ܷ���FALSE
	*����*  Parameter_1:	ppThreadIdList������������ѻ�ȡ���߳�Id�ŵ�ppThreadIdList����
	*����*  Parameter_2:
	*����*  Parameter_3:
	*����*  Parameter_4:
	*����*	Parameter_5:
	*����*	Author:		    LCH
	*/////////////////////////////////////////////////////////////////;
	BOOL GetAllThreadIdByProcessId(DWORD dwPid, _Out_ vector<DWORD>*ppThreadIdList);


/*////////////////////////////////////////////////////////////////
*����*  FullName:		GetModuleInf - ����PID��ȡ������ģ��Ļ�ַ�ʹ�С		
*����*  Returns:		�ɹ�����1��ʧ�ܷ���0
*����*  Parameter_1:	PID������PID
*����*  Parameter_2:	BaseAddr�������������ģ��Ļ�ַ
*����*  Parameter_3:	BaseSize�������������ģ��Ĵ�С
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
	void GetModuleInf(DWORD PID, _Out_ ULONG_PTR& BaseAddr, _Out_ ULONG_PTR& BaseSize);
};
