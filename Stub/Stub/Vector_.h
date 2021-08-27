#pragma once
#include <Windows.h>
#include "PreventDebug.h"
#define SUCCESS			1	//成功
#define INDEX_ERROR			-1	//失败
#define MALLOC_ERROR	-2	//申请内存失败
#define INDEX_ERROR		-3	//错误的索引号

using namespace std;
extern PreventDebug Preventdebug;
//在头文件中:
template <class T_ELE>
class vector_
{
public:
	vector_();
	vector_(DWORD dwSize);
	~vector_();
public:
	DWORD at(DWORD dwIndex, OUT T_ELE* pEle);	//根据给定的索引得到元素
	DWORD push_back(T_ELE Element);				//将元素储存到容器最后一个位置
	VOID pop_back();							//删除最后一个元素
	DWORD insert(DWORD dwIndex, T_ELE Element);	//向指定位置新增一个元素
	DWORD capacity();							//返回在不增容的情况下，还能储存多少元素
	VOID clear();								//清空所有元素
	BOOL empty();								//判断vector_是否为空 返回true时为空
	VOID erase(DWORD dwIndex);					//删除指定元素
	DWORD size();								//返回vector_元素数量的大小
	void outoforder(vector_<T_ELE>& arr);		//乱序排序(参数arr，既是输入参数，也是输出参数)

public:

private:
	BOOL expand();
private:
	DWORD m_dwIndex;	//下一个可用索引
	DWORD m_dwIncrement;//每次增容的大小
	DWORD m_dwLen;		//当前容器的长度
	DWORD m_dwInitSize;	//默认初始化大小
	T_ELE* m_pVector;	//容器指针

public:

//迭代器
	class iterator
	{
	public:
		iterator(T_ELE* p = nullptr) :_ptr(p) {}
		bool operator!=(const iterator& it)const
		{
			return _ptr != it._ptr;
		}
		void operator++()
		{
			++_ptr;
		}
		T_ELE* operator+(int index)
		{
			return _ptr + index;
		}
		T_ELE* operator-(int index)
		{
			return _ptr - index;
		}
		T_ELE& operator*()
		{
			return *_ptr;
		}
		const T_ELE& operator*()const
		{
			return *_ptr;
		}
	private:
		T_ELE* _ptr;
	};
	iterator begin()
	{
		return iterator(_first);
	}
	iterator end()
	{
		return iterator(_end);
	}

	T_ELE* _first;//指向数组起始位置
	T_ELE* _end;//指向数组空间的后继位置
};

template<class T_ELE>
inline vector_<T_ELE>::vector_() :m_dwInitSize(100), m_dwIncrement(5)
{
	m_pVector=(T_ELE*)Preventdebug.g_HeapAlloc(Preventdebug.g_fnGetProcessHeap(), 8, sizeof(T_ELE)*100);
	m_dwLen = m_dwInitSize;
	m_dwIndex = 0;
	_first = m_pVector;
}

template<class T_ELE>
inline vector_<T_ELE>::vector_(DWORD dwSize) :m_dwIncrement(5)
{
	m_pVector = (T_ELE*)Preventdebug.g_HeapAlloc(Preventdebug.g_fnGetProcessHeap(), 8, sizeof(T_ELE)*dwSize);
	m_dwLen = dwSize;
	m_dwIndex = 0;
}

template<class T_ELE>
inline vector_<T_ELE>::~vector_()
{
	//delete[] m_pVector;
	Preventdebug.g_HeapFree(Preventdebug.g_fnGetProcessHeap(), 1, m_pVector);
	m_pVector = NULL;
}

//根据给定的索引得到元素
template<class T_ELE>
inline DWORD vector_<T_ELE>::at(DWORD dwIndex, OUT T_ELE* pEle)
{
	if (!(dwIndex >= 0 && dwIndex < m_dwLen))
	{
		//cout << "索引超出范围" << endl;
		return INDEX_ERROR;
	}
	*pEle = m_pVector[dwIndex];
	return 0;
}

//将元素储存到容器最后一个位置
template<class T_ELE>
inline DWORD vector_<T_ELE>::push_back(T_ELE Element)
{
	//判断申请内存是否够用
	if (m_dwIndex >= 100)
		expand();

	m_pVector[m_dwIndex] = Element;
	_end = m_pVector + m_dwIndex;
	m_dwIndex++;
	return 0;
}

//删除最后一个元素
template<class T_ELE>
inline VOID vector_<T_ELE>::pop_back()
{
	m_pVector[m_dwIndex - 1] = 0;
	--m_dwIndex;
	_end = m_dwIndex;
	return VOID();
}

//向指定位置新增一个元素
template<class T_ELE>
inline DWORD vector_<T_ELE>::insert(DWORD dwIndex, T_ELE Element)
{
	//判断申请内存是否够用
	if (m_dwIndex >= 100)
		expand();

	//无符号转换为有符号
	if ((int)dwIndex < 0)
		dwIndex = 0;

	for (int i = 0; i < m_dwIndex - dwIndex + 1; i++)
	{
		m_pVector[m_dwIndex + 1 - i] = m_pVector[m_dwIndex - i];
	}
	m_pVector[dwIndex] = Element;
	m_dwIndex++;
	_end = m_dwIndex;
	return 0;
}

//返回在不增容的情况下，还能储存多少元素
template<class T_ELE>
inline DWORD vector_<T_ELE>::capacity()
{
	return m_dwLen - m_dwIndex;
}

//清空所有元素
template<class T_ELE>
inline VOID vector_<T_ELE>::clear()
{
	for (int i = 0; i < m_dwIndex + 1; i++)
		m_pVector[i] = NULL;
	m_dwIndex = 0;
	return VOID();
}

//判断vector_是否为空 返回true时为空
template<class T_ELE>
inline BOOL vector_<T_ELE>::empty()
{
	for (int i = 0; i < m_dwLen; i++)
	{
		if (m_pVector[i] != NULL)
			return false;
	}
	return true;
}

//删除指定元素
template<class T_ELE>
inline VOID vector_<T_ELE>::erase(DWORD dwIndex)
{
	--dwIndex;//删除第dwIndex个元素，那么下标应该减减
	for (int i = 0; i < m_dwIndex - dwIndex + 1; i++)
	{
		m_pVector[dwIndex + i] = m_pVector[dwIndex + i + 1];
	}
	--m_dwIndex;
	return VOID();
}

//返回vector_元素数量的大小
template<class T_ELE>
inline DWORD vector_<T_ELE>::size()
{
	return m_dwIndex;
}

//乱序排序(参数arr，既是输入参数，也是输出参数)
template<class T_ELE>
inline void vector_<T_ELE>::outoforder(vector_<T_ELE>& arr)
{
	vector_<T_ELE>::iterator ter_1 = arr.begin();
	vector_<T_ELE>temp_arr;
	int calc = 0;
	int length = arr.size();
	for (int i = 0; i < length; i++)
	{
		DWORD lls = Preventdebug.g_fnGetTickCount() % (length - calc);
		temp_arr.push_back(*(ter_1 + lls));
		arr.erase(lls + 1);
		++calc;
	}

	vector_<T_ELE>::iterator ter_2 = temp_arr.begin();
	for (int i = 0; i < length; i++)
	{
		//*(ter_1 + i) = *(ter_2 + i);
		arr.push_back(*(ter_2 + i));
	}
}



template<class T_ELE>
inline BOOL vector_<T_ELE>::expand()
{
	int nLength = (int)m_dwIncrement + (int)m_dwLen;
	//重新申请一块内存,大小为nLength
	T_ELE* pNewBuff = (T_ELE * )Preventdebug.g_HeapAlloc(Preventdebug.g_fnGetProcessHeap(), 8, sizeof(T_ELE)*nLength);

	//把原始数据拷贝到新内存
	Preventdebug.pmemcpy(pNewBuff, m_pVector, m_dwLen * sizeof(T_ELE));
	
	//释放原来的空间
	Preventdebug.g_HeapFree(Preventdebug.g_fnGetProcessHeap(), 1, m_pVector);

	//把新指针赋值给m_pVector
	m_pVector = pNewBuff;

	_first = m_pVector;

	//为各种属性赋值
	m_dwLen = nLength;

	return SUCCESS;
}