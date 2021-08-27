#include "JunkCode.h"


//随机数种子
DWORD holdrand = 0;


char jncode_one = 0xEB;

char jncode[0x4] = { 0xE8,0xE9,0xFF,0xEB };

char second[0x2] = { 0x15,0x25 };

char moveax[5] = { 0xB8,0x00,0x00,0x00,0x00 };

char jmpoep[7] = { 0x35,0x68,0x68,0x68,0x68,0x50,0xC3 };

#define RANDSSS 24
char randsss[RANDSSS] = {
    0xE9,0x90,0xE8,0xE9,0xFF,0xEB,0x15,0x25 ,0x6A,0x07,0x27,0xFF,0x75,0x8E,0x5E,
    0x15,0x25,0x40,0x41,0x42,0x43,0x44,0x54,0x50
};



void JunkCode_::SrandJunkCode()
{
    BUFFERSTRUCT_ buffer;
    buffer.value = jncode_one;
    buffer.match = 1;
    g_buffer.push_back(buffer);

    buffer.value = buffer.match = 0;
    g_buffer.push_back(buffer);

    char x = jncode[rand_v() % 4];
    buffer.value = x;
    g_buffer.push_back(buffer);


    if (x == 0xFF)
    {
        buffer.value = second[rand_v() % 2];
        g_buffer.push_back(buffer);
    }

    int y = rand_v() % 3;

    for (int i = 0; i < y; i++)
    {
        buffer.value = randsss[rand_v() % RANDSSS];
        g_buffer.push_back(buffer);
    }


    buffer.value = jncode_one;
    buffer.match = 0x3;
    buffer.jmpmatch = 0x2;
    g_buffer.push_back(buffer);

    buffer.value = buffer.match = buffer.jmpmatch = 0;
    g_buffer.push_back(buffer);

    x = jncode[rand_v() % 4];
    buffer.value = x;
    g_buffer.push_back(buffer);


    if (x == 0xFF)
    {
        buffer.value = second[rand_v() % 2];
        g_buffer.push_back(buffer);
    }

    y = rand_v() % 3;

    for (int i = 0; i < y; i++)
    {
        buffer.value = randsss[rand_v() % RANDSSS];
        g_buffer.push_back(buffer);
    }

    for (int i = 0; i < 5; i++)
    {
        if (i == 0)
        {
            buffer.jmpmatch = 1;
            buffer.recodemodify = 1;
            buffer.value = moveax[i];
            g_buffer.push_back(buffer);
            buffer.jmpmatch = buffer.recodemodify = 0;
            continue;
        }
        buffer.value = moveax[i];
        g_buffer.push_back(buffer);
    }


    buffer.value = jncode_one;
    buffer.match = 0x2;
    g_buffer.push_back(buffer);

    buffer.value = buffer.match = buffer.jmpmatch = 0;
    g_buffer.push_back(buffer);


    x = jncode[rand_v() % 4];
    buffer.value = x;
    g_buffer.push_back(buffer);


    if (x == 0xFF)
    {
        buffer.value = second[rand_v() % 2];
        g_buffer.push_back(buffer);
    }

    y = rand_v() % 2;

    for (int i = 0; i < y; i++)
    {
        buffer.value = randsss[rand_v() % RANDSSS];
        g_buffer.push_back(buffer);
    }

    for (int i = 0; i < 7; i++)
    {
        if (i == 0)
        {
            buffer.jmpmatch = 3;    //3
            buffer.value = jmpoep[i];
            g_buffer.push_back(buffer);
            buffer.match = buffer.jmpmatch = 0;
            continue;
        }

        buffer.value = jmpoep[i];
        g_buffer.push_back(buffer);
    }

    //修复数据
    vector_< BUFFERSTRUCT_>::iterator iter_buff = g_buffer.begin();
    vector_< BUFFERSTRUCT_>::iterator iter_buff_1 = g_buffer.begin();
    for (int i = 0; i < g_buffer.size(); i++)
    {

        if ((*iter_buff).match != 0)
        {
            int temp = (*iter_buff).match;
            for (int j = 0; j < g_buffer.size(); j++)
            {
                if (temp == (*iter_buff_1).jmpmatch)
                {
                    (*(iter_buff + 1)).value= j - i - 2;
                    iter_buff_1 = g_buffer.begin();
                    break;

                }
                ++iter_buff_1;
            }
        }
        ++iter_buff;
    }

}

void JunkCode_::srand_v(DWORD num)
{
    if (holdrand!=0)
    {
        return;
    }
    holdrand = num;
}


DWORD JunkCode_::rand_v()
{ 
    DWORD res = 0;
    _asm {
        mov eax, holdrand
        imul eax, eax, 343fdh
        add eax, 269ec3h
        mov holdrand, eax
        mov eax, holdrand
        sar eax, 10h
        and eax, 7fffh
        mov res, eax
    }

    return res;
}
