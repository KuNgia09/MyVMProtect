#pragma once
#include <vector>
#include <basetsd.h>
using namespace std;

class AllocMemory
{
	vector<char*>p;	
public:
	virtual ~AllocMemory()
	{
		for (int i = 0; i < p.size(); i++)
		{
			if (p[i]==0)
			{
				continue;
			}
			free(p[i]);
			p[i] = 0;
		}
		p.clear();
	}

public:
	template<typename T>
	T auto_malloc( ULONG_PTR MAXSIZE)
	{
		T tmp = (T)malloc(MAXSIZE);
		memset((char*)tmp, 0, MAXSIZE);
		p.push_back((char*)tmp);
		return tmp;
	}
};
