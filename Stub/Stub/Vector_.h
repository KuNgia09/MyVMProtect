#pragma once
#include <Windows.h>
#include "PreventDebug.h"
#define SUCCESS			1	//�ɹ�
#define INDEX_ERROR			-1	//ʧ��
#define MALLOC_ERROR	-2	//�����ڴ�ʧ��
#define INDEX_ERROR		-3	//�����������

using namespace std;
extern PreventDebug Preventdebug;
//��ͷ�ļ���:
template <class T_ELE>
class vector_
{
public:
	vector_();
	vector_(DWORD dwSize);
	~vector_();
public:
	DWORD at(DWORD dwIndex, OUT T_ELE* pEle);	//���ݸ����������õ�Ԫ��
	DWORD push_back(T_ELE Element);				//��Ԫ�ش��浽�������һ��λ��
	VOID pop_back();							//ɾ�����һ��Ԫ��
	DWORD insert(DWORD dwIndex, T_ELE Element);	//��ָ��λ������һ��Ԫ��
	DWORD capacity();							//�����ڲ����ݵ�����£����ܴ������Ԫ��
	VOID clear();								//�������Ԫ��
	BOOL empty();								//�ж�vector_�Ƿ�Ϊ�� ����trueʱΪ��
	VOID erase(DWORD dwIndex);					//ɾ��ָ��Ԫ��
	DWORD size();								//����vector_Ԫ�������Ĵ�С
	void outoforder(vector_<T_ELE>& arr);		//��������(����arr���������������Ҳ���������)

public:

private:
	BOOL expand();
private:
	DWORD m_dwIndex;	//��һ����������
	DWORD m_dwIncrement;//ÿ�����ݵĴ�С
	DWORD m_dwLen;		//��ǰ�����ĳ���
	DWORD m_dwInitSize;	//Ĭ�ϳ�ʼ����С
	T_ELE* m_pVector;	//����ָ��

public:

//������
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

	T_ELE* _first;//ָ��������ʼλ��
	T_ELE* _end;//ָ������ռ�ĺ��λ��
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

//���ݸ����������õ�Ԫ��
template<class T_ELE>
inline DWORD vector_<T_ELE>::at(DWORD dwIndex, OUT T_ELE* pEle)
{
	if (!(dwIndex >= 0 && dwIndex < m_dwLen))
	{
		//cout << "����������Χ" << endl;
		return INDEX_ERROR;
	}
	*pEle = m_pVector[dwIndex];
	return 0;
}

//��Ԫ�ش��浽�������һ��λ��
template<class T_ELE>
inline DWORD vector_<T_ELE>::push_back(T_ELE Element)
{
	//�ж������ڴ��Ƿ���
	if (m_dwIndex >= 100)
		expand();

	m_pVector[m_dwIndex] = Element;
	_end = m_pVector + m_dwIndex;
	m_dwIndex++;
	return 0;
}

//ɾ�����һ��Ԫ��
template<class T_ELE>
inline VOID vector_<T_ELE>::pop_back()
{
	m_pVector[m_dwIndex - 1] = 0;
	--m_dwIndex;
	_end = m_dwIndex;
	return VOID();
}

//��ָ��λ������һ��Ԫ��
template<class T_ELE>
inline DWORD vector_<T_ELE>::insert(DWORD dwIndex, T_ELE Element)
{
	//�ж������ڴ��Ƿ���
	if (m_dwIndex >= 100)
		expand();

	//�޷���ת��Ϊ�з���
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

//�����ڲ����ݵ�����£����ܴ������Ԫ��
template<class T_ELE>
inline DWORD vector_<T_ELE>::capacity()
{
	return m_dwLen - m_dwIndex;
}

//�������Ԫ��
template<class T_ELE>
inline VOID vector_<T_ELE>::clear()
{
	for (int i = 0; i < m_dwIndex + 1; i++)
		m_pVector[i] = NULL;
	m_dwIndex = 0;
	return VOID();
}

//�ж�vector_�Ƿ�Ϊ�� ����trueʱΪ��
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

//ɾ��ָ��Ԫ��
template<class T_ELE>
inline VOID vector_<T_ELE>::erase(DWORD dwIndex)
{
	--dwIndex;//ɾ����dwIndex��Ԫ�أ���ô�±�Ӧ�ü���
	for (int i = 0; i < m_dwIndex - dwIndex + 1; i++)
	{
		m_pVector[dwIndex + i] = m_pVector[dwIndex + i + 1];
	}
	--m_dwIndex;
	return VOID();
}

//����vector_Ԫ�������Ĵ�С
template<class T_ELE>
inline DWORD vector_<T_ELE>::size()
{
	return m_dwIndex;
}

//��������(����arr���������������Ҳ���������)
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
	//��������һ���ڴ�,��СΪnLength
	T_ELE* pNewBuff = (T_ELE * )Preventdebug.g_HeapAlloc(Preventdebug.g_fnGetProcessHeap(), 8, sizeof(T_ELE)*nLength);

	//��ԭʼ���ݿ��������ڴ�
	Preventdebug.pmemcpy(pNewBuff, m_pVector, m_dwLen * sizeof(T_ELE));
	
	//�ͷ�ԭ���Ŀռ�
	Preventdebug.g_HeapFree(Preventdebug.g_fnGetProcessHeap(), 1, m_pVector);

	//����ָ�븳ֵ��m_pVector
	m_pVector = pNewBuff;

	_first = m_pVector;

	//Ϊ�������Ը�ֵ
	m_dwLen = nLength;

	return SUCCESS;
}