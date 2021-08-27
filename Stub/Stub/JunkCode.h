/*
* 花指令构造器

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
    char recodemodify = 0;//记录IAT要修改数据的位置
};

class JunkCode_
{
public:
    vector_<BUFFERSTRUCT_>g_buffer;
public:

    //构造随机花指令
    void SrandJunkCode();

    //随机因子
    void srand_v(DWORD num);
    //获取随机数
    DWORD rand_v();
    
};

