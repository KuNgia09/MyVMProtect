/*
* ��ָ�����

*/

#pragma once
#include "Vector_.h"
#include <winnt.h>
using namespace std;

struct BUFFERSTRUCT_
{
    char value = 0;
    char match = 0;
    char jmpmatch = 0;
    char recodemodify = 0;//��¼IATҪ�޸����ݵ�λ��
};

class JunkCode_
{
public:
    vector_<BUFFERSTRUCT_>g_buffer;
public:

    //���������ָ��
    void SrandJunkCode();

    //�������
    void srand_v(DWORD num);
    //��ȡ�����
    DWORD rand_v();
    
};

