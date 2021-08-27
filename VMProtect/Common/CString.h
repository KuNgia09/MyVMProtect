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
    CString();					    //无参的构造函数
    ~CString();					    //析构函数

public:
    CString(const TCHAR* p);			    //有参构造函数(字符串)
    CString(int numb);				    //有参构造函数(有符号整型)
    CString(DWORD numb);			    //有参构造函数(无符号整型)
    CString(DWORD numb, int outformat);	    //有参构造函数(无符号整型)
    CString(const CString& obj);		    //拷贝构造函数

    				    
//重载运算符(分为内类重载和内外重载，内外重载要用到frind友元)
public:

    //重载 = 运算符
    CString operator=(const CString& obj);
    CString operator=(const TCHAR* str);
    CString operator=(const int numb);					    //有符号
    CString operator=(DWORD  numb);				    //无符号

    //重载 + 运算符
    friend CString operator+(const CString &obj_1, const CString &obj_2);
    friend CString operator+(const CString & obj,int numb);		    //有符号
    friend CString operator+(const CString & obj, DWORD numb);	    //无符号

    //重载 += 运算符
    CString& operator+=(const CString& obj);

	//重载 [] 运算符
	TCHAR& operator[](int x)
	{
		return *(m_pData+x);
	}

//字母大小写转换
public:

    //所有字母小写
    CString& MakeLower()
    {
		_tcslwr_s(m_pData, _tcslen(m_pData)+1);
	return *this;
    }
    //所有字母大写
    CString& MakeUpper()
    {
		_tcsupr_s(m_pData, _tcslen(m_pData)+1);
	return *this;
    }

//内联函数
public:

    //取m_pData字符串的长度
    int GetCount()
    {
		return _tcslen(m_pData);
    }

    //获取字符串，返回char*类型
	TCHAR* GetString()
    {
		return  m_pData ;
    }


//字符串的操作(截取 查找 替换 倒序等待)
public:
   
	/*
	功能：从中间截取字符串
	@start[]：开始截取的位置
	@nLength[]：截取的长度

	返回值：截取字符串的首地址
	*/    
    CString MidCut(int start, int nLength);


    /*////////////////////////////////////////////////////////////////
    *※※*  FullName:       MidCut_Intel  
    *※※*  功能	:       从中间截取字符串
    *※※*  Returns:	    返回截取字符串的首地址
    *※※*  Parameter_1:    str，要操作的字符串
    *※※*  Parameter_2:    start，  开始截取的位置
    *※※*  Parameter_3:    nLength，  截取的长度
    *※※*  Parameter_4:
    *※※*	Parameter_5:
    *※※*	Author:		    LCH
    */////////////////////////////////////////////////////////////////;
    TCHAR* MidCut_Intel(TCHAR* str, int start, int nLength);

//********
/*
※※* 功能：		从右边截取字符串
※※* @nLength[]：	要截取的长度
※※* 返回值：		截取字符串的首地址
*/
//********
    CString FromCutRight(int nLength)
    {
	    return MidCut(GetCount() - nLength, nLength);
    }

    //

/*////////////////////////////////////////////////////////////////
*※※*  FullName:replace --- 替换字符串
*※※*  Returns:	成功返回string类型
*※※*  Parameter_1:inStr,字符串
*※※*  Parameter_2:pSrc，要替换的字符
*※※*  Parameter_3:pReplace，替换成什么
*※※*  Parameter_4:
*※※*	Parameter_5:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
    std::string replace(const std::string& inStr, const char* pSrc, const char* pReplace);
  

//*********
// FullName:		GB2312转UTF8编码
// Returns:		    转换后的字符串指针
// Parameter:		const char * gb2312：gb2312字符串指针
// Author:		    Lch
//**********
    TCHAR* Gb2312_To_Utf8(const TCHAR* gb2312);

 
//**********
// FullName:		SetClipboardData__ - 把数据复制到剪贴板
// Returns:		    成功返回：1   失败返回：0
// Parameter:		TCHAR strText[]，要复制的字符串
// Author:		    Lch
//**********
    bool SetClipboardData__(TCHAR strText[]);


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
    int RightFindChar(TCHAR* p, TCHAR cFind);


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
    int LeftFindChar(TCHAR* p, TCHAR cFind);


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
    int LeftFindStr(TCHAR* strRes, TCHAR* strTar);


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
    int RightFindStr(TCHAR* strRes, TCHAR* strTar);



//二进制操作
public:

/*////////////////////////////////////////////////////////////////
*※※*	FullName:	Find_Memory_Bytes
*※※*	功能：		在指定内存查找目标二进制
*※※*	@strRes[]:	要查找的内存
*※※*	@strTar[]:	目标二进制
*※※*	@nLenRes[]:	要查找的内存的长度
*※※*	@nLenTar[]:	目标二进制的长度
*※※*	
*※※*	成功返回:匹配到的内存地址,失败返回:0
*※※*	目标二进制的书写格式： char strTar[]={0xE8,0x80,0x65，0x67,0x89};
*※※*	
*※※*	注意:可以支持模糊匹配，通配符是'*'，比如0xE8后面的地址是变化的，
			则目标二进制可以这样写char strTar[]={0xE8,'*','*','*','*',0x80,0x65}。
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////
	POINTER_TYPE Find_Memory_Bytes(char*strRes, char*strTar, int nLenRes, int nLenTar);

public:

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
	POINTER_TYPE AccumulateMultiply(int index);


/*////////////////////////////////////////////////////////////////
*※※*  FullName:	Str_To_Num
*※※*	功能	:	把字符串的数字格式转换为整型的数字
*※※*  Returns:	成功返转换后的整形数字，失败返回0
*※※*  Parameter:	TCHAR hex_[]，输入格式是十六进制的字符串,比如可以这样写"0xff0055"或者"ff0055"
*※※*  Parameter:
*※※*  Parameter:
*※※*  Parameter:
*※※*	Parameter:
*※※*	Author:		    LCH
*/////////////////////////////////////////////////////////////////;
	POINTER_TYPE Str_To_Num(TCHAR hex_[]);


};

