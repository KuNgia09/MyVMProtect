#include <iostream>

#include "Common/header.h"
#include <algorithm>
#include "vmserv.h"
#include "vmtest.h"
#include "VMLoader2.h"
#include "Pack.h"
#include "holder.h"


vector<DWORD>g_outoforder;
struct FUNCANDINSLENGTH
{
    int inslen = 0;//记录指令的条数
    int hcodelength = 0;//记录硬编码长度
    bool istrue = true;//读取指令是否成功
};

//JCC指令字符串
#define JCCNUMSS1 30
const char* JCCSTR1[JCCNUMSS1] = {
    "JE ","JZ ","JNE ","JNZ ","JS ","JNS ","JP ","JPE ","JNP ",
    "JPO ","JO ","JNO ","JC","JB ","JNAE ","JNC ","JNB ","JAE ","JBE ","JNA ",
    "JNBE ","JA ","JL","JNGE ","JNL ","JGE ","JLE ","JNG ","JNLE ","JG "
};

//反汇编,计算函数的长度
FUNCANDINSLENGTH Disassembler_(DWORD virtualaddr, DWORD instruction)
{
    FUNCANDINSLENGTH fuclen;
    int inslength = 0;//记录指令的条数

    vector<DWORD>JccAddr;//记录JCC后面的地址

    DISASM disAsm = { 0 };

    // 3. 配置结构体，初始化反汇编的opcode
    disAsm.EIP = (UIntPtr)instruction;
    disAsm.VirtualAddr = virtualaddr; // opcode 指令的地址
    disAsm.Archi = 0; // 0 => 32 , 1 => 64
    disAsm.Options = 0x000; // masm 汇编指令格式

    int nCount = 0;// 用于记录在循环当中，反汇编了多少个字节
    int nLen = 0; // 用于记录当前的汇编指令的字节数

    while (true)
    {
        nLen = Disasm(&disAsm); // 每次只反汇编一条汇编指令， 并且返回当前得到的汇编指令的长度
        unsigned int uAddr = disAsm.VirtualAddr;

        if (inslength == 0x1000)
        {//防止死循环
            MessageBoxA(NULL, "未知错误，读取指令失败!!", "提示", MB_OK);
            fuclen.istrue = false;
            return fuclen;
        }

        ++inslength;//记录指令的条数，为后面申请内存提供依据
        nCount += nLen; // 累加已经反汇编的字节数
        disAsm.EIP += nLen; // 定位到下一条汇编指令
        disAsm.VirtualAddr += nLen; // 设置到下一条汇编指令的地址

        //如果遇到jcc指令或者jmp指令，记录后面的地址
        for (int i = 0; i < JCCNUMSS1; i++)
        {
            if (0 == _stricmp(disAsm.Instruction.Mnemonic, JCCSTR1[i]))
            {
                DWORD tempaddr = 0;
                sscanf_s(disAsm.Argument1.ArgMnemonic, "%X", &tempaddr);
                JccAddr.push_back(tempaddr);
                break;
            }
        }

        if (
            (0 == _stricmp(disAsm.Instruction.Mnemonic, "ret ")) ||
            (0 == _stricmp(disAsm.Instruction.Mnemonic, "retn ")) ||
            (0 == _stricmp(disAsm.Instruction.Mnemonic, "int3 "))
            )
        {
            int isretn = 1;
            for (int i = 0; i < JccAddr.size(); i++)
            {
                if ((*(JccAddr.begin() + i)) > disAsm.VirtualAddr - nLen)
                {
                    isretn = 0;
                    break;
                }
            }
            if (isretn || (0 == _stricmp(disAsm.Instruction.Mnemonic, "int3 ")))
            {
                fuclen.hcodelength = nCount;
                fuclen.inslen = inslength;
                return fuclen;
            }
        }
    }
}


//DWORD dataencrypt = 0;//数据表加密的变量


//记录函数信息的结构体
struct FUNCINFOR
{
    int funclength;//函数长度(多少硬编码)
    int inslen;//指令条数
    int funaddr;//函数地址
    int buffaddr;//在内存的实际地址
};
vector< FUNCINFOR>funcinfor;

//记录内存用了多少
int g_calcmem = 0;

void loadvm(ULONG_PTR startaddr, PEInfo peinfo, ULONG_PTR StretchAddr)
{
    //选择要保护的函数的起始位置
    ULONG_PTR proaddr = startaddr - peinfo.ImageBase + StretchAddr;

    //调用Disassembler_函数，计算要保护的函数的结束位置
    FUNCANDINSLENGTH funcins;
    funcins = Disassembler_(startaddr, proaddr);


    FUNCINFOR funcinfor_l;
    funcinfor_l.buffaddr = proaddr;
    funcinfor_l.funaddr = startaddr;
    //要保护的函数的长度
    funcinfor_l.funclength = funcins.hcodelength;
    funcinfor_l.inslen = funcins.inslen;

    //把得到的函数信息存入到funcinfor链表
    funcinfor.push_back(funcinfor_l);

}

int calculatelength(int x)
{
    int res = 0;
    while (true)
    {
        if (x <= 10)
        {
            res += 1;
            break;
        }
        x -= 10;
        res += 1;
    }
    return res;
}


void vmloader_(PE& pe, PEInfo& peinfo, char* vmp1_buff, int index)
{

    vector< FUNCINFOR>::iterator iter_func = funcinfor.begin();
    DWORD inslen = (calculatelength(((*(iter_func + index)).inslen))) * 0x9000;
    DWORD funaddr = (*(iter_func + index)).funaddr;
    DWORD buffaddr = (*(iter_func + index)).buffaddr;
    DWORD funclength = (*(iter_func + index)).funclength;

    int imageofsize = pe.AlignSize(peinfo.SizeofImage, peinfo.SectionAlignment);

    //vmp1节在内存中开始的位置
    int vmpstart = imageofsize + peinfo.ImageBase + g_calcmem;

    //调度器地址
    int dispatcheraddr = vmpstart;

    //esi跳转表的地址
    int codeaddr = vmpstart + CODETABLEADDR;

    //ebp数据表地址
    int datatbladdr = vmpstart + DISPATCHERTABLEADDR;

    //指令开始的地址(要通过esi跳转表的位置加上esi跳转表的估算长度得到)
    int vmstartaddr = codeaddr + inslen;

    //vCheckESP函数的地址
    int checkespaddr = vmpstart + VCHECKESPADDR;

    //vRetnNOT函数地址
    int vretnnotaddr = vmpstart + VRETNOTADDR;

    //vResumeStart函数的地址
    int vresumestartaddr = vmpstart + VRESUMESTARTADDR;

    //保存esi状态的地址
    int esistateaddr = vmpstart + ESISTATEADDR;

    //保存ebp状态的地址
    int ebpstateaddr = vmpstart + EBPSTATEADDR;

    vmp1_buff += g_calcmem;


    VMPStruct vmps;
    vmps.vmp1_startaddr = vmpstart;
    vmps.jmptbladdr = codeaddr;
    vmps.datatbladdr = datatbladdr;
    vmps.dispatcheraddr = dispatcheraddr;
    vmps.instbuffaddr = (int)vmp1_buff + inslen + CODETABLEADDR;
    vmps.length = funclength;
    vmps.originaddr = funaddr;
    vmps.startaddr = buffaddr;
    vmps.vcheckespaddr = checkespaddr;
    vmps.vmstartaddr = vmstartaddr;
    vmps.vresumestartaddr = vresumestartaddr;
    vmps.vretnotaddr = vretnnotaddr;
    vmps.esistateaddr = esistateaddr;
    vmps.ebpstateaddr = ebpstateaddr;
    //把vmps赋值给全局的g_vmstruct，方便vmtest.cpp里的函数调用
    g_vmstruct = vmps;

    VMLoader2 vmlder(vmps);


    //数据表加密取随机数
    dataencrypt = vmlder.SrandNum(buffaddr, buffaddr + 0x1000);
    //解析指令
    vmlder.MainVMP();

    //填充vCheckESP
    int check_len = GetFunSize(VCheckESP);
    memcpy( vmp1_buff+ VCHECKESPADDR, (char*)VCheckESP, check_len + 0x30);
    //修复地址
    *(int*)(vmp1_buff + VCHECKESPADDR + check_len - 6) = dispatcheraddr;


    //填充调度器
    int dispatcher_len = GetFunSize(VMDispatcher);
    memcpy(vmp1_buff, (char*)VMDispatcher, dispatcher_len + 0x30);

    //填充开头垃圾指令的数据
    DWORD outaddr = (int)vmp1_buff - g_calcmem + OUTOFORDERADDR;
    vector<DWORD>::iterator iter_outoforder = g_outoforder.begin();
    for (int i = 0; i < g_outoforder.size(); i++)
    {
        *(DWORD*)(outaddr + i * 4) = (*iter_outoforder);
        ++iter_outoforder;
    }

    //填充跳转表
    int lesitable = (int)vmp1_buff + CODETABLEADDR;

    vector<JMPTABLE>::iterator iter_jesi = vmlder.m_jmptbl.begin();
    for (int i = 0; i < vmlder.m_jmptbl.size(); i++)
    {
        *(int*)(lesitable + i * 4) = (*iter_jesi).addr;
        ++iter_jesi;
    }
    //填充数据表
    int esptable = (int)vmp1_buff + DISPATCHERTABLEADDR;
    vector<DATATABLE>::iterator iter_debp = vmlder.m_datatable.begin();
    for (int i = 0; i < vmlder.m_datatable.size(); i++)
    {
        *(int*)(esptable + i * 4) = (((*iter_debp).data) ^ dataencrypt);//数据加密后在存入数据表中
        ++iter_debp;
    }


    //在目标文件添加跳转地址
    int jmptovm_len = GetFunSize(JmpToVMStart);
    memcpy((char*)buffaddr, JmpToVMStart, jmptovm_len);
    //修复地址
    *(int*)(buffaddr + 10) = dispatcheraddr;/*vmpstart + VMSTARTADDR;*//*g_vmstruct.vmstartaddr; */
    *(int*)(buffaddr + 3) = codeaddr;
    //清除后面多余的指令
    for (int i = 0; i < funclength - jmptovm_len; i++)
    {
        *(char*)(buffaddr + jmptovm_len + i) = 0xCC;
    }


    //-----------------------------重要-----------------------------
    //更新g_calcmem的数据，方便下一个要保护的函数使用

    g_calcmem += pe.AlignSize(vmlder.m_szcalc + inslen + CODETABLEADDR, peinfo.SectionAlignment);

}

void testprofunc(char* path, vector<DWORD>temp_addr)
{
    AllocMemory l_alloc;
    PEInfo peinfo;
    PE pe;
    VMPStruct vmps;
    pe.GetPEInformation_(path, &peinfo);

    //----去掉随机加载基址----
    peinfo.OptionalHeader->DllCharacteristics = 0;

    //记录镜像大小
    DWORD mirrsize = peinfo.ImageBase + pe.AlignSize(peinfo.SizeofImage, peinfo.SectionAlignment);

    if (0 == temp_addr.size())
    {
        //IAT加密(并保存文件)
        Pack pack;
        pack.StartProtect(0, path, (char*)peinfo.FileBuffer, peinfo, 0x15, mirrsize + OUTOFORDERADDR);
        return;
    }


    //拉伸文件
    ULONG_PTR StretchAddr = pe.StretchFile(peinfo.FileBuffer, peinfo.SizeofImage);
    //更新peinfo信息
    pe.GetPEInformation_1((char*)StretchAddr, &peinfo, peinfo.FileSize);


    vector<DWORD>::iterator taddr = temp_addr.begin();
    for (int i = 0; i < temp_addr.size(); i++)
    {
        int x = *(taddr + i);
        loadvm(*(taddr+i), peinfo, StretchAddr);
    }


    int addmem = 0;//看有多少需要保护的函数，然后申请内存
    vector< FUNCINFOR>::iterator iter_func = funcinfor.begin();
    for (int i = 0; i < funcinfor.size(); i++)
    {
        if ((*iter_func).funclength < 18)
        {//如果整个函数的硬编码的长度小于18则直接退出
            MessageBoxA(NULL, "所要保护的函数指令太少，无法虚拟化!", "错误提示!", MB_OK);
            return;
        }
        addmem += calculatelength(((*iter_func).inslen));
        ++iter_func;
    }

    //申请内存
    int newsectinofsize = 1024 * 1024 * (3 + addmem / 3);//.vmp1所在节的大小，根据指令的条数来估算需要的内存
    char* vmp1_buff = l_alloc.auto_malloc<char*>(newsectinofsize);


    //vmloader_函数处理要保护的函数
    for (int i = 0; i < funcinfor.size(); i++)
    {
        vmloader_(pe, peinfo, vmp1_buff, i);
    }


    //还原成源文件大小
    char* filebuff = pe.ImageBuff_To_FileBuff((char*)StretchAddr, &peinfo.FileSize);


    //新节的大小
    int newSectionSize = g_calcmem;


    //添加新节
    pe.addSeciton((DWORD)filebuff, newSectionSize, (char*)".vmp3");

    //申请内存，合并节
    char* NewFileBuff = l_alloc.auto_malloc<char*>(newSectionSize + peinfo.FileSize);
    memcpy(NewFileBuff, filebuff, peinfo.FileSize);
    memcpy(NewFileBuff + peinfo.FileSize, vmp1_buff, newSectionSize);

    //更新peinfo信息
    pe.GetPEInformation_1((char*)NewFileBuff, &peinfo, newSectionSize + peinfo.FileSize);
    VMLoader2 ls;
    

    //IAT加密(并保存文件)
    Pack pack;
    pack.StartProtect(0, path, NewFileBuff, peinfo, 0x15, mirrsize + OUTOFORDERADDR);

}


void IATEncypt(char* VR0, char* VR1)
{
    CString str = "";
    str = str + "mov " + VR0 + ",ebp\n";
    str = str + "mov " + VR0 + ",dword ptr[" + VR0 + "]\n";
    //str = str + "xor " + VR0 + "," + 密钥;      //第1解密
    //str = str + "add " + VR0 + "," + 密钥;      //第2解密
    //str = str + "xor " + VR0 + "," + 密钥;      //第3解密
    //str = str + "sub " + VR0 + "," + 密钥;      //第4解密
    str = str + "popfd\n";
    str = str + "popad\n";
    str = str + "jmp " + VR0 + "\n"; //跳到花指处
}

int VMStart(char* path_in, vector<DWORD>temp_addr)
{
    testprofunc((char*)path_in, temp_addr);

    return 0;
}