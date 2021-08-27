#include "CString.h"


CString::CString()
{
    m_pData = new TCHAR(0);
}

CString::~CString()
{
    delete[]m_pData;
}

CString::CString(const TCHAR * str)
{
    int nLength = _tcslen(str);

    m_pData = new TCHAR[nLength + 1];
    _tcscpy_s(m_pData, nLength+1, str);

}

CString::CString(int numb)
{
	TCHAR str[20];
    memset(str, 0, sizeof(str));

    if (numb < 0)
    {
	numb = (numb ^ 0xFFFFFFFF) + 1;
	_tcscpy_s(str, sizeof(_T("-")), _T("-"));
    }

	TCHAR str1[20];
    memset(str1, 0, sizeof(str));
    _stprintf_s(str1, _T("%d"), numb);
    _tcscat_s(str, _tcslen(str1) + _tcslen(str) + 1, str1);

    int nLen = _tcslen(str);
    m_pData = new TCHAR[nLen + 1];
    _tcscpy_s(m_pData, nLen + 1, str);
}

CString::CString(DWORD numb)
{
	TCHAR str[20];

    _stprintf_s(str, _T("0x%X"), numb);

    int nLen = _tcslen(str);
    m_pData = new TCHAR[nLen + 1];
    _tcscpy_s(m_pData, nLen + 1, str);
}




/*
����һ Ҫת��Ϊ�ַ������޷�������
������ Ĭ�ϲ�����0   �����д1,��ô���8λ����,�����ǰ�油0

*/
CString::CString(DWORD numb,int outformat=0)
{
	TCHAR str[20];
    if (outformat==0)
    {
	_stprintf_s(str, _T("0x%X"), numb);
    }
    else
    {
	_stprintf_s(str, _T("0x%08X"), numb);
    }

    int nLen = _tcslen(str);
    m_pData = new TCHAR[nLen + 1];
    _tcscpy_s(m_pData, nLen + 1, str);
}


CString::CString(const CString& obj)
{
    //ǳ����
    //m_pData = str.m_pData;

    //���
    int nLen = ((CString&)obj).GetCount();
    m_pData = new TCHAR[nLen + 1];
    _tcscpy_s(m_pData, nLen+1, obj.m_pData);
}


CString CString::operator=(const CString& obj)
{
    int nLen = ((CString&)obj).GetCount();
    if (GetCount()<nLen)
    {
	delete m_pData;
	m_pData = new TCHAR[nLen + 1];
    }
    _tcscpy_s(m_pData, nLen+1, obj.m_pData);

    return *this;
}

CString CString::operator=(const TCHAR * str)
{
    int nLen = _tcslen(str);
    if (GetCount()<nLen)
    {
	delete m_pData;
	m_pData = new TCHAR[nLen + 1];
    }
    _tcscpy_s(m_pData, nLen + 1, str);
    return *this;
}

CString CString::operator=(int numb)
{
	TCHAR str[20];
    memset(str, 0, sizeof(str));

    if (numb < 0)
    {
	numb = (numb ^ 0xFFFFFFFF) + 1;
	_tcscpy_s(str, sizeof(_T("-")), _T("-"));
    }

	TCHAR str1[20];
    memset(str1, 0, sizeof(str));
    _stprintf_s(str1, _T("%d"), numb);
    _tcscat_s(str, _tcslen(str1) + _tcslen(str) + 1, str1);

    int nLen = _tcslen(str);
    if (GetCount() < nLen)
    {
	delete m_pData;
	m_pData = new TCHAR[nLen + 1];
    }
    _tcscpy_s(m_pData, nLen + 1, str);

    return *this;
}

CString CString::operator=(DWORD numb)
{
	TCHAR str[20];
    memset(str, 0, sizeof(str));
    _stprintf_s(str, _T("0x%X"), numb);

    int nLen = _tcslen(str);
    if (GetCount() < nLen)
    {
	delete m_pData;
	m_pData = new TCHAR[nLen + 1];
    }
    _tcscpy_s(m_pData, nLen + 1, str);
    return *this;
}

CString & CString::operator+=(const CString & obj)
{
    int nLen = GetCount() + ((CString &) obj).GetCount();
	TCHAR* temp = new TCHAR[nLen + 1];
    memset(temp, 0, nLen + 1);
    _tcscpy_s(temp, GetCount() + 1, m_pData);
    _tcscat_s(temp, nLen + 1, obj.m_pData);

    delete m_pData;
    m_pData = NULL;

    m_pData = new TCHAR[nLen + 1];
    _tcscpy_s(m_pData, nLen + 1, temp);

    delete temp;
    temp = NULL;
    return *this;

}

CString operator+(const CString& obj_1,int numb)
{
	TCHAR str[20];
    memset(str, 0, sizeof(str));
    if (numb<0)
    {
	numb = (numb ^ 0xFFFFFFFF) + 1;
	_tcscpy_s(str, sizeof(_T("-")), _T("-"));
    }

	TCHAR str1[20];
    memset(str1, 0, sizeof(str));
    _stprintf_s(str1, _T("%d"), numb);
    _tcscat_s(str, _tcslen(str1) + _tcslen(str)+1, str1);
   
    CString obj;

    int nLen = ((CString &)obj_1).GetCount() + _tcslen(str);
    obj.m_pData = new TCHAR[nLen + 1];
    _tcscpy_s(obj.m_pData, ((CString &)obj_1).GetCount() + 1, obj_1.m_pData);
    _tcscat_s(obj.m_pData, nLen + 1, str);

    return obj;
}

CString operator+(const CString & obj_1, DWORD numb)
{
	TCHAR str[20];
    memset(str, 0, sizeof(str));
    _stprintf_s(str, _T("0x%X"), numb);

    CString obj;

    int nLen = ((CString &)obj_1).GetCount() + _tcslen(str);
    obj.m_pData = new TCHAR[nLen + 1];
    _tcscpy_s(obj.m_pData, ((CString &)obj_1).GetCount() + 1, obj_1.m_pData);
    _tcscat_s(obj.m_pData, nLen + 1, str);

    return obj;
}


CString operator+(const CString & obj_1, const CString & obj_2)
{
    CString obj; 

    int nLen = ((CString &) obj_1).GetCount() + 
	((CString &)obj_2).GetCount();
    obj.m_pData = new TCHAR[nLen + 1];
    memset(obj.m_pData, 0, nLen + 1);
    _tcscpy_s(obj.m_pData, ((CString &)obj_1).GetCount()+1, obj_1.m_pData);
    _tcscat_s(obj.m_pData, nLen+1, obj_2.m_pData);

    return obj;
}


/*
���м��ȡ�ַ���

@start[]����ʼ��ȡ��λ��
@nLength[]����ȡ�ĳ���

����ֵ����ȡ�ַ������׵�ַ
*/
CString CString::MidCut(int start, int nLength)
{
    if (start > GetCount())
    {
        return CString();
    }

    if (nLength > GetCount())
    {
        nLength = GetCount() - start;
    }

    CString p;
    delete p.m_pData;
    p.m_pData = new TCHAR[nLength + 1];
#ifdef UNICODE
    wmemcpy(p.m_pData, m_pData + start, nLength);
#else
    memcpy(p.m_pData, m_pData + start, nLength);
#endif // !UNICODE
    p.m_pData[nLength] = '\0';
    return p;
}

/*////////////////////////////////////////////////////////////////
*����*  FullName:       MidCut_Intel
*����*  ����	:       ���м��ȡ�ַ���
*����*  Returns:	    ���ؽ�ȡ�ַ������׵�ַ
*����*  Parameter_1:    str��      Ҫ�������ַ���
*����*  Parameter_2:    start��    ��ʼ��ȡ��λ��
*����*  Parameter_3:    nLength��  ��ȡ�ĳ���
*����*  Parameter_4:
*����*	Parameter_5:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
TCHAR* CString::MidCut_Intel(TCHAR* str, int start, int nLength)
{

    TCHAR* temp = (TCHAR*)new TCHAR[nLength + 1];
    //char* temp = (char*)malloc(nLength + 1);
    memset(temp, 0, nLength + 1);
#ifdef UNICODE
    wmemcpy(temp, str, nLength);
#else
    memcpy(temp, str, nLength);
#endif // !UNICODE

    temp[nLength] = '\0';
    return temp;
}

std::string CString::replace(const std::string& inStr, const char* pSrc, const char* pReplace)
{
    std::string str = inStr;
    std::string::size_type stStart = 0;
    std::string::iterator iter = str.begin();
    while (iter != str.end())
    {
        // ��ָ��λ�� ������һ��Ҫ�滻���ַ�������ʼλ�á�
        std::string::size_type st = str.find(pSrc, stStart);
        if (st == str.npos)
        {
            break;
        }
        iter = iter + st - stStart;
        // ��Ŀ���ַ���ȫ���滻��
        str.replace(iter, iter + strlen(pSrc), pReplace);
        iter = iter + strlen(pReplace);
        // �滻���ַ�����һ���ַ���λ��
        stStart = st + strlen(pReplace);
    }
    return str;
}


//*********************************************************************************************
// FullName:		GB2312תUTF8����
// Returns:		ת������ַ���ָ��
// Parameter:		const char * gb2312��gb2312�ַ���ָ��
// Author:		Lch
//*********************************************************************************************
TCHAR * CString::Gb2312_To_Utf8(const TCHAR * gb2312)
{
    int len = _tcslen(gb2312);
	wchar_t* wstr = new wchar_t[len + 1];
    if (NULL == wstr)
    {

	MessageBox(NULL, _T("Gb2312_To_Utf8�ڴ�����ʧ��!"), _T("��ʾ"), MB_OK);
	return NULL;
    }
    memset(wstr, 0, (len + 1) * 2);

    MultiByteToWideChar(CP_ACP, 0, (LPCCH)gb2312, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    delete m_pData;

    m_pData= new TCHAR[len + 1];
    if (NULL == m_pData)
    {
	MessageBox(NULL, _T("Gb2312_To_Utf8�ڴ�����ʧ��!"), _T("��ʾ"), MB_OK);
	return NULL;
    }
    memset(m_pData, 0, len + 1);

    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, (LPSTR)m_pData, len, NULL, NULL);

    if (wstr)
	delete[] wstr;

    return m_pData;
}


//*********************************************************************************************
// FullName:		SetClipboardData__�������ݸ��Ƶ�������
// Returns:		�ɹ����أ�1   ʧ�ܷ��أ�0
// Parameter:		TCHAR strText[]��Ҫ���Ƶ��ַ���
// Author:		Lch
//*********************************************************************************************
bool CString::SetClipboardData__(TCHAR strText[])
{
    // �򿪼�����   
    if (!OpenClipboard(NULL) || !EmptyClipboard())
    {
	MessageBox(NULL, _T("�򿪻���ռ��а����"), _T("��ʾ"), MB_ICONINFORMATION);
	return 0;
    }

    HGLOBAL hMen;

    // ����ȫ���ڴ�    
    hMen = GlobalAlloc(GMEM_MOVEABLE, (_tcslen(strText) + 1) * sizeof(TCHAR));

    if (!hMen)
    {
	MessageBox(NULL, _T("����ȫ���ڴ����"), _T("��ʾ"), MB_ICONINFORMATION);
	// �رռ��а�    
	CloseClipboard();
	return 0;
    }

    // �����ݿ�����ȫ���ڴ���   
    // ��ס�ڴ���    
    LPSTR lpStr = (LPSTR)GlobalLock(hMen);

    // �ڴ渴��   
    memcpy(lpStr, strText, ((_tcslen(strText)) * sizeof(TCHAR)));
    // �ַ�������    
    lpStr[_tcslen(strText)] = (TCHAR)0;
    // �ͷ���    
    GlobalUnlock(hMen);

    // ���ڴ��е����ݷŵ����а���   
    SetClipboardData(CF_TEXT, hMen);
    CloseClipboard();
    return 1;
}

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
int CString::RightFindChar(TCHAR* p, TCHAR cFind)
{
    int nIndex = _tcslen(p);
    while (nIndex--)
    {
        if (p[nIndex] == cFind)
        {
            return nIndex;
        }
    }
    return -1;
}

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
int CString::LeftFindChar(TCHAR* p, TCHAR cFind)
{
    int nIndex = _tcslen(p);
    int i = 0;
    while (nIndex--)
    {
        if (p[i++] == cFind)
        {
            return i;
        }
    }
    return -1;
}

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
int CString::LeftFindStr(TCHAR* strRes, TCHAR* strTar)
{
    int calc = 0;
    int nLenRes = _tcslen(strRes);
    int nLenTar = _tcslen(strTar);
    for (int i = 0; i < nLenRes; ++i)
    {
        if (strRes[i] == strTar[calc])
        {
            ++calc;
        }
        else
        {
            calc = 0;
        }

        if (calc == nLenTar)
        {
            return  i;
        }
    }
    return -1;
}

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
int CString::RightFindStr(TCHAR* strRes, TCHAR* strTar)
{
    int calc = 0;
    int nLenRes = _tcslen(strRes);
    int nLenTar = _tcslen(strTar);
    for (int i = 0; i < nLenRes; ++i)
    {
        //int k = strRes[nLenRes - i - 1];
        //int j = strTar[nLenTar - calc - 1];
        if (strRes[nLenRes - i - 1] == strTar[nLenTar - calc - 1])
        {
            ++calc;
        }
        else
        {
            if (calc != 0)
            {
                --i;
            }
            calc = 0;
        }

        if (calc == nLenTar)
        {
            return  nLenRes - i;
        }
    }
    return -1;
}


/*
���ܣ���ָ���ڴ����Ŀ�������

@strRes[]:Ҫ���ҵ��ڴ�
@strTar[]:Ŀ�������
@nLenRes[]:Ҫ���ҵ��ڴ�ĳ���
@nLenTar[]:Ŀ������Ƶĳ���

�ɹ�����:ƥ�䵽���ڴ��ַ
ʧ�ܷ���:0

Ŀ������Ƶ���д��ʽ�� char strTar[]={0xE8,0x80,0x65��0x67,0x89};

ע��:����֧��ģ��ƥ�䣬ͨ�����'*'������0xE8����ĵ�ַ�Ǳ仯�ģ�
	��Ŀ������ƿ�������дchar strTar[]={0xE8,'*','*','*','*',0x80,0x65}��
*/
POINTER_TYPE CString::Find_Memory_Bytes(char* strRes, char* strTar, int nLenRes, int nLenTar)
{
    int calc = 0;

    for (int i = 0; i < nLenRes; ++i)
    {
	if (strRes[i] == strTar[calc] || strTar[calc] == '*')
	{
	    ++calc;
	}
	else
	{
	    calc = 0;
	}

	if (calc == nLenTar)
	{
	    return  (POINTER_TYPE)strRes + i - nLenTar+1;
	}
    }
    return 0;
}


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
POINTER_TYPE CString::AccumulateMultiply(int index)
{
	POINTER_TYPE res = 1;
    for (int i = 0; i < index - 1; i++)
    {
	res *= 0x10;
    }
    return res;
}


/*////////////////////////////////////////////////////////////////
*����*  FullName:	Str_To_Num
*����*	����	:	���ַ��������ָ�ʽת��Ϊ���͵�����
*����*  Returns:	�ɹ���ת������������֣�ʧ�ܷ���0
*����*  Parameter:	char hex_[]�������ʽ��ʮ�����Ƶ��ַ����������������д"0xff0055"����"ff0055"
*����*  Parameter:
*����*  Parameter:
*����*  Parameter:
*����*	Parameter:
*����*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
POINTER_TYPE CString::Str_To_Num(TCHAR hex_[])
{
	TCHAR hex[50] = { 0 };
#ifdef UNICODE
    wmemcpy(hex, hex_, _tcslen(hex_));
#else
    memcpy(hex, hex_, _tcslen(hex_));
#endif // UNICODE



    if (_tcsnccmp(_T("0x"), hex, _tcslen(_T("0x"))) == 0)
    {
		_tcscpy_s(hex, &hex[2]);
    }
    //printf("%s\n", hex);

	POINTER_TYPE res = 0;
    for (int i = 0; i < _tcslen(hex); i++)
    {
	if (hex[i] >= 48 && hex[i] <= 57)
	{
	    res += (hex[i] - 48)*AccumulateMultiply(_tcslen(hex) - i);
	    continue;
	}
	if (hex[i] >= 65 && hex[i] <= 70)
	{
	    res += (hex[i] - 55)*AccumulateMultiply(_tcslen(hex) - i);
	    continue;
	}
	if (hex[i] >= 97 && hex[i] <= 102)
	{
	    res += (hex[i] - 87)*AccumulateMultiply(_tcslen(hex) - i);
	    continue;
	}

	//MessageBox(NULL, "������������������", "��ʾ",0);
	return 0;

    }
    //printf("%X\n", res);
    return res;
}
