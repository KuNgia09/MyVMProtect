#pragma once
#include<string>
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#ifdef _WIN64
#define POINTER_TYPE ULONGLONG
#else
#define POINTER_TYPE DWORD
#endif


class CString
{
    TCHAR* m_pData;
public:
    CString();					    //�޲εĹ��캯��
    ~CString();					    //��������

public:
    CString(const TCHAR* p);			    //�вι��캯��(�ַ���)
    CString(int numb);				    //�вι��캯��(�з�������)
    CString(DWORD numb);			    //�вι��캯��(�޷�������)
    CString(DWORD numb, int outformat);	    //�вι��캯��(�޷�������)
    CString(const CString& obj);		    //�������캯��

    				    
//���������(��Ϊ�������غ��������أ���������Ҫ�õ�frind��Ԫ)
public:

    //���� = �����
    CString operator=(const CString& obj);
    CString operator=(const TCHAR* str);
    CString operator=(const int numb);					    //�з���
    CString operator=(DWORD  numb);				    //�޷���

    //���� + �����
    friend CString operator+(const CString &obj_1, const CString &obj_2);
    friend CString operator+(const CString & obj,int numb);		    //�з���
    friend CString operator+(const CString & obj, DWORD numb);	    //�޷���

    //���� += �����
    CString& operator+=(const CString& obj);

	//���� [] �����
	TCHAR& operator[](int x)
	{
		return *(m_pData+x);
	}

//��ĸ��Сдת��
public:

    //������ĸСд
    CString& MakeLower()
    {
		_tcslwr_s(m_pData, _tcslen(m_pData)+1);
	return *this;
    }
    //������ĸ��д
    CString& MakeUpper()
    {
		_tcsupr_s(m_pData, _tcslen(m_pData)+1);
	return *this;
    }

//��������
public:

    //ȡm_pData�ַ����ĳ���
    int GetCount()
    {
		return _tcslen(m_pData);
    }

    //��ȡ�ַ���������char*����
	TCHAR* GetString()
    {
		return  m_pData ;
    }


//�ַ����Ĳ���(��ȡ ���� �滻 ����ȴ�)
public:
   
	/*
	���ܣ����м��ȡ�ַ���
	@start[]����ʼ��ȡ��λ��
	@nLength[]����ȡ�ĳ���

	����ֵ����ȡ�ַ������׵�ַ
	*/    
    CString MidCut(int start, int nLength);


    /*////////////////////////////////////////////////////////////////
    *����*  FullName:       MidCut_Intel  
    *����*  ����	:       ���м��ȡ�ַ���
    *����*  Returns:	    ���ؽ�ȡ�ַ������׵�ַ
    *����*  Parameter_1:    str��Ҫ�������ַ���
    *����*  Parameter_2:    start��  ��ʼ��ȡ��λ��
    *����*  Parameter_3:    nLength��  ��ȡ�ĳ���
    *����*  Parameter_4:
    *����*	Parameter_5:
    *����*	Author:		    LCH
    */////////////////////////////////////////////////////////////////;
    TCHAR* MidCut_Intel(TCHAR* str, int start, int nLength);

//********
/*
����* ���ܣ�		���ұ߽�ȡ�ַ���
����* @nLength[]��	Ҫ��ȡ�ĳ���
����* ����ֵ��		��ȡ�ַ������׵�ַ
*/
//********
    CString FromCutRight(int nLength)
    {
	    return MidCut(GetCount() - nLength, nLength);
    }

    //

/*////////////////////////////////////////////////////////////////
*����*  FullName:replace --- �滻�ַ���
*����*  Returns:	�ɹ�����string����
*����*  Parameter_1:inStr,�ַ���
*����*  Parameter_2:pSrc��Ҫ�滻���ַ�
*����*  Parameter_3:pReplace���滻��ʲô
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
    std::string replace(const std::string& inStr, const char* pSrc, const char* pReplace);
  

//*********
// FullName:		GB2312תUTF8����
// Returns:		    ת������ַ���ָ��
// Parameter:		const char * gb2312��gb2312�ַ���ָ��
// Author:		    Lch
//**********
    TCHAR* Gb2312_To_Utf8(const TCHAR* gb2312);

 
//**********
// FullName:		SetClipboardData__ - �����ݸ��Ƶ�������
// Returns:		    �ɹ����أ�1   ʧ�ܷ��أ�0
// Parameter:		TCHAR strText[]��Ҫ���Ƶ��ַ���
// Author:		    Lch
//**********
    bool SetClipboardData__(TCHAR strText[]);


/*////////////////////////////////////////////////////////////////
*����*  FullName:       RightFindChar
*����*  ����	:       ���ұ߿�ʼѰ���ַ�
*����*  Returns:	    �ɹ����ظ��ַ���λ�ã�ʧ�ܷ���-1
*����*  Parameter_1:    p,��Դ�ַ��� 
*����*  Parameter_2:    cFind��Ŀ���ַ�
*����*  Parameter_3:
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
    int RightFindChar(TCHAR* p, TCHAR cFind);


    /*////////////////////////////////////////////////////////////////
    *����*  FullName:       LeftFindChar
    *����*  ����	:       ����߿�ʼѰ���ַ�
    *����*  Returns:	    �ɹ����ظ��ַ���λ�ã�ʧ�ܷ���-1
    *����*  Parameter_1:    p,��Դ�ַ���
    *����*  Parameter_2:    cFind��Ŀ���ַ�
    *����*  Parameter_3:
    *����*  Parameter_4:
    *����*	Parameter_5:
    *����*	Author:		    LCH
    */////////////////////////////////////////////////////////////////;
    int LeftFindChar(TCHAR* p, TCHAR cFind);


    /*////////////////////////////////////////////////////////////////
    *����*  FullName:       LeftFindStr
    *����*  ����	:       ����߿�ʼѰ���ַ���
    *����*  Returns:	    �ɹ����ظ��ַ�����λ�ã�ʧ�ܷ���-1
    *����*  Parameter_1:    strRes,��Դ�ַ���
    *����*  Parameter_2:    strTar��Ŀ���ַ���
    *����*  Parameter_3:
    *����*  Parameter_4:
    *����*	Parameter_5:
    *����*	Author:		    LCH
    */////////////////////////////////////////////////////////////////;
    int LeftFindStr(TCHAR* strRes, TCHAR* strTar);


    /*////////////////////////////////////////////////////////////////
    *����*  FullName:       RightFindStr
    *����*  ����	:       ���ұ߿�ʼѰ���ַ���
    *����*  Returns:	    �ɹ����ظ��ַ�����λ�ã�ʧ�ܷ���-1
    *����*  Parameter_1:    strRes,��Դ�ַ���
    *����*  Parameter_2:    strTar��Ŀ���ַ���
    *����*  Parameter_3:
    *����*  Parameter_4:
    *����*	Parameter_5:
    *����*	Author:		    LCH
    */////////////////////////////////////////////////////////////////;
    int RightFindStr(TCHAR* strRes, TCHAR* strTar);



//�����Ʋ���
public:

/*////////////////////////////////////////////////////////////////
*����*	FullName:	Find_Memory_Bytes
*����*	���ܣ�		��ָ���ڴ����Ŀ�������
*����*	@strRes[]:	Ҫ���ҵ��ڴ�
*����*	@strTar[]:	Ŀ�������
*����*	@nLenRes[]:	Ҫ���ҵ��ڴ�ĳ���
*����*	@nLenTar[]:	Ŀ������Ƶĳ���
*����*	
*����*	�ɹ�����:ƥ�䵽���ڴ��ַ,ʧ�ܷ���:0
*����*	Ŀ������Ƶ���д��ʽ�� char strTar[]={0xE8,0x80,0x65��0x67,0x89};
*����*	
*����*	ע��:����֧��ģ��ƥ�䣬ͨ�����'*'������0xE8����ĵ�ַ�Ǳ仯�ģ�
			��Ŀ������ƿ�������дchar strTar[]={0xE8,'*','*','*','*',0x80,0x65}��
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////
	POINTER_TYPE Find_Memory_Bytes(char*strRes, char*strTar, int nLenRes, int nLenTar);

public:

/*////////////////////////////////////////////////////////////////
*����*  FullName:		AccumulateMultiply
*����*  ����	:		�۳�
*����*  Returns:		�ɹ�����0x10��index�η�
*����*  Parameter_1:	index
*����*  Parameter_2:
*����*  Parameter_3:
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
	POINTER_TYPE AccumulateMultiply(int index);


/*////////////////////////////////////////////////////////////////
*����*  FullName:	Str_To_Num
*����*	����	:	���ַ��������ָ�ʽת��Ϊ���͵�����
*����*  Returns:	�ɹ���ת������������֣�ʧ�ܷ���0
*����*  Parameter:	TCHAR hex_[]�������ʽ��ʮ�����Ƶ��ַ���,�����������д"0xff0055"����"ff0055"
*����*  Parameter:
*����*  Parameter:
*����*  Parameter:
*����*	Parameter:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
	POINTER_TYPE Str_To_Num(TCHAR hex_[]);


};

