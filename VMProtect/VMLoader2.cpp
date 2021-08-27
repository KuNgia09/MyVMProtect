#include "VMLoader2.h"
#include "vmtest.h"
#include "vmserv.h"
#include "../Stub/Stub/Stub.h"

extern VMTable vmtable32[VMTABLEMAXLEN];
extern VMPStruct g_vmstruct;
extern vector<DWORD>g_outoforder;
struct FUNCNAME
{
	HANDLERFUN s_Func;
	char s_opecode[OPECODELENGTH];
	char vm_opcoed[OPECODELENGTH];
};

#define FUNNUMS 56
FUNCNAME g_FunName[FUNNUMS] = {
	{vJBE,"jbe " ,"vJBE "},{vJBE,"jna " ,"vJBE "},{vJB,"jb "," vJB "},{vJB,"jnae "," vJB "},{vJB,"jc "," vJB "},
	{vJA,"ja ","vJA "},{vJA,"jnbe ","vJA "},{vJG,"jg ","vJG "},{vJG,"jnle ","vJG "},{vJL,"jl ","vJL "},{vJL,"jnge ","vJL "},
	{vJLE,"jle ","vJLE "},{vJLE,"jng ","vJLE "},{vJZ,"jz ","vJZ "},{vJZ,"je ","vJZ "},{vJNZ,"jnz ","vJNZ "},{vJNZ,"jne ","vJNZ "},
	{vJAE,"jae ","vJAE "},{vJAE,"jnb ","vJAE "},{vJO,"jo ","vJO "},{vJNO,"jno ","vJNO "},{vJS,"js ","vJS "},{vJNS,"jns ","vJNS "},
	{vJPE,"jpe ","vJPE "},{vJPE,"jp ","vJPE "},{vJPO,"jpo ","vJPO "},{vJPO,"jnp ","vJPO "},{vJCXZ,"jcxz ","vJCXZ "},{vJECXZ,"jecxz ","vJECXZ "},
	{vJAE,"jnc ","vJAE "},{vJGE,"jnl ","vJGE "},{vJGE,"jge ","vJGE "},{vJMP,"jmp ","vJMP "},{vMOV4,"mov ","vMOV4 "},{vSUB4,"sub ","vSUB4 "},
	{vAdd4,"add ","vAdd4 "},{vPushImm4," ","vPushImm4 "},{vRETN,"retn ","vRETN "},{vRETN,"ret ","vRETN "} ,{vCALL,"call ","vCALL "},{vXOR4,"xor ","vXOR4 "},
	{vPushReg2," ","vPushReg2 "},{vPopReg2," ","vPopReg2 "},{vPushReg4," ","vPushReg4 "},{vPopReg4," ","vPopReg4 "},
	{vPushReg1_low," ","vPushReg1_low "},{vPushReg1_above," ","vPushReg1_above "},{vPopReg1_low," ","vPopReg1_low "},{vPopReg1_above," ","vPopReg1_above "},
	{vCMP,"cmp ","vCMP "},{vTEST,"test ","vTEST "},{vPUSH,"push ","vPUSH "},{vPOP4,"pop ","vPOP4 "},{vLEA,"lea ","vLEA "},{vAddEsp," ","vAddEsp "},
	{vINC,"inc ","vINC "}
};

//Ԥ����һЩhandler�����ٹ�����
#define PRENUMS 8
FUNCNAME Predict[PRENUMS] = {
	{vPopReg4,"pop ","vPopReg4 "},{vPushReg4,"push ","vPushReg4 "},{vPushImm4,"push ","vPushImm4 "},
	{vRetnNOT_," ","vRetnNOT_ "},{vResumeStart_2," ","vResumeStart_2 "},{vResumeStart_1," ","vResumeStart_1 "},
	{VMStartVM,"","VMStartVM "},{VMStartVM_2,"","VMStartVM_2 "}
};

//�����Ϊ�ڴ�handler�Ͳ���ģ��ָ��,�Լ�һЩ����ָ����Ƶ�
#define MEMNUMS 13
FUNCNAME g_FuncMem[MEMNUMS] = {
	{ vWriteMemDs4,"  ","vWriteMemDs4 "},{vWriteMemDs2," ","vWriteMemDs2 "},{vWriteMemDs1," ","vWriteMemDs1 "},
	{vReadMemDs4," ","vReadMemDs4 "},{vReadMemDs2," ","vReadMemDs2 "},{vReadMemDs1," ","vReadMemDs1 "},
	{vNotSimulate," ","vNotSimulate "},{vCALL,"call ","vCALL "},{vMUL_MEM," ","vMUL_MEM "},{vCallMem," ","vCallMem "},
	{vPopWirteMem4," ","vPopWirteMem4 "},{vPOP2," ","vPOP2 "},{vCallReg," ","vCallReg "}
};

//���ı�ͨ�üĴ�����ָ��
#define NOTREGNUMS 3
const char* cmp_opecode[NOTREGNUMS] = { "CMP ","TEST ","PUSH " };

//�Ĵ����ṹ��
struct regstr
{
	short index;
	char reg[4];
};
#define REGNUMS 8
regstr tempreg[REGNUMS] = {
	{16,"esp"},{1,"eax"},{2,"ecx"},{4,"edx"},{8,"ebx"},{32,"ebp"},{64,"esi"},{128,"edi"}
};

//JCCָ���ַ���
#define JCCNUMS 33
const char* JCCstr[JCCNUMS] = {
	"JNBE ","JE ","JZ ","JNE ","JNZ ","JS ","JNS ","JP ","JPE ","JNP ","JCXZ ","JECXZ ",
	"JPO ","JO ","JNO ","JC ","JB ","JNAE ","JNC ","JNB ","JAE ","JBE ","JNA ",
	"JA ","JL ","JNGE ","JNL ","JGE ","JLE ","JNG ","JNLE ","JG ","JMP "
};

//�Ĵ�������
const char* regname_[3][14] =
{
	{ "AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH", "CS","DS","ES","FS","GS","SS"  },
	{ "AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI", "CS","DS","ES","FS","GS","SS" },
	{ "EAX","ECX","EDX","EBX","ESP","EBP","ESI","EDI","CS","DS","ES","FS","GS","SS" }
};

//��ѡ�Ĵ���
#define OPTIONREGNUMS 4
const char* optionreg[OPTIONREGNUMS] = { "eax","ecx","edx","ebx"};

//��ʼ��
void VMLoader2::InitialVM(VMPStruct vmps)
{
	//���������
	srand(time(0));

	//vmtable32�ṹ������ĳ���
	m_vmlength = VMLength();

	//��vmps���Ƶ�m_vmps
	m_vmps = vmps;
}

//��vmtable32�ṹ������ĳ���
int VMLoader2::VMLength()
{
	for (int i = 0; i < VMTABLEMAXLEN; i++)
	{
		if (vmtable32[i].bitnum[0] == 0x520000)
		{
			return i;
		}
	}
	return 0;
}

//����ָ���ָ���ŵ�m_basic����
void VMLoader2::XEParseIns(char* str)
{
	XEDPARSE xed = { 0 };
	xed.x64 = false;//�Ƿ���64λ
	xed.cip = (ULONGLONG)m_vmps.vmstartaddr + m_szcalc;

	//printf("%X : %s\n", (int)xed.cip, str);

	memcpy(xed.instr, str, strlen(str));
	if (XEDPARSE_OK != XEDParseAssemble(&xed))
	{
		cout << "ָ�����" << xed.error << endl;
	}
	CString strs = str;
	memcpy(temp_basic.instruction, strs.GetString(), strs.GetCount());

	m_basic.push_back(temp_basic);
	xed.cip += xed.dest_size;
	m_szcalc += xed.dest_size;

	temp_basic.index = -1;
	temp_basic.addr = 0;
	temp_basic.nmatch = 0;
	memset(temp_basic.instruction, 0, STRUCTIONLENGTH);
}

//������
void VMLoader2::Construct_Rub_Instruction(char* jmpdispatcher, int index, int nmatch)
{
	CString rubstr = "";
	JMPTABLE l_jmptbl;
	if (m_bool)
	{//��¼��ͨhandler���߸���handler������
		temp_basic.index = index;
		l_jmptbl.origin = m_origin;
		m_bool = !m_bool;
	}
	DWORD tempsnum = m_vmps.vmstartaddr + m_szcalc;
	temp_basic.addr = tempsnum;

	l_jmptbl.index = temp_basic.index;
	l_jmptbl.addr = temp_basic.addr;
	m_jmptbl.push_back(l_jmptbl);//�ѵ�ַ��¼����ת����
	int srandnum = SrandNum(6, 18);
	for (int i = 0; i < srandnum; i++)
	{
		rubstr = ProduceRubbishOpecode(m_reg04, m_reg05);
		XEParseIns(rubstr.GetString());
	}
	if (nmatch)
	{
		temp_basic.nmatch = nmatch;
		temp_basic.addr = tempsnum;
	}

	//�����ת��ַ
	XEParseIns(jmpdispatcher);
}

//�������������Ŀ
void VMLoader2::Construct_Rub_Instruction_Num(char* jmpdispatcher, int index)
{
	int srandnum = SrandNum(5, 8);
	for (int i = 0; i < srandnum; i++)
	{
		Construct_Rub_Instruction(jmpdispatcher, index);
	}
}

//��������ָ�����vm������ʼǰ
void VMLoader2::Out_Of_Order_RubInstruction()
{
	CString jmpdispatcher = "jmp ";
	jmpdispatcher = jmpdispatcher + m_vmps.dispatcheraddr;//��������ַ

	//�������ָ��
	for (int i = 0; i < 12; i++)
	{
		Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), 0);
	}

	//��m_basic��ŵ�ָ��ŵ�������
	SaveInstructionToBuffer();


	//������ָ�����Ϣ���浽g_outoforder����ݸ�Stub.dll
	g_outoforder.push_back(m_vmps.jmptbladdr);
	g_outoforder.push_back(m_jmptbl.size());

	//���m_basic��������
	m_basic.clear();

}
//JCCָ���
void VMLoader2::Construct_Jcc_Instruction(char* jccstr, int index)
{
	
	Construct_Rub_Instruction(jccstr, index,1);

}

//��������ͻ�������
void VMLoader2::Construct_BasicBlock_Instruction(char* instruction, char* jmpdispatcher, int nbool)
{
	CString rubstr = "";
	if (nbool == 3)
	{
		temp_basic.nmatch = 1;
	}
	DWORD tempaddr = m_vmps.vmstartaddr + m_szcalc;
	temp_basic.addr = tempaddr;
	JMPTABLE l_jmptbl;
	l_jmptbl.addr = tempaddr;
	m_jmptbl.push_back(l_jmptbl);

	for (int i = 0; i < 3; i++)
	{
		rubstr = ProduceRubbishOpecode(m_reg04, m_reg05);
		XEParseIns(rubstr.GetString());
	}

	XEParseIns(instruction);

	for (int i = 0; i < 3; i++)
	{
		rubstr = ProduceRubbishOpecode(m_reg04, m_reg05);
		XEParseIns(rubstr.GetString());
	}
	temp_basic.nmatch = tempaddr;
	XEParseIns((char*)"add esi,4");
	XEParseIns(jmpdispatcher);
}

//�޸�m_basic�����������
void VMLoader2::Modify_BasicData()
{
	vector<BASICBLOCKADDR>::iterator iter_basic = m_basic.begin();
	vector<JMPTABLE>::iterator iter_jmp = m_jmptbl.begin();
	CString cstr = " ";

	//�޸�JCC��תָ��
	int tempnum_[2] = { 0 };
	int lcalc = 0;
	for (int i = 0; i < m_basic.size(); i++)
	{
		if (((*iter_basic).nmatch == 1))
		{
			tempnum_[lcalc] = i;
			if ((++lcalc)==2)
			{
				break;
			}
		}
		++iter_basic;
	}

	for (int i = 0; i < m_jmptbl.size(); i++)
	{
		if ((*iter_jmp).addr > m_basic.at(tempnum_[1]).addr)
		{
			int srd = SrandNum(1, 4);//�����
			iter_jmp -= 2;
			cstr = "jge ";
			cstr = cstr + (*iter_jmp).addr;
			memcpy(m_basic.at(tempnum_[0]).instruction, cstr.GetString(), STRUCTIONLENGTH);
			break;
		}
		++iter_jmp;
	}

	//�޸���������
	lcalc = 0;
	iter_basic = m_basic.begin();
	iter_jmp = m_jmptbl.begin();
	for (int i = 0; i < m_basic.size(); i++)
	{
		if ( ((*iter_basic).nmatch!=0) && ((*iter_basic).nmatch != 1))
		{
			tempnum_[lcalc] = i;
			if ((++lcalc) == 2) break;
		}
		++iter_basic;
	}

	lcalc = 0;
	for (int i = 0; i < m_jmptbl.size(); i++)
	{
		DWORD jjj = m_basic.at(tempnum_[lcalc]).nmatch;
		if ((*iter_jmp).addr == m_basic.at(tempnum_[lcalc]).nmatch)
		{
			DWORD temp_ = (m_jmptbl.size() - i-1)*4;
			cstr = "add esi,";
			cstr = cstr + temp_;
			memcpy(m_basic.at(tempnum_[lcalc]).instruction, cstr.GetString(), STRUCTIONLENGTH);		

			if ((++lcalc) == 2)	break;		
		}
		++iter_jmp;
	}
}

//��m_basic��ŵ�ָ��ŵ�������
void VMLoader2::SaveInstructionToBuffer()
{
	XEDPARSE xed = { 0 };
	xed.x64 = false;//�Ƿ���64λ
	xed.cip = (ULONGLONG)m_vmps.vmstartaddr + m_szcalc_2;
	
	vector<BASICBLOCKADDR>::iterator iter_basic = m_basic.begin();
	for (int i = 0; i < m_basic.size(); i++)
	{
		memcpy(xed.instr, (*iter_basic).instruction, STRUCTIONLENGTH);
		if (XEDPARSE_OK != XEDParseAssemble(&xed))
		{
			//cout << "ָ�����" << xed.error << endl;
		}
		//printf("%X : %s\n", (int)xed.cip, xed.instr);
		memcpy((char*)m_vmps.instbuffaddr+ m_szcalc_2, xed.dest, xed.dest_size);
		xed.cip += xed.dest_size;
		m_szcalc_2 += xed.dest_size;
		++iter_basic;
	}	
}

//��������������ָ���
void VMLoader2::Const_BasicAndRub_Block(char* instruction, int index)
{
	CString jmpdispatcher = "jmp ";
	jmpdispatcher = jmpdispatcher + m_vmps.dispatcheraddr;//��������ַ
	
	char jccstr[] = "jge 0x66666666";
	//char jccstr[] = "mov eax,ebx";
	//1��������
	Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), index);
	
	//2��JCC��(������һ)
	Construct_Jcc_Instruction(jccstr, index);

	//3��������
	Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), index);

	//4���������
	Construct_BasicBlock_Instruction(instruction, jmpdispatcher.GetString(), 2);

	//5��������
	Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), index);
	Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), index);
	//6����������
	Construct_BasicBlock_Instruction(instruction, jmpdispatcher.GetString(), 3);

	//7��������
	Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), index);



	//8���޸�m_basic����������
	Modify_BasicData();
	//9����m_basic��ŵ�ָ��ŵ�������
	SaveInstructionToBuffer();
	//10�����m_basic��������
	m_basic.clear();
}


//�ж�ָ���Ƿ��ܹ�����
bool VMLoader2::IsAnalysis(char* str)
{
	XEDPARSE xed = { 0 };
	xed.x64 = false;//�Ƿ���64λ
	xed.cip = (ULONGLONG)m_vmps.vmstartaddr + m_szcalc;

	memcpy(xed.instr, str, STRUCTIONLENGTH);
	if (XEDPARSE_OK != XEDParseAssemble(&xed))
	{
		//cout << "ָ�����" << xed.error << endl;
		return false;
	}
	return true;
}


//������ָܷ�ָ�������
void VMLoader2::ColdTreatment(char* str)
{
	CString cstr = str;
	int ncalc = cstr.LeftFindChar(str, '-');
	cstr = cstr.MidCut(1, cstr.GetCount());
	JMPTABLE ljmptbl;

	if (cold_bool)
	{		
		ljmptbl.addr= m_vmps.vmstartaddr + m_szcalc;
		m_jmptbl.push_back(ljmptbl);

		cold_bool = !cold_bool;
	}
	if (ncalc == 1)
	{//�ؼ�ָ��
		ljmptbl.origin = m_origin;
		ljmptbl.addr = m_vmps.vmstartaddr + m_szcalc;
		m_jmptbl.push_back(ljmptbl);
	}
	//����ָ��
	XEParseIns(cstr.GetString());
	//����ָ��
	SaveInstructionToBuffer();
	//���m_basic����
	m_basic.clear();
}


//����handler(Ԥ���غ���)
void VMLoader2::PreLoadHandler()
{
	vector<const char*>voptionreg;
	for (int i = 0; i < OPTIONREGNUMS; i++)
	{//�ѿ�ѡ�Ĵ�������voptionreg����
		voptionreg.push_back(optionreg[i]);
	}

	//����handler������ָ��
	for (int i = 0; i < PRENUMS; i++)
	{
		//����˳��
		random_shuffle(voptionreg.begin(), voptionreg.end());

		char* reg01 = (char*)voptionreg[0];
		char* reg02 = (char*)voptionreg[1];
		char* reg03 = (char*)voptionreg[2];
		m_reg04 = reg03;
		char* reg04 = (char*)voptionreg[3];
		m_reg05 = reg04;		
		CString tempstr= Predict[i].s_Func(reg01, reg02);
		//��ӡ
		//printf("%s", tempstr.GetString());
		char linestr[32] = { 0 };
		int nlen = tempstr.GetCount();
		char* cstr = tempstr.GetString();
		for (int i = 0; i < nlen;)
		{
			int ncalc = tempstr.LeftFindChar(cstr + i, '\n');
			memcpy(linestr, cstr + i, ncalc - 1);
			if (IsAnalysis(linestr))
			{//������ָ�����Ϊ����������(JCC�����顢������һ���������)				
				Const_BasicAndRub_Block(linestr, i);
			}
			else
			{//���ָ����������͹ؼ�ָ�������				
				ColdTreatment(linestr);
			}

			memset(linestr, 0, 32);
			i += ncalc;
		}

		m_bool = true;
		cold_bool = true;

		//����Ԥ���ص�������Ϣ
		PREINFORMATION preinfor;
		memcpy(preinfor.vmfunc, Predict[i].vm_opcoed, OPECODELENGTH);
		preinfor.temp_jmptbl.assign(m_jmptbl.begin(), m_jmptbl.end());
		m_preinformation.push_back(preinfor);

		//���m_jmptbl��ת��
		m_jmptbl.clear();
	}

}

//ɨ��Ҫ������Ӳ���룬����Ϊ���
void VMLoader2::DisassembleEngine()
{
	DISASM disAsm = { 0 };
	//���ýṹ�壬��ʼ��disAsm
	disAsm.EIP = (UIntPtr)m_vmps.startaddr;
	disAsm.VirtualAddr = (UInt64)m_vmps.originaddr;
	disAsm.Archi = 0;//0��ʾ32λ��64��ʾ64λ
	disAsm.Options = 0;
	int nCount = 0;
	int nLen = 0;

	//ɨ��Ӳ���룬����Ϊ���
	while (nCount < m_vmps.length)
	{
		nLen = Disasm(&disAsm);
		//��ӡ
		//printOpcode(disAsm, nLen);

		//����ָ��,����Ϊ�м��ʾ
		MiddleRepresent(disAsm);

		nCount += nLen;
		disAsm.EIP += nLen;
		disAsm.VirtualAddr += nLen;
	}


}

//��ͨhandler���������handler����
void VMLoader2::TransformToHandler_1(int index,bool isGeneral)
{
	CString tempstr = "";
	if (isGeneral)
	{
		vector<const char*>voptionreg;
		for (int i = 0; i < OPTIONREGNUMS; i++)
		{//�ѿ�ѡ�Ĵ�������voptionreg����
			voptionreg.push_back(optionreg[i]);
		}

		//����˳��
		random_shuffle(voptionreg.begin(), voptionreg.end());

		char* reg01 = (char*)voptionreg[0];
		char* reg02 = (char*)voptionreg[1];
		char* reg03 = (char*)voptionreg[2];
		m_reg04 = reg03;
		char* reg04 = (char*)voptionreg[3];
		m_reg05 = reg04;
		tempstr = g_FunName[index].s_Func(reg01, reg02);

	}
	else
	{
		m_reg04 = m_reg05 = (char*)optionreg[3];
		vector<MIDDLESTRUCT>::iterator iter_mid = m_middle.begin();

		for (int k = 0; k < m_middle.size(); k++)
		{
			if (0 == _stricmp(g_FuncMem[index].vm_opcoed,(*iter_mid).vmfunc))
			{
				tempstr = g_FuncMem[index].s_Func(m_middle.at(k).param1, m_middle.at(k).param2);
				break;
			}
			++iter_mid;
		}
	}
	
	//��ӡ
	//printf("%s", tempstr.GetString());
	char linestr[STRUCTIONLENGTH] = { 0 };
	int nlen = tempstr.GetCount();
	char* cstr = tempstr.GetString();
	for (int i = 0; i < nlen;)
	{
		int ncalc = tempstr.LeftFindChar(cstr + i, '\n');
		memcpy(linestr, cstr + i, ncalc - 1);
		//printf("%s\n", linestr);
		if (IsAnalysis(linestr))
		{//������ָ�����Ϊ����������(JCC�����顢������һ���������)				
			Const_BasicAndRub_Block(linestr, i);
		}
		else
		{//���ָ����������͹ؼ�ָ�������				
			ColdTreatment(linestr);
		}

		memset(linestr, 0, 32);
		i += ncalc;
	}
	m_bool = true;
	cold_bool = true;

}

//���м��ʾ����handler����������ָ��
void VMLoader2::TransformToHandler()
{
	vector< MIDDLESTRUCT>::iterator iter_middle = m_middle.begin();

	for (int i = 0; i < m_middle.size(); i++)
	{
		if (0 == _stricmp((*iter_middle).vmfunc,"VCheckESP "))
		{//�����VCheckESP����
			JMPTABLE ljmptbl;
			ljmptbl.addr = m_vmps.vcheckespaddr;
			m_jmptbl.push_back(ljmptbl);
			goto continue_;
		}
		if (0 == _stricmp((*iter_middle).vmfunc, "vRetnNOT "))
		{
			JMPTABLE ljmptbl;
			ljmptbl.index = 0x88888888;
			ljmptbl.origin = (*iter_middle).originaddr;
			ljmptbl.addr = m_vmps.vretnotaddr;
			m_jmptbl.push_back(ljmptbl);
			goto continue_;
		}


		for (int t = 0; t < PRENUMS; t++)//�ж��ǲ��Ƿ���Ԥ���غ���
		{
			if (0 == _stricmp(Predict[t].vm_opcoed, (*iter_middle).vmfunc))
			{				
				//�޸ĵ�ַ
				m_preinformation[t].temp_jmptbl.at(0).origin=(*iter_middle).originaddr;

				//��temp_jmptbl׷�ӵ�m_jmptbl
				m_jmptbl.insert(m_jmptbl.end(), m_preinformation[t].temp_jmptbl.begin(), m_preinformation[t].temp_jmptbl.end());

				m_preinformation[t].temp_jmptbl.at(0).origin = 0;
				goto continue_;
			}
		}

		for (int j = 0; j < FUNNUMS; j++)
		{//��ͨhandler
			if (0 == _stricmp(g_FunName[j].vm_opcoed,(*iter_middle).vmfunc))
			{
				m_origin = (*iter_middle).originaddr;
				TransformToHandler_1(j,true);
				goto continue_;
			}
		}
		for (int k = 0; k < MEMNUMS; k++)
		{//����handler
			if (0 == _stricmp(g_FuncMem[k].vm_opcoed, (*iter_middle).vmfunc))
			{
				m_origin = (*iter_middle).originaddr;
				TransformToHandler_1(k, false);
				memcpy(m_middle.at(i).vmfunc, " ", sizeof(" "));
				break;
			}
		}
	continue_:
		++iter_middle;
	}	
}

//�޸����ݱ������
void VMLoader2::ModifyDataTbl()
{
	vector<DATATABLE>::iterator iter_data = m_datatable.begin();
	vector<JMPTABLE>::iterator iter_jmp = m_jmptbl.begin();
	DWORD jccaddr = 0;
	DWORD jmpaddr = 0;

	//�޸���������(����JCC��ת�ˣ���ô���ݱ��е�����ҲҪ��ת)
	for (int i = 0; i < m_datatable.size(); i++)
	{
		if (0 != (*iter_data).originaddr)
		{
			jccaddr = i;
			jmpaddr = (*iter_data).data;
			iter_data = m_datatable.begin();
			for (int j = 0; j < m_datatable.size(); j++)
			{
				if ((*iter_data).recodeOaddr == jmpaddr)
				{
					m_datatable.at(jccaddr + 2).data = (j - jccaddr - 3) * 4;
					iter_data = m_datatable.begin()+i;
					break;
				}
				++iter_data;
			}
		}
		++iter_data;
	}

	iter_data = m_datatable.begin();		
	for (int i = 0; i < m_datatable.size(); i++)
	{//�޸����ݱ���JCCҪ�õ�������
		if (0 != (*iter_data).originaddr)
		{
			for (int j = 0; j < m_jmptbl.size(); j++)
			{
				if (-1 == (*iter_jmp).index && (*iter_jmp).origin == (*iter_data).originaddr)
				{
					jccaddr = j;
					iter_jmp= m_jmptbl.begin();
					break;
				}
				++iter_jmp;
			}

			for (int j = 0; j < m_jmptbl.size(); j++)
			{
				if ((*iter_jmp).origin == (*iter_data).data)
				{
					jmpaddr = j;
					iter_jmp = m_jmptbl.begin();
					break;
				}
				++iter_jmp;
			}
			m_datatable.at(i).data = (jmpaddr - jccaddr) * 4;
		}
		++iter_data;
	}
}

//�ָ�ebp��esi��vm������ݵĵ�ַ�ĺ���
void VMLoader2::ResumeEsiAndEbp()
{
	vector<PREINFORMATION>::iterator iter_preinfor = m_preinformation.begin();
	for (int i = 0; i < m_preinformation.size(); i++)
	{
		if (0 == _stricmp("vResumeStart_1 ", (*iter_preinfor).vmfunc))
		{					
			g_vmstruct.resume_esiandebpaddr = (*((*iter_preinfor).temp_jmptbl.begin())).addr;
		}
		//VMStartVM
		if (0 == _stricmp("VMStartVM ", (*iter_preinfor).vmfunc))
		{
			g_vmstruct.vmstartaddr= (*((*iter_preinfor).temp_jmptbl.begin())).addr;
		}
		++iter_preinfor;
	}
}

//������
void VMLoader2::MainVMP()
{	
	//1������handler(Ԥ���غ���)
	PreLoadHandler();
	ResumeEsiAndEbp();

	//2����������ָ�����vm������ʼǰ
	Out_Of_Order_RubInstruction();

	//3����ȡ��Ҫ������ָ��
	DisassembleEngine();

	//4�����м��ʾ����handler����������ָ��
	TransformToHandler();

	//5���޸����ݱ������
	ModifyDataTbl();

}


//��������ָ��
CString VMLoader2::ProduceRubbishOpecode(char* reg04, char* reg05)
{
	VMTable vmtbl = vmtable32[SrandNum(0, m_vmlength)];
	CString str = vmtbl.strInstruction;
	//1��Ŀ�Ĳ���
	switch (vmtbl.optype[0])
	{
	case NONETYPE://û�в�����
		break;
	case IMMTYPE://������
	{
		if (8 == vmtbl.bitnum[0])
		{
			str = str + " " + (DWORD)4;
		}
		else if (16 == vmtbl.bitnum[0])
		{
			str = str + " " + (DWORD)4;
		}
		else
		{
			str = str + " " + (DWORD)8;
		}
		
	}
		break;
	case REGTYPE://�Ĵ���
	{

		if (8 == vmtbl.bitnum[0])
		{
			for (int i = 0; i < 14; i++)
			{
				if (_stricmp(reg04, regname_[2][i]) == 0)
				{
					str = str + " " + regname_[0][i];
					break;
				}
			}			
		}
		else if (16 == vmtbl.bitnum[0])
		{
			for (int i = 0; i < 14; i++)
			{
				if (_stricmp(reg05, regname_[2][i]) == 0)
				{
					str = str + " " + regname_[1][i];
					break;
				}
			}
		}
		else
		{
			str = str + " " + reg05;
		}
	}
		break;
	case MEMTYPE://�ڴ�
	{//���ѡ��vmp1����û���õ����ڴ�
		DWORD dnum = SrandNum(m_vmps.vmp1_startaddr+ EBPSTATEADDR+0x300, m_vmps.vmp1_startaddr+ CODETABLEADDR-0x100);
		CString memstr = dnum;
		if (8 == vmtbl.bitnum[0])
		{
			str = str + " byte ptr[" + memstr.GetString() + "]";
		}
		else if (16 == vmtbl.bitnum[0])
		{
			str = str + " word ptr[" + memstr.GetString() + "]";
		}
		else
		{
			str = str + " dword ptr[" + memstr.GetString() + "]";
		}
	}
		break;
	default:
		break;
	}

	//2��Դ������
	switch (vmtbl.optype[1])
	{
	case NONETYPE://û�в�����
		break;
	case IMMTYPE://������
	{
		if (8 == vmtbl.bitnum[1])
		{
			str = str + "," + 4;
		}
		else if (16 == vmtbl.bitnum[1])
		{
			str = str + "," + 8;
		}
		else
		{
			str = str + "," + 4;
		}
	}
		break;
	case REGTYPE://�Ĵ���(������2�ļĴ���������8���Ĵ���������ѡ��)
	{
		if (0 == _stricmp(vmtbl.strInstruction,"xchg"))
		{//�����xchg���Ĵ�����ѡ��reg04������reg05
			if (8 == vmtbl.bitnum[1])
			{
				for (int i = 0; i < 14; i++)
				{
					if (_stricmp(reg05, regname_[2][i]) == 0)
					{
						str = str + "," + regname_[0][i];
						break;
					}
				}

			}
			else if (16 == vmtbl.bitnum[1])
			{
				for (int i = 0; i < 14; i++)
				{
					if (_stricmp(reg04, regname_[2][i]) == 0)
					{
						str = str + "," + regname_[1][i];
						break;
					}
				}
			}
			else
			{
				str = str + "," + reg04;
			}
			break;
		}
		if (8 == vmtbl.bitnum[1])
		{
			str = str + "," + regname_[0][SrandNum(0, 8)];
		}
		else if (16 == vmtbl.bitnum[1])
		{
			str = str + "," + regname_[1][SrandNum(0, 8)];
		}
		else
		{
			str = str + "," + regname_[2][SrandNum(0, 8)];
		}
	}
		break;
	case MEMTYPE://�ڴ�
	{//���ѡ��vmp1���ڵĵ�ַ������ѡesp�Ĵ���������ѡesi�Ĵ���
		DWORD dnum = SrandNum(m_vmps.vmp1_startaddr, m_vmps.vmstartaddr);
		CString memstr = dnum;
		const char* memchr[5] = { memstr.GetString(),"esp","esp+4","esi","ebp" };
		const char* srandstr = memchr[SrandNum(0, 5)];

		if (8 == vmtbl.bitnum[1])
		{
			str = str + ",byte ptr[" + srandstr + "]";
		}
		else if (16 == vmtbl.bitnum[1])
		{
			str = str + ",word ptr[" + srandstr + "]";
		}
		else
		{
			str = str + ",dword ptr[" + srandstr + "]";
		}
	}
		break;
	default:
		break;
	}

	return str;
}

//�����ڴ���������ڴ淭��Ϊ�м��ʾ
void VMLoader2::MemoryMiddle(DISASM disAsm,MEMORYTYPE memtype)
{
	/*
	* vPushImm4
	* vPushReg
	* vMUL_MEM //�ڴ����ר�ó˷�handler
	* vPushReg
	* vAdd
	* vPushImm4
	* vAdd
	* vWriteMemDs4/2/1
	*/
	MIDDLESTRUCT midstr;
	DATATABLE datatbl;
	midstr.originaddr = disAsm.VirtualAddr;
	if (memtype.Scale!=0 && memtype.IndexRegister!=0)
	{
		char vpushreg4[] = "vPushImm4 ";
		//printf("%s\n", vpushreg4);
		memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
		m_middle.push_back(midstr);//������ѹ���м��ʾ	


		datatbl.data = memtype.Scale;
		datatbl.recodeOaddr = disAsm.VirtualAddr;
		m_datatable.push_back(datatbl);//����ѹ�����ݱ�

		for (int i = 0; i < REGNUMS; i++)
		{
			if (memtype.IndexRegister == tempreg[i].index)
			{
				char vpushreg4[] = "vPushReg4 ";
				//printf("%s\n", vpushreg4);
				memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
				m_middle.push_back(midstr);//������ѹ���м��ʾ	

				DATATABLE datatbl;

				datatbl.data = i;
				datatbl.recodeOaddr = disAsm.VirtualAddr;
				m_datatable.push_back(datatbl);//����ѹ�����ݱ�

				break;
			}
		}

		char vmul_mem[] = "vMUL_MEM ";
		//printf("%s\n", vmul_mem);
		memcpy(midstr.vmfunc, vmul_mem, sizeof(vmul_mem));
		m_middle.push_back(midstr);//������ѹ���м��ʾ	
	}
	
	int i = 0;
	for (; i < REGNUMS; i++)
	{
		if (memtype.BaseRegister == tempreg[i].index)
		{
			char vpushreg4[] = "vPushReg4 ";
			//printf("%s\n", vpushreg4);
			memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
			m_middle.push_back(midstr);//������ѹ���м��ʾ	

			DATATABLE datatbl;
			datatbl.data = i;
			datatbl.recodeOaddr = disAsm.VirtualAddr;
			m_datatable.push_back(datatbl);//����ѹ�����ݱ�
			break;
		}
	}

	if ((memtype.IndexRegister != 0) && (i != REGNUMS))
	{
		char vadd[] = "vAdd4 ";
		//printf("%s\n", vadd);
		memcpy(midstr.vmfunc, vadd, sizeof(vadd));
		m_middle.push_back(midstr);//������ѹ���м��ʾ
	}
	

	if (0 != memtype.Displacement)
	{
		char vpushreg4[] = "vPushImm4 ";
		//printf("%s\n", vpushreg4);
		memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
		m_middle.push_back(midstr);//������ѹ���м��ʾ	

		datatbl.data = memtype.Displacement;
		m_datatable.push_back(datatbl);//����ѹ�����ݱ�
	}
	

	if ((0 != memtype.Displacement) && (i != REGNUMS))
	{
		char vadd[] = "vAdd4 ";
		//printf("%s\n", vadd);
		memcpy(midstr.vmfunc, vadd, sizeof(vadd));
		m_middle.push_back(midstr);//������ѹ���м��ʾ
	}
}

//����Ҫ������ָ�����Ϊ�м��ʾ
void VMLoader2::MiddleRepresent(DISASM disAsm)
{
	MIDDLESTRUCT midstr;
	midstr.originaddr = disAsm.VirtualAddr;
	bool IsSimulation = true;

	//vm�Ļ���׼��
	if (start_bool)
	{		

		char vresumestart1[] = "VMStartVM ";
		printf("%s\n", vresumestart1);
		memcpy(midstr.vmfunc, vresumestart1, sizeof(vresumestart1));
		m_middle.push_back(midstr);//ѹ��VMStartVM

		char vresumestart[] = "VMStartVM_2 ";
		printf("%s\n", vresumestart);
		memcpy(midstr.vmfunc, vresumestart, sizeof(vresumestart));
		m_middle.push_back(midstr);//ѹ��VMStartVM_2
		start_bool = false;
	}
	

/*----------------------------------------------------------------------------------*/
/*	0���ж��Ƿ��ǲ���ģ��ָ��		      				     */
/*----------------------------------------------------------------------------------*/
	for (int i = 0; i < FUNNUMS; i++)
	{
		if (_stricmp(disAsm.Instruction.Mnemonic, g_FunName[i].s_opecode) == 0)
		{			
			IsSimulation = false;
			break;
		}
	}
	if (
		0 == _stricmp(disAsm.Instruction.Mnemonic, "push ") && ((disAsm.Argument2.ArgType & 0xF0000000) == MEMORY_TYPE)||
		0 == _stricmp(disAsm.Instruction.Mnemonic, "pop ") && ((disAsm.Argument1.ArgType & 0xF0000000) == MEMORY_TYPE)
		)
	{
		IsSimulation = true;
	}
	//0��1 ����ǲ���ģ��ָ��,����ú�ֱ�ӷ���
	if (IsSimulation)
	{
		char retnotaddr[] = "vRetnNOT_ ";
		//printf("%s\n", retnotaddr);
		memcpy(midstr.vmfunc, retnotaddr, sizeof(retnotaddr));
		m_middle.push_back(midstr);//��ѹ��vRetnNOT

		char notsimulate[] = "vNotSimulate ";
		char vresumestartaddr[OPECODELENGTH];
		sprintf(vresumestartaddr, "%X", m_vmps.vresumestartaddr);

		//printf("%s\n", notsimulate);
		memcpy(midstr.vmfunc, notsimulate, sizeof(notsimulate));
		memcpy(midstr.param1, disAsm.CompleteInstr, STRUCTIONLENGTH);
		memcpy(midstr.param2, vresumestartaddr, OPECODELENGTH);
		m_middle.push_back(midstr);//��ѹ�벻��ģ��ָ���handler
		
		char vresumestart[] = "vResumeStart_2 ";
		//printf("%s\n", vresumestart);
		memcpy(midstr.vmfunc, vresumestart, sizeof(vresumestart));
		m_middle.push_back(midstr);//ѹ��vResumeStart_2

		return;
	}

/*----------------------------------------------------------------------------------*/
/*		1���Ƿ��в�����3		      	
		��ʱû�д������3����Ϊһ�����ָ������������£��������ַָ�һ��ַָ�����ַָ�����������ַָ�
			    
/*----------------------------------------------------------------------------------*/
	if (NO_ARGUMENT != disAsm.Argument3.ArgType)
	{
		switch (disAsm.Argument3.ArgType & 0xF0000000)
		{
		case REGISTER_TYPE: //�Ĵ���
			break;
		case MEMORY_TYPE: //�ڴ�
			break;
		case CONSTANT_TYPE://����
			break;
		default:
			break;
		}
	}

/*----------------------------------------------------------------------------------*/
/*	2���Ƿ��в�����2		      				     */
/*----------------------------------------------------------------------------------*/
	if (NO_ARGUMENT != disAsm.Argument2.ArgType)
	{
		switch (disAsm.Argument2.ArgType & 0xF0000000)
		{
		case REGISTER_TYPE: //�Ĵ���
		{
			for (int i = 0; i < REGNUMS; i++)
			{
				if (tempreg[i].index == (disAsm.Argument2.ArgType & 0xFFFF))
				{
					if (0x20 == disAsm.Argument2.ArgSize)
					{//32λ
						char vpushreg4[] = "vPushReg4 ";
						//printf("%s\n", vpushreg4);
						memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
						m_middle.push_back(midstr);//������ѹ���м��ʾ							
					}
					else if (0x10 == disAsm.Argument2.ArgSize)
					{//16λ
						char vpushreg4[] = "vPushReg2 ";
						//printf("%s\n", vpushreg4);
						memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
						m_middle.push_back(midstr);//������ѹ���м��ʾ	
					}
					else
					{//8λ(Ҫ�жϸ�λ���ǵ�λ)
						for (int i = 0; i < 14; i++)
						{
							if (_stricmp(disAsm.Argument2.ArgMnemonic, regname_[0][i]) == 0)
							{
								if (i<=3)//С�ڵ���3�ǵ�λ
								{
									char vpushreg4[] = "vPushReg1_low ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//������ѹ���м��ʾ	
								}
								else
								{
									char vpushreg4[] = "vPushReg1_above ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//������ѹ���м��ʾ	
								}
								break;
							}
						}
					}
					DATATABLE datatbl;
					datatbl.data = i;
					datatbl.recodeOaddr = disAsm.VirtualAddr;
					m_datatable.push_back(datatbl);//����ѹ�����ݱ�
					break;
				}
			}
			
		}
			break;
		case MEMORY_TYPE: //�ڴ�
		{
			if (0 == _stricmp("pop ", disAsm.Instruction.Mnemonic)) break;
			if (0 == _stricmp("ret ", disAsm.Instruction.Mnemonic)) break;

			MemoryMiddle(disAsm, disAsm.Argument2.Memory);
			
			if (0x20 == disAsm.Argument2.ArgSize)
			{//32λ
				char vReadMem[] = "vReadMemDs4 ";
				//printf("%s\n", vReadMem);
				memcpy(midstr.vmfunc, vReadMem, sizeof(vReadMem));
			}
			else if (0x10 == disAsm.Argument2.ArgSize)
			{//16λ
				char vReadMem[] = "vReadMemDs2 ";
				//printf("%s\n", vReadMem);
				memcpy(midstr.vmfunc, vReadMem, sizeof(vReadMem));
			}
			else
			{//8λ
				char vReadMem[] = "vReadMemDs1 ";
				//printf("%s\n", vReadMem);
				memcpy(midstr.vmfunc, vReadMem, sizeof(vReadMem));
			}

			memcpy(midstr.param1, disAsm.Instruction.Mnemonic, 16);//����һ
			memcpy(midstr.param2, disAsm.Argument2.ArgMnemonic, 32);//������
			m_middle.push_back(midstr);//������ѹ���м��ʾ
		}
			break;
		case CONSTANT_TYPE://����
		{
			DWORD constnums = 0;
			sscanf(disAsm.Argument2.ArgMnemonic, "%X", &constnums);

			char vpushimm4[] = "vPushImm4 ";
			//printf("%s\n", vpushimm4);

			memcpy(midstr.vmfunc, vpushimm4, sizeof(vpushimm4));
			m_middle.push_back(midstr);//������ѹ���м��ʾ

			DATATABLE datatbl;
			datatbl.data = constnums;
			datatbl.recodeOaddr = disAsm.VirtualAddr;
			m_datatable.push_back(datatbl);//����ѹ�����ݱ�

		}
			break;
		default:
			break;
		}
	}

/*----------------------------------------------------------------------------------*/
/*	3���Ƿ��в�����1		      				     */
/*----------------------------------------------------------------------------------*/
	if (NO_ARGUMENT != disAsm.Argument1.ArgType)
	{
		switch (disAsm.Argument1.ArgType & 0xF0000000)
		{
		case REGISTER_TYPE: //�Ĵ���
		{
			for (int i = 0; i < REGNUMS; i++)
			{
				if (tempreg[i].index == (disAsm.Argument1.ArgType & 0xFFFF))
				{
					if (0x20 == disAsm.Argument1.ArgSize)
					{//32λ
						char vpushreg4[] = "vPushReg4 ";
						//printf("%s\n", vpushreg4);
						memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
						m_middle.push_back(midstr);//������ѹ���м��ʾ						
					}
					else if (0x10 == disAsm.Argument1.ArgSize)
					{//16λ
						char vpushreg4[] = "vPushReg2 ";
						//printf("%s\n", vpushreg4);
						memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
						m_middle.push_back(midstr);//������ѹ���м��ʾ
					}
					else
					{//8λ(Ҫ�жϸ�λ���ǵ�λ)
						for (int i = 0; i < 14; i++)
						{
							if (_stricmp(disAsm.Argument1.ArgMnemonic, regname_[0][i]) == 0)
							{
								if (i <= 3)//С�ڵ���3�ǵ�λ
								{
									char vpushreg4[] = "vPushReg1_low ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//������ѹ���м��ʾ	
								}
								else
								{
									char vpushreg4[] = "vPushReg1_above ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//������ѹ���м��ʾ	
								}
								break;
							}
						}
					}
					DATATABLE datatbl;
					datatbl.data = i;
					datatbl.recodeOaddr = disAsm.VirtualAddr;
					m_datatable.push_back(datatbl);//����ѹ�����ݱ�
					break;
				}
			}
		}
			break;
		case MEMORY_TYPE: //�ڴ�(������1������ڴ棬�Ҳ�����2��Ϊ�գ�����ú�ֱ���˳�)
		{
			if (0 == _stricmp("push ",disAsm.Instruction.Mnemonic)) break;
	
			MemoryMiddle(disAsm, disAsm.Argument1.Memory);			

			if (NO_ARGUMENT == disAsm.Argument2.ArgType) break;//�ⲽ�ǶԴ�������������

			if (0x20 == disAsm.Argument1.ArgSize)
			{//32λ
				char vWriteMem[] = "vWriteMemDs4 ";
				//printf("%s\n", vWriteMem);
				memcpy(midstr.vmfunc, vWriteMem, sizeof(vWriteMem));				
			}
			else if (0x10 == disAsm.Argument1.ArgSize)
			{//16λ
				char vWriteMem[] = "vWriteMemDs2 ";
				//printf("%s\n", vWriteMem);
				memcpy(midstr.vmfunc, vWriteMem, sizeof(vWriteMem));
			}
			else
			{//8λ
				char vWriteMem[] = "vWriteMemDs1 ";
				//printf("%s\n", vWriteMem);
				memcpy(midstr.vmfunc, vWriteMem, sizeof(vWriteMem));
			}
			
			memcpy(midstr.param1, disAsm.Instruction.Mnemonic, 16);//����һ
			memcpy(midstr.param2, disAsm.Argument1.ArgMnemonic, 32);//������
			m_middle.push_back(midstr);//������ѹ���м��ʾ

			return;
		}
			break;
		case CONSTANT_TYPE://����
		{
			DWORD constnums = 0;
			sscanf(disAsm.Argument1.ArgMnemonic, "%X", &constnums);

			char vpushimm4[] = "vPushImm4 ";
			//printf("%s\n", vpushimm4);

			memcpy(midstr.vmfunc, vpushimm4, sizeof(vpushimm4));
			m_middle.push_back(midstr);//������ѹ���м��ʾ

			DATATABLE datatbl;
			datatbl.data = constnums;
			datatbl.recodeOaddr = disAsm.VirtualAddr;
			m_datatable.push_back(datatbl);//����ѹ�����ݱ�
		}
			break;
		default:
			break;
		}
	}

/*----------------------------------------------------------------------------------*/
/*	4��������ͨhandler		      				     */
/*----------------------------------------------------------------------------------*/
	//4��1 �����ret��ֱ�ӷ���
	if (0 == _stricmp("ret ", disAsm.Instruction.Mnemonic))
	{
		if (disAsm.Argument1.ArgType == 0x10000000)
		{
			char vpushreg[] = "vPushImm4 ";
			//printf("%s\n", vpushreg);
			memcpy(midstr.vmfunc, vpushreg, sizeof(vpushreg));
			m_middle.push_back(midstr);//ѹ��vPushReg4

			DATATABLE datatbl;
			datatbl.data = 0;
			datatbl.recodeOaddr = disAsm.VirtualAddr;
			m_datatable.push_back(datatbl);//����ѹ�����ݱ�
		}
		char callmem[] = "vRETN ";
		//printf("%s\n", callmem);
		memcpy(midstr.vmfunc, callmem, sizeof(callmem));
		m_middle.push_back(midstr);//ѹ��vRETN
		return;
	}


	//4��2 ���������2���ڴ棬m_middle�����������Ԫ�ػ���
	if ((disAsm.Argument2.ArgType & 0xF0000000) == MEMORY_TYPE)
	{
		if (0 != _stricmp("pop ", disAsm.Instruction.Mnemonic))
		{
			swap(m_middle[m_middle.size()-1], m_middle[m_middle.size() - 2]);
			
		}
		goto ttttt__;
	}

	//4��3 ����ǵ��ú���������ú�ֱ�ӷ���
	if (
		0 == _stricmp("call ", disAsm.Instruction.Mnemonic) ||
		(0 == _stricmp(disAsm.Instruction.Mnemonic, "jmp ") && ((disAsm.Argument1.ArgType & 0xF0000000) == MEMORY_TYPE)) ||
		(0 == _stricmp(disAsm.Instruction.Mnemonic, "jmp ") && ((disAsm.Argument1.ArgType & 0xF0000000) == REGISTER_TYPE))
		)//�������ǵ�jmp����call�Ĺ��ܣ����jmp�ں�������ת���������������޷����������޸����ݱ��ʱ������
	{	
		//�����jmp��Ҫ������ջ
		if (0 == _stricmp(disAsm.Instruction.Mnemonic, "jmp "))
		{
			char addesp[] = "vAddEsp ";
			//printf("%s\n", addesp);
			memcpy(midstr.vmfunc, addesp, sizeof(addesp));
			m_middle.push_back(midstr);//��ѹ��vCallMem
		}

		
		if ((disAsm.Argument1.ArgType & 0xF0000000) == MEMORY_TYPE)
		{//���������1���ڴ�			
			char callmem[] = "vCallMem ";
			//printf("%s\n", callmem);
			memcpy(midstr.vmfunc, callmem, sizeof(callmem));
			m_middle.push_back(midstr);//��ѹ��vCallMem
		}
		else if ((disAsm.Argument1.ArgType & 0xF0000000) == REGISTER_TYPE)
		{//���������1�ǼĴ���
			char callreg[] = "vCallReg ";
			//printf("%s\n", callreg);
			memcpy(midstr.vmfunc, callreg, sizeof(callreg));
			m_middle.push_back(midstr);//��ѹ��vCallMem
		}
		else
		{
			//ɾ��vPushImm4
			m_middle.pop_back();
		}
		char retnotaddr[] = "vRetnNOT_ ";
		//printf("%s\n", retnotaddr);
		memcpy(midstr.vmfunc, retnotaddr, sizeof(retnotaddr));
		m_middle.push_back(midstr);//ѹ��vRetnNOT

		char vcall[] = "vCALL ";
		//printf("%s\n", vcall);
		memcpy(midstr.vmfunc, vcall, sizeof(vcall));
		m_middle.push_back(midstr);

		char vresumestart[] = "vResumeStart_2 ";
		//printf("%s\n", vresumestart);
		memcpy(midstr.vmfunc, vresumestart, sizeof(vresumestart));
		m_middle.push_back(midstr);//ѹ��vResumeStart_2
		return;
	}

	//4��4 ������ͨhandler
	for (int i = 0; i < FUNNUMS; i++)
	{
		if (0 == _stricmp(g_FunName[i].s_opecode,disAsm.Instruction.Mnemonic))
		{
			//printf("%s\n", g_FunName[i].vm_opcoed);
			memcpy(midstr.vmfunc, g_FunName[i].vm_opcoed,OPECODELENGTH);
			m_middle.push_back(midstr);//������ѹ���м��ʾ
			break;
		}

	}


/*----------------------------------------------------------------------------------*/
/*	5���ж��Ƿ��и���handler		      				     */
/*----------------------------------------------------------------------------------*/
	if (
		0x10000000 != disAsm.Argument1.ArgType ||
		0x10000000 != disAsm.Argument2.ArgType ||
		0x10000000 != disAsm.Argument3.ArgType
		)
	{
		ttttt__:
		//5.1 �ж�����ָ���Ƿ�ı������1��ֵ������cmp��test�������Ͳ���ı������1��ֵ��ֱ�ӷ���	
		for (int i = 0; i < NOTREGNUMS; i++)
		{
			if (_stricmp(cmp_opecode[i], disAsm.Instruction.Mnemonic) == 0)
			{
				return;
			}
		}
		
		//5.2 �ж��Ƿ���JCC����JMPָ�������ú�ֱ�ӷ���
		for (int i = 0; i < JCCNUMS; i++)
		{//�ж��Ƿ���JCCָ��
			if (0 == _stricmp(disAsm.Instruction.Mnemonic, JCCstr[i]))
			{
				//�ı����ݱ������
				m_datatable.at(m_datatable.size() - 1).originaddr = disAsm.VirtualAddr;
				
				//��ѹ��һ�������ݵ����ݱ�
				DATATABLE datatbl;
				datatbl.data = 0;
				m_datatable.push_back(datatbl);//����ѹ�����ݱ�

				//��ѹ��һ�������ݵ����ݱ�
				datatbl.data = 0;
				m_datatable.push_back(datatbl);//����ѹ�����ݱ�

				//JCCָ��ǰ��������vPushImm4
				char vpushimm4[] = "vPushImm4 ";
				//printf("%s\n", vpushimm4);
				memcpy(midstr.vmfunc, vpushimm4, sizeof(vpushimm4));
				m_middle.insert(m_middle.end()-1, midstr);//������ѹ���м��ʾ
				m_middle.insert(m_middle.end() - 1, midstr);//������ѹ���м��ʾ
				return;
			}
		}

		//5.2 ������ͨ����handler
		switch (disAsm.Argument1.ArgType & 0xF0000000)
		{
		case REGISTER_TYPE://�Ĵ���
		{
			for (int i = 0; i < REGNUMS; i++)
			{
				if (tempreg[i].index == (disAsm.Argument1.ArgType & 0xFFFF))
				{
					if (0x20 == disAsm.Argument1.ArgSize)
					{//32λ
						char vpopreg4[] = "vPopReg4 ";
						//printf("%s\n", vpopreg4);
						memcpy(midstr.vmfunc, vpopreg4, sizeof(vpopreg4));
						m_middle.push_back(midstr);//������ѹ���м��ʾ						
					}
					else if (0x10 == disAsm.Argument1.ArgSize)
					{//16λ
						char vpopreg4[] = "vPopReg2 ";
						//printf("%s\n", vpopreg4);
						memcpy(midstr.vmfunc, vpopreg4, sizeof(vpopreg4));
						m_middle.push_back(midstr);//������ѹ���м��ʾ	
					}
					else
					{//8λ(Ҫ�жϸ�λ���ǵ�λ)
						for (int i = 0; i < 14; i++)
						{
							if (_stricmp(disAsm.Argument1.ArgMnemonic, regname_[0][i]) == 0)
							{
								if (i <= 3)//С�ڵ���3�ǵ�λ
								{
									char vpushreg4[] = "vPopReg1_low ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//������ѹ���м��ʾ	
								}
								else
								{
									char vpushreg4[] = "vPopReg1_above ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//������ѹ���м��ʾ	
								}
								break;
							}
						}
					}
					DATATABLE datatbl;
					datatbl.data = i;
					m_datatable.push_back(datatbl);//����ѹ�����ݱ�
				}
			}

			//���������1��esp�����ı�ջ��С������ָ��sub esp,0x100,
			//��Ҫ����vCheckESP()���������VMContext�Ƿ񱻸���
			if (16 == (disAsm.Argument1.ArgType & 0xFFFF))
			{//16��BegEngine���������Լ������esp�Ĵ���
				char vcheckesp[] = "VCheckESP ";
				//printf("%s\n", vcheckesp);
				memcpy(midstr.vmfunc, vcheckesp, sizeof(vcheckesp));
				m_middle.push_back(midstr);//������ѹ���м��ʾ
			}
			//�����������pop,��vPOP��ӵ�m_middle����,Ȼ�󷵻�
			if (0 == _stricmp("pop ",disAsm.Instruction.Mnemonic))
			{
				if (0x20 == disAsm.Argument1.ArgSize)
				{
					char vpushreg4[] = "vPOP4 ";
					//printf("%s\n", vpushreg4);
					memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
					m_middle.push_back(midstr);//������ѹ���м��ʾ	
				}
				else
				{
					char vpushreg4[] = "vPOP2 ";
					//printf("%s\n", vpushreg4);
					memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
					m_middle.push_back(midstr);//������ѹ���м��ʾ	
				}
				return;
			}
		}
			break;

		case MEMORY_TYPE://�ڴ�
		{
			//
		}
		break;
		case CONSTANT_TYPE:
		{
			//
		}
		break;
		}
		

	}
}

