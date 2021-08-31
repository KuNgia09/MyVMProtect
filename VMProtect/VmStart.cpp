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
    int inslen = 0;//��¼ָ�������
    int hcodelength = 0;//��¼Ӳ���볤��
    bool istrue = true;//��ȡָ���Ƿ�ɹ�
};

//JCCָ���ַ���
#define JCCNUMSS1 30
const char* JCCSTR1[JCCNUMSS1] = {
    "JE ","JZ ","JNE ","JNZ ","JS ","JNS ","JP ","JPE ","JNP ",
    "JPO ","JO ","JNO ","JC","JB ","JNAE ","JNC ","JNB ","JAE ","JBE ","JNA ",
    "JNBE ","JA ","JL","JNGE ","JNL ","JGE ","JLE ","JNG ","JNLE ","JG "
};

//�����,���㺯���ĳ���
FUNCANDINSLENGTH Disassembler_(DWORD virtualaddr, DWORD instruction)
{
    FUNCANDINSLENGTH fuclen;
    int inslength = 0;//��¼ָ�������

    vector<DWORD>JccAddr;//��¼JCC����ĵ�ַ

    DISASM disAsm = { 0 };

    // 3. ���ýṹ�壬��ʼ��������opcode
    disAsm.EIP = (UIntPtr)instruction;
    disAsm.VirtualAddr = virtualaddr; // opcode ָ��ĵ�ַ
    disAsm.Archi = 0; // 0 => 32 , 1 => 64
    disAsm.Options = 0x000; // masm ���ָ���ʽ

    int nCount = 0;// ���ڼ�¼��ѭ�����У�������˶��ٸ��ֽ�
    int nLen = 0; // ���ڼ�¼��ǰ�Ļ��ָ����ֽ���

    while (true)
    {
        nLen = Disasm(&disAsm); // ÿ��ֻ�����һ�����ָ� ���ҷ��ص�ǰ�õ��Ļ��ָ��ĳ���
        unsigned int uAddr = disAsm.VirtualAddr;

        if (inslength == 0x1000)
        {//��ֹ��ѭ��
            MessageBoxA(NULL, "δ֪���󣬶�ȡָ��ʧ��!!", "��ʾ", MB_OK);
            fuclen.istrue = false;
            return fuclen;
        }

        ++inslength;//��¼ָ���������Ϊ���������ڴ��ṩ����
        nCount += nLen; // �ۼ��Ѿ��������ֽ���
        disAsm.EIP += nLen; // ��λ����һ�����ָ��
        disAsm.VirtualAddr += nLen; // ���õ���һ�����ָ��ĵ�ַ

        //�������jccָ�����jmpָ���¼����ĵ�ַ
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


//DWORD dataencrypt = 0;//���ݱ���ܵı���


//��¼������Ϣ�Ľṹ��
struct FUNCINFOR
{
    int funclength;//��������(����Ӳ����)
    int inslen;//ָ������
    int funaddr;//������ַ
    int buffaddr;//���ڴ��ʵ�ʵ�ַ
};
vector< FUNCINFOR>funcinfor;

//��¼�ڴ����˶���
int g_calcmem = 0;

void loadvm(ULONG_PTR startaddr, PEInfo peinfo, ULONG_PTR StretchAddr)
{
    //ѡ��Ҫ�����ĺ�������ʼλ��
    ULONG_PTR proaddr = startaddr - peinfo.ImageBase + StretchAddr;

    //����Disassembler_����������Ҫ�����ĺ����Ľ���λ��
    FUNCANDINSLENGTH funcins;
    funcins = Disassembler_(startaddr, proaddr);


    FUNCINFOR funcinfor_l;
    funcinfor_l.buffaddr = proaddr;
    funcinfor_l.funaddr = startaddr;
    //Ҫ�����ĺ����ĳ���
    funcinfor_l.funclength = funcins.hcodelength;
    funcinfor_l.inslen = funcins.inslen;

    //�ѵõ��ĺ�����Ϣ���뵽funcinfor����
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

    //vmp1�����ڴ��п�ʼ��λ��
    int vmpstart = imageofsize + peinfo.ImageBase + g_calcmem;

    //��������ַ
    int dispatcheraddr = vmpstart;

    //esi��ת��ĵ�ַ
    int codeaddr = vmpstart + CODETABLEADDR;

    //ebp���ݱ��ַ
    int datatbladdr = vmpstart + DISPATCHERTABLEADDR;

    //ָ�ʼ�ĵ�ַ(Ҫͨ��esi��ת���λ�ü���esi��ת��Ĺ��㳤�ȵõ�)
    int vmstartaddr = codeaddr + inslen;

    //vCheckESP�����ĵ�ַ
    int checkespaddr = vmpstart + VCHECKESPADDR;

    //vRetnNOT������ַ
    int vretnnotaddr = vmpstart + VRETNOTADDR;

    //vResumeStart�����ĵ�ַ
    int vresumestartaddr = vmpstart + VRESUMESTARTADDR;

    //����esi״̬�ĵ�ַ
    int esistateaddr = vmpstart + ESISTATEADDR;

    //����ebp״̬�ĵ�ַ
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
    //��vmps��ֵ��ȫ�ֵ�g_vmstruct������vmtest.cpp��ĺ�������
    g_vmstruct = vmps;

    VMLoader2 vmlder(vmps);


    //���ݱ����ȡ�����
    dataencrypt = vmlder.SrandNum(buffaddr, buffaddr + 0x1000);
    //����ָ��
    vmlder.MainVMP();

    //���vCheckESP
    int check_len = GetFunSize(VCheckESP);
    memcpy( vmp1_buff+ VCHECKESPADDR, (char*)VCheckESP, check_len + 0x30);
    //�޸���ַ
    *(int*)(vmp1_buff + VCHECKESPADDR + check_len - 6) = dispatcheraddr;


    //��������
    int dispatcher_len = GetFunSize(VMDispatcher);
    memcpy(vmp1_buff, (char*)VMDispatcher, dispatcher_len + 0x30);

    //��俪ͷ����ָ�������
    DWORD outaddr = (int)vmp1_buff - g_calcmem + OUTOFORDERADDR;
    vector<DWORD>::iterator iter_outoforder = g_outoforder.begin();
    for (int i = 0; i < g_outoforder.size(); i++)
    {
        *(DWORD*)(outaddr + i * 4) = (*iter_outoforder);
        ++iter_outoforder;
    }

    //�����ת��
    int lesitable = (int)vmp1_buff + CODETABLEADDR;

    vector<JMPTABLE>::iterator iter_jesi = vmlder.m_jmptbl.begin();
    for (int i = 0; i < vmlder.m_jmptbl.size(); i++)
    {
        *(int*)(lesitable + i * 4) = (*iter_jesi).addr;
        ++iter_jesi;
    }
    //������ݱ�
    int esptable = (int)vmp1_buff + DISPATCHERTABLEADDR;
    vector<DATATABLE>::iterator iter_debp = vmlder.m_datatable.begin();
    for (int i = 0; i < vmlder.m_datatable.size(); i++)
    {
        *(int*)(esptable + i * 4) = (((*iter_debp).data) ^ dataencrypt);//���ݼ��ܺ��ڴ������ݱ���
        ++iter_debp;
    }


    //��Ŀ���ļ������ת��ַ
    int jmptovm_len = GetFunSize(JmpToVMStart);
    memcpy((char*)buffaddr, JmpToVMStart, jmptovm_len);
    //�޸���ַ
    *(int*)(buffaddr + 10) = dispatcheraddr;/*vmpstart + VMSTARTADDR;*//*g_vmstruct.vmstartaddr; */
    *(int*)(buffaddr + 3) = codeaddr;
    //�����������ָ��
    for (int i = 0; i < funclength - jmptovm_len; i++)
    {
        *(char*)(buffaddr + jmptovm_len + i) = 0xCC;
    }


    //-----------------------------��Ҫ-----------------------------
    //����g_calcmem�����ݣ�������һ��Ҫ�����ĺ���ʹ��

    g_calcmem += pe.AlignSize(vmlder.m_szcalc + inslen + CODETABLEADDR, peinfo.SectionAlignment);

}

void testprofunc(char* path, vector<DWORD>temp_addr)
{
    AllocMemory l_alloc;
    PEInfo peinfo;
    PE pe;
    VMPStruct vmps;
    pe.GetPEInformation_(path, &peinfo);

    //----ȥ��������ػ�ַ----
    peinfo.OptionalHeader->DllCharacteristics = 0;

    //��¼�����С
    DWORD mirrsize = peinfo.ImageBase + pe.AlignSize(peinfo.SizeofImage, peinfo.SectionAlignment);

    if (0 == temp_addr.size())
    {
        //IAT����(�������ļ�)
        Pack pack;
        pack.StartProtect(0, path, (char*)peinfo.FileBuffer, peinfo, 0x15, mirrsize + OUTOFORDERADDR);
        return;
    }


    //�����ļ�
    ULONG_PTR StretchAddr = pe.StretchFile(peinfo.FileBuffer, peinfo.SizeofImage);
    //����peinfo��Ϣ
    pe.GetPEInformation_1((char*)StretchAddr, &peinfo, peinfo.FileSize);


    vector<DWORD>::iterator taddr = temp_addr.begin();
    for (int i = 0; i < temp_addr.size(); i++)
    {
        int x = *(taddr + i);
        loadvm(*(taddr+i), peinfo, StretchAddr);
    }


    int addmem = 0;//���ж�����Ҫ�����ĺ�����Ȼ�������ڴ�
    vector< FUNCINFOR>::iterator iter_func = funcinfor.begin();
    for (int i = 0; i < funcinfor.size(); i++)
    {
        if ((*iter_func).funclength < 18)
        {//�������������Ӳ����ĳ���С��18��ֱ���˳�
            MessageBoxA(NULL, "��Ҫ�����ĺ���ָ��̫�٣��޷����⻯!", "������ʾ!", MB_OK);
            return;
        }
        addmem += calculatelength(((*iter_func).inslen));
        ++iter_func;
    }

    //�����ڴ�
    int newsectinofsize = 1024 * 1024 * (3 + addmem / 3);//.vmp1���ڽڵĴ�С������ָ���������������Ҫ���ڴ�
    char* vmp1_buff = l_alloc.auto_malloc<char*>(newsectinofsize);


    //vmloader_��������Ҫ�����ĺ���
    for (int i = 0; i < funcinfor.size(); i++)
    {
        vmloader_(pe, peinfo, vmp1_buff, i);
    }


    //��ԭ��Դ�ļ���С
    char* filebuff = pe.ImageBuff_To_FileBuff((char*)StretchAddr, &peinfo.FileSize);


    //�½ڵĴ�С
    int newSectionSize = g_calcmem;


    //����½�
    pe.addSeciton((DWORD)filebuff, newSectionSize, (char*)".vmp3");

    //�����ڴ棬�ϲ���
    char* NewFileBuff = l_alloc.auto_malloc<char*>(newSectionSize + peinfo.FileSize);
    memcpy(NewFileBuff, filebuff, peinfo.FileSize);
    memcpy(NewFileBuff + peinfo.FileSize, vmp1_buff, newSectionSize);

    //����peinfo��Ϣ
    pe.GetPEInformation_1((char*)NewFileBuff, &peinfo, newSectionSize + peinfo.FileSize);
    VMLoader2 ls;
    

    //IAT����(�������ļ�)
    Pack pack;
    pack.StartProtect(0, path, NewFileBuff, peinfo, 0x15, mirrsize + OUTOFORDERADDR);

}


void IATEncypt(char* VR0, char* VR1)
{
    CString str = "";
    str = str + "mov " + VR0 + ",ebp\n";
    str = str + "mov " + VR0 + ",dword ptr[" + VR0 + "]\n";
    //str = str + "xor " + VR0 + "," + ��Կ;      //��1����
    //str = str + "add " + VR0 + "," + ��Կ;      //��2����
    //str = str + "xor " + VR0 + "," + ��Կ;      //��3����
    //str = str + "sub " + VR0 + "," + ��Կ;      //��4����
    str = str + "popfd\n";
    str = str + "popad\n";
    str = str + "jmp " + VR0 + "\n"; //������ָ��
}

int VMStart(char* path_in, vector<DWORD>temp_addr)
{
    testprofunc((char*)path_in, temp_addr);

    return 0;
}