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
参数一 要转化为字符串的无符号整型
参数二 默认参数是0   如果填写1,那么输出8位数字,不足的前面补0

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
    //浅拷贝
    //m_pData = str.m_pData;

    //深拷贝
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
从中间截取字符串

@start[]：开始截取的位置
@nLength[]：截取的长度

返回值：截取字符串的首地址
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
*※※*  FullName:       MidCut_Intel
*※※*  功能	:       从中间截取字符串
*※※*  Returns:	    返回截取字符串的首地址
*※※*  Parameter_1:    str，      要操作的字符串
*※※*  Parameter_2:    start，    开始截取的位置
*※※*  Parameter_3:    nLength，  截取的长度
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
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
        // 从指定位置 查找下一个要替换的字符串的起始位置。
        std::string::size_type st = str.find(pSrc, stStart);
        if (st == str.npos)
        {
            break;
        }
        iter = iter + st - stStart;
        // 将目标字符串全部替换。
        str.replace(iter, iter + strlen(pSrc), pReplace);
        iter = iter + strlen(pReplace);
        // 替换的字符串下一个字符的位置
        stStart = st + strlen(pReplace);
    }
    return str;
}


//*********************************************************************************************
// FullName:		GB2312转UTF8编码
// Returns:		转换后的字符串指针
// Parameter:		const char * gb2312：gb2312字符串指针
// Author:		Lch
//*********************************************************************************************
TCHAR * CString::Gb2312_To_Utf8(const TCHAR * gb2312)
{
    int len = _tcslen(gb2312);
	wchar_t* wstr = new wchar_t[len + 1];
    if (NULL == wstr)
    {

	MessageBox(NULL, _T("Gb2312_To_Utf8内存申请失败!"), _T("提示"), MB_OK);
	return NULL;
    }
    memset(wstr, 0, (len + 1) * 2);

    MultiByteToWideChar(CP_ACP, 0, (LPCCH)gb2312, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    delete m_pData;

    m_pData= new TCHAR[len + 1];
    if (NULL == m_pData)
    {
	MessageBox(NULL, _T("Gb2312_To_Utf8内存申请失败!"), _T("提示"), MB_OK);
	return NULL;
    }
    memset(m_pData, 0, len + 1);

    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, (LPSTR)m_pData, len, NULL, NULL);

    if (wstr)
	delete[] wstr;

    return m_pData;
}


//*********************************************************************************************
// FullName:		SetClipboardData__，把数据复制到剪贴板
// Returns:		成功返回：1   失败返回：0
// Parameter:		TCHAR strText[]，要复制的字符串
// Author:		Lch
//*********************************************************************************************
bool CString::SetClipboardData__(TCHAR strText[])
{
    // 打开剪贴板   
    if (!OpenClipboard(NULL) || !EmptyClipboard())
    {
	MessageBox(NULL, _T("打开或清空剪切板出错！"), _T("提示"), MB_ICONINFORMATION);
	return 0;
    }

    HGLOBAL hMen;

    // 分配全局内存    
    hMen = GlobalAlloc(GMEM_MOVEABLE, (_tcslen(strText) + 1) * sizeof(TCHAR));

    if (!hMen)
    {
	MessageBox(NULL, _T("分配全局内存出错！"), _T("提示"), MB_ICONINFORMATION);
	// 关闭剪切板    
	CloseClipboard();
	return 0;
    }

    // 把数据拷贝考全局内存中   
    // 锁住内存区    
    LPSTR lpStr = (LPSTR)GlobalLock(hMen);

    // 内存复制   
    memcpy(lpStr, strText, ((_tcslen(strText)) * sizeof(TCHAR)));
    // 字符结束符    
    lpStr[_tcslen(strText)] = (TCHAR)0;
    // 释放锁    
    GlobalUnlock(hMen);

    // 把内存中的数据放到剪切板上   
    SetClipboardData(CF_TEXT, hMen);
    CloseClipboard();
    return 1;
}

/*////////////////////////////////////////////////////////////////
*※※*  FullName:       RightFindChar
*※※*  功能	:       从右边开始寻找字符
*※※*  Returns:	    成功返回该字符的位置，失败返回-1
*※※*  Parameter_1:    p,资源字符串
*※※*  Parameter_2:    cFind，目标字符
*※※*  Parameter_3:
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
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
*※※*  FullName:       LeftFindChar
*※※*  功能	:       从左边开始寻找字符
*※※*  Returns:	    成功返回该字符的位置，失败返回-1
*※※*  Parameter_1:    p,资源字符串
*※※*  Parameter_2:    cFind，目标字符
*※※*  Parameter_3:
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
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
*※※*  FullName:       LeftFindStr
*※※*  功能	:       从左边开始寻找字符串
*※※*  Returns:	    成功返回该字符串的位置，失败返回-1
*※※*  Parameter_1:    strRes,资源字符串
*※※*  Parameter_2:    strTar，目标字符串
*※※*  Parameter_3:
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
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
*※※*  FullName:       RightFindStr
*※※*  功能	:       从右边开始寻找字符串
*※※*  Returns:	    成功返回该字符串的位置，失败返回-1
*※※*  Parameter_1:    strRes,资源字符串
*※※*  Parameter_2:    strTar，目标字符串
*※※*  Parameter_3:
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
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
功能：在指定内存查找目标二进制

@strRes[]:要查找的内存
@strTar[]:目标二进制
@nLenRes[]:要查找的内存的长度
@nLenTar[]:目标二进制的长度

成功返回:匹配到的内存地址
失败返回:0

目标二进制的书写格式： char strTar[]={0xE8,0x80,0x65，0x67,0x89};

注意:可以支持模糊匹配，通配符是'*'，比如0xE8后面的地址是变化的，
	则目标二进制可以这样写char strTar[]={0xE8,'*','*','*','*',0x80,0x65}。
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
*※※*  FullName:		AccumulateMultiply
*※※*  功能	:		累乘
*※※*  Returns:		成功返回0x10的index次方
*※※*  Parameter_1:	index
*※※*  Parameter_2:
*※※*  Parameter_3:
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
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
*※※*  FullName:	Str_To_Num
*※※*	功能	:	把字符串的数字格式转换为整型的数字
*※※*  Returns:	成功返转换后的整形数字，失败返回0
*※※*  Parameter:	char hex_[]，输入格式是十六进制的字符串，比如可以这样写"0xff0055"或者"ff0055"
*※※*  Parameter:
*※※*  Parameter:
*※※*  Parameter:
*※※*	Parameter:
*※※*	Author:		    LCH
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

	//MessageBox(NULL, "输入有误，请重新输入", "提示",0);
	return 0;

    }
    //printf("%X\n", res);
    return res;
}
