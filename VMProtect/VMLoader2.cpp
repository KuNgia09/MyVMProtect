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

//预加载一些handler，减少工作量
#define PRENUMS 8
FUNCNAME Predict[PRENUMS] = {
	{vPopReg4,"pop ","vPopReg4 "},{vPushReg4,"push ","vPushReg4 "},{vPushImm4,"push ","vPushImm4 "},
	{vRetnNOT_," ","vRetnNOT_ "},{vResumeStart_2," ","vResumeStart_2 "},{vResumeStart_1," ","vResumeStart_1 "},
	{VMStartVM,"","VMStartVM "},{VMStartVM_2,"","VMStartVM_2 "}
};

//这个是为内存handler和不可模拟指令,以及一些特殊指令设计的
#define MEMNUMS 13
FUNCNAME g_FuncMem[MEMNUMS] = {
	{ vWriteMemDs4,"  ","vWriteMemDs4 "},{vWriteMemDs2," ","vWriteMemDs2 "},{vWriteMemDs1," ","vWriteMemDs1 "},
	{vReadMemDs4," ","vReadMemDs4 "},{vReadMemDs2," ","vReadMemDs2 "},{vReadMemDs1," ","vReadMemDs1 "},
	{vNotSimulate," ","vNotSimulate "},{vCALL,"call ","vCALL "},{vMUL_MEM," ","vMUL_MEM "},{vCallMem," ","vCallMem "},
	{vPopWirteMem4," ","vPopWirteMem4 "},{vPOP2," ","vPOP2 "},{vCallReg," ","vCallReg "}
};

//不改变通用寄存器的指令
#define NOTREGNUMS 3
const char* cmp_opecode[NOTREGNUMS] = { "CMP ","TEST ","PUSH " };

//寄存器结构体
struct regstr
{
	short index;
	char reg[4];
};
#define REGNUMS 8
regstr tempreg[REGNUMS] = {
	{16,"esp"},{1,"eax"},{2,"ecx"},{4,"edx"},{8,"ebx"},{32,"ebp"},{64,"esi"},{128,"edi"}
};

//JCC指令字符串
#define JCCNUMS 33
const char* JCCstr[JCCNUMS] = {
	"JNBE ","JE ","JZ ","JNE ","JNZ ","JS ","JNS ","JP ","JPE ","JNP ","JCXZ ","JECXZ ",
	"JPO ","JO ","JNO ","JC ","JB ","JNAE ","JNC ","JNB ","JAE ","JBE ","JNA ",
	"JA ","JL ","JNGE ","JNL ","JGE ","JLE ","JNG ","JNLE ","JG ","JMP "
};

//寄存器名称
const char* regname_[3][14] =
{
	{ "AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH", "CS","DS","ES","FS","GS","SS"  },
	{ "AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI", "CS","DS","ES","FS","GS","SS" },
	{ "EAX","ECX","EDX","EBX","ESP","EBP","ESI","EDI","CS","DS","ES","FS","GS","SS" }
};

//可选寄存器
#define OPTIONREGNUMS 4
const char* optionreg[OPTIONREGNUMS] = { "eax","ecx","edx","ebx"};

//初始化
void VMLoader2::InitialVM(VMPStruct vmps)
{
	//随机数种子
	srand(time(0));

	//vmtable32结构体数组的长度
	m_vmlength = VMLength();

	//把vmps复制到m_vmps
	m_vmps = vmps;
}

//求vmtable32结构体数组的长度
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

//解析指令并把指令存放到m_basic链表
void VMLoader2::XEParseIns(char* str)
{
	XEDPARSE xed = { 0 };
	xed.x64 = false;//是否是64位
	xed.cip = (ULONGLONG)m_vmps.vmstartaddr + m_szcalc;

	//printf("%X : %s\n", (int)xed.cip, str);

	memcpy(xed.instr, str, strlen(str));
	if (XEDPARSE_OK != XEDParseAssemble(&xed))
	{
		cout << "指令错误：" << xed.error << endl;
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

//垃圾块
void VMLoader2::Construct_Rub_Instruction(char* jmpdispatcher, int index, int nmatch)
{
	CString rubstr = "";
	JMPTABLE l_jmptbl;
	if (m_bool)
	{//记录普通handler或者辅助handler的索引
		temp_basic.index = index;
		l_jmptbl.origin = m_origin;
		m_bool = !m_bool;
	}
	DWORD tempsnum = m_vmps.vmstartaddr + m_szcalc;
	temp_basic.addr = tempsnum;

	l_jmptbl.index = temp_basic.index;
	l_jmptbl.addr = temp_basic.addr;
	m_jmptbl.push_back(l_jmptbl);//把地址记录到跳转表里
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

	//添加跳转地址
	XEParseIns(jmpdispatcher);
}

//设置垃圾块的数目
void VMLoader2::Construct_Rub_Instruction_Num(char* jmpdispatcher, int index)
{
	int srandnum = SrandNum(5, 8);
	for (int i = 0; i < srandnum; i++)
	{
		Construct_Rub_Instruction(jmpdispatcher, index);
	}
}

//乱序垃圾指令，用于vm环境开始前
void VMLoader2::Out_Of_Order_RubInstruction()
{
	CString jmpdispatcher = "jmp ";
	jmpdispatcher = jmpdispatcher + m_vmps.dispatcheraddr;//调度器地址

	//添加垃圾指令
	for (int i = 0; i < 12; i++)
	{
		Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), 0);
	}

	//把m_basic存放的指令放到缓存里
	SaveInstructionToBuffer();


	//把垃圾指令的信息保存到g_outoforder里，传递给Stub.dll
	g_outoforder.push_back(m_vmps.jmptbladdr);
	g_outoforder.push_back(m_jmptbl.size());

	//清空m_basic链表数据
	m_basic.clear();

}
//JCC指令块
void VMLoader2::Construct_Jcc_Instruction(char* jccstr, int index)
{
	
	Construct_Rub_Instruction(jccstr, index,1);

}

//基本块二和基本块三
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

//修复m_basic链表里的数据
void VMLoader2::Modify_BasicData()
{
	vector<BASICBLOCKADDR>::iterator iter_basic = m_basic.begin();
	vector<JMPTABLE>::iterator iter_jmp = m_jmptbl.begin();
	CString cstr = " ";

	//修复JCC跳转指令
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
			int srd = SrandNum(1, 4);//随机数
			iter_jmp -= 2;
			cstr = "jge ";
			cstr = cstr + (*iter_jmp).addr;
			memcpy(m_basic.at(tempnum_[0]).instruction, cstr.GetString(), STRUCTIONLENGTH);
			break;
		}
		++iter_jmp;
	}

	//修复其他数据
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

//把m_basic存放的指令放到缓存里
void VMLoader2::SaveInstructionToBuffer()
{
	XEDPARSE xed = { 0 };
	xed.x64 = false;//是否是64位
	xed.cip = (ULONGLONG)m_vmps.vmstartaddr + m_szcalc_2;
	
	vector<BASICBLOCKADDR>::iterator iter_basic = m_basic.begin();
	for (int i = 0; i < m_basic.size(); i++)
	{
		memcpy(xed.instr, (*iter_basic).instruction, STRUCTIONLENGTH);
		if (XEDPARSE_OK != XEDParseAssemble(&xed))
		{
			//cout << "指令错误：" << xed.error << endl;
		}
		//printf("%X : %s\n", (int)xed.cip, xed.instr);
		memcpy((char*)m_vmps.instbuffaddr+ m_szcalc_2, xed.dest, xed.dest_size);
		xed.cip += xed.dest_size;
		m_szcalc_2 += xed.dest_size;
		++iter_basic;
	}	
}

//构造基本块和垃圾指令块
void VMLoader2::Const_BasicAndRub_Block(char* instruction, int index)
{
	CString jmpdispatcher = "jmp ";
	jmpdispatcher = jmpdispatcher + m_vmps.dispatcheraddr;//调度器地址
	
	char jccstr[] = "jge 0x66666666";
	//char jccstr[] = "mov eax,ebx";
	//1、垃圾块
	Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), index);
	
	//2、JCC块(基本块一)
	Construct_Jcc_Instruction(jccstr, index);

	//3、垃圾块
	Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), index);

	//4、基本块二
	Construct_BasicBlock_Instruction(instruction, jmpdispatcher.GetString(), 2);

	//5、垃圾块
	Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), index);
	Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), index);
	//6、基本块三
	Construct_BasicBlock_Instruction(instruction, jmpdispatcher.GetString(), 3);

	//7、垃圾块
	Construct_Rub_Instruction_Num(jmpdispatcher.GetString(), index);



	//8、修复m_basic链表里数据
	Modify_BasicData();
	//9、把m_basic存放的指令放到缓存里
	SaveInstructionToBuffer();
	//10、清空m_basic链表数据
	m_basic.clear();
}


//判断指令是否能够解析
bool VMLoader2::IsAnalysis(char* str)
{
	XEDPARSE xed = { 0 };
	xed.x64 = false;//是否是64位
	xed.cip = (ULONGLONG)m_vmps.vmstartaddr + m_szcalc;

	memcpy(xed.instr, str, STRUCTIONLENGTH);
	if (XEDPARSE_OK != XEDParseAssemble(&xed))
	{
		//cout << "指令错误：" << xed.error << endl;
		return false;
	}
	return true;
}


//解决不能分割指令的问题
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
	{//关键指令
		ljmptbl.origin = m_origin;
		ljmptbl.addr = m_vmps.vmstartaddr + m_szcalc;
		m_jmptbl.push_back(ljmptbl);
	}
	//解析指令
	XEParseIns(cstr.GetString());
	//保存指令
	SaveInstructionToBuffer();
	//清空m_basic链表
	m_basic.clear();
}


//加载handler(预加载函数)
void VMLoader2::PreLoadHandler()
{
	vector<const char*>voptionreg;
	for (int i = 0; i < OPTIONREGNUMS; i++)
	{//把可选寄存器加入voptionreg链表
		voptionreg.push_back(optionreg[i]);
	}

	//加载handler和垃圾指令
	for (int i = 0; i < PRENUMS; i++)
	{
		//打乱顺序
		random_shuffle(voptionreg.begin(), voptionreg.end());

		char* reg01 = (char*)voptionreg[0];
		char* reg02 = (char*)voptionreg[1];
		char* reg03 = (char*)voptionreg[2];
		m_reg04 = reg03;
		char* reg04 = (char*)voptionreg[3];
		m_reg05 = reg04;		
		CString tempstr= Predict[i].s_Func(reg01, reg02);
		//打印
		//printf("%s", tempstr.GetString());
		char linestr[32] = { 0 };
		int nlen = tempstr.GetCount();
		char* cstr = tempstr.GetString();
		for (int i = 0; i < nlen;)
		{
			int ncalc = tempstr.LeftFindChar(cstr + i, '\n');
			memcpy(linestr, cstr + i, ncalc - 1);
			if (IsAnalysis(linestr))
			{//把所得指令变形为三个基本块(JCC基本块、基本块一、基本块二)				
				Const_BasicAndRub_Block(linestr, i);
			}
			else
			{//解决指令的黏滞问题和关键指令的问题				
				ColdTreatment(linestr);
			}

			memset(linestr, 0, 32);
			i += ncalc;
		}

		m_bool = true;
		cold_bool = true;

		//保存预加载的所有信息
		PREINFORMATION preinfor;
		memcpy(preinfor.vmfunc, Predict[i].vm_opcoed, OPECODELENGTH);
		preinfor.temp_jmptbl.assign(m_jmptbl.begin(), m_jmptbl.end());
		m_preinformation.push_back(preinfor);

		//清除m_jmptbl跳转表
		m_jmptbl.clear();
	}

}

//扫描要保护的硬编码，翻译为汇编
void VMLoader2::DisassembleEngine()
{
	DISASM disAsm = { 0 };
	//配置结构体，初始化disAsm
	disAsm.EIP = (UIntPtr)m_vmps.startaddr;
	disAsm.VirtualAddr = (UInt64)m_vmps.originaddr;
	disAsm.Archi = 0;//0表示32位，64表示64位
	disAsm.Options = 0;
	int nCount = 0;
	int nLen = 0;

	//扫描硬编码，翻译为汇编
	while (nCount < m_vmps.length)
	{
		nLen = Disasm(&disAsm);
		//打印
		//printOpcode(disAsm, nLen);

		//解析指令,翻译为中间表示
		MiddleRepresent(disAsm);

		nCount += nLen;
		disAsm.EIP += nLen;
		disAsm.VirtualAddr += nLen;
	}


}

//普通handler处理和特殊handler处理
void VMLoader2::TransformToHandler_1(int index,bool isGeneral)
{
	CString tempstr = "";
	if (isGeneral)
	{
		vector<const char*>voptionreg;
		for (int i = 0; i < OPTIONREGNUMS; i++)
		{//把可选寄存器加入voptionreg链表
			voptionreg.push_back(optionreg[i]);
		}

		//打乱顺序
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
	
	//打印
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
		{//把所得指令变形为三个基本块(JCC基本块、基本块一、基本块二)				
			Const_BasicAndRub_Block(linestr, i);
		}
		else
		{//解决指令的黏滞问题和关键指令的问题				
			ColdTreatment(linestr);
		}

		memset(linestr, 0, 32);
		i += ncalc;
	}
	m_bool = true;
	cold_bool = true;

}

//把中间表示换成handler，并加垃圾指令
void VMLoader2::TransformToHandler()
{
	vector< MIDDLESTRUCT>::iterator iter_middle = m_middle.begin();

	for (int i = 0; i < m_middle.size(); i++)
	{
		if (0 == _stricmp((*iter_middle).vmfunc,"VCheckESP "))
		{//如果是VCheckESP函数
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


		for (int t = 0; t < PRENUMS; t++)//判断是不是符合预加载函数
		{
			if (0 == _stricmp(Predict[t].vm_opcoed, (*iter_middle).vmfunc))
			{				
				//修改地址
				m_preinformation[t].temp_jmptbl.at(0).origin=(*iter_middle).originaddr;

				//将temp_jmptbl追加到m_jmptbl
				m_jmptbl.insert(m_jmptbl.end(), m_preinformation[t].temp_jmptbl.begin(), m_preinformation[t].temp_jmptbl.end());

				m_preinformation[t].temp_jmptbl.at(0).origin = 0;
				goto continue_;
			}
		}

		for (int j = 0; j < FUNNUMS; j++)
		{//普通handler
			if (0 == _stricmp(g_FunName[j].vm_opcoed,(*iter_middle).vmfunc))
			{
				m_origin = (*iter_middle).originaddr;
				TransformToHandler_1(j,true);
				goto continue_;
			}
		}
		for (int k = 0; k < MEMNUMS; k++)
		{//特殊handler
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

//修复数据表的数据
void VMLoader2::ModifyDataTbl()
{
	vector<DATATABLE>::iterator iter_data = m_datatable.begin();
	vector<JMPTABLE>::iterator iter_jmp = m_jmptbl.begin();
	DWORD jccaddr = 0;
	DWORD jmpaddr = 0;

	//修复其他数据(比如JCC跳转了，那么数据表中的数据也要跳转)
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
	{//修复数据表中JCC要用到的数据
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

//恢复ebp和esi在vm里的数据的地址的函数
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

//主函数
void VMLoader2::MainVMP()
{	
	//1、加载handler(预加载函数)
	PreLoadHandler();
	ResumeEsiAndEbp();

	//2、乱序垃圾指令，用于vm环境开始前
	Out_Of_Order_RubInstruction();

	//3、读取需要保护的指令
	DisassembleEngine();

	//4、把中间表示换成handler，并加垃圾指令
	TransformToHandler();

	//5、修复数据表的数据
	ModifyDataTbl();

}


//生成垃圾指令
CString VMLoader2::ProduceRubbishOpecode(char* reg04, char* reg05)
{
	VMTable vmtbl = vmtable32[SrandNum(0, m_vmlength)];
	CString str = vmtbl.strInstruction;
	//1、目的操作
	switch (vmtbl.optype[0])
	{
	case NONETYPE://没有操作数
		break;
	case IMMTYPE://立即数
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
	case REGTYPE://寄存器
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
	case MEMTYPE://内存
	{//随机选择vmp1节中没有用到的内存
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

	//2、源操作数
	switch (vmtbl.optype[1])
	{
	case NONETYPE://没有操作数
		break;
	case IMMTYPE://立即数
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
	case REGTYPE://寄存器(操作数2的寄存器可以在8个寄存器中任意选择)
	{
		if (0 == _stricmp(vmtbl.strInstruction,"xchg"))
		{//如果是xchg，寄存器则选择reg04，或者reg05
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
	case MEMTYPE://内存
	{//随机选择vmp1节内的地址，或者选esp寄存器，或者选esi寄存器
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

//处理内存操作，把内存翻译为中间表示
void VMLoader2::MemoryMiddle(DISASM disAsm,MEMORYTYPE memtype)
{
	/*
	* vPushImm4
	* vPushReg
	* vMUL_MEM //内存操作专用乘法handler
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
		m_middle.push_back(midstr);//操作码压入中间表示	


		datatbl.data = memtype.Scale;
		datatbl.recodeOaddr = disAsm.VirtualAddr;
		m_datatable.push_back(datatbl);//数据压入数据表

		for (int i = 0; i < REGNUMS; i++)
		{
			if (memtype.IndexRegister == tempreg[i].index)
			{
				char vpushreg4[] = "vPushReg4 ";
				//printf("%s\n", vpushreg4);
				memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
				m_middle.push_back(midstr);//操作码压入中间表示	

				DATATABLE datatbl;

				datatbl.data = i;
				datatbl.recodeOaddr = disAsm.VirtualAddr;
				m_datatable.push_back(datatbl);//数据压入数据表

				break;
			}
		}

		char vmul_mem[] = "vMUL_MEM ";
		//printf("%s\n", vmul_mem);
		memcpy(midstr.vmfunc, vmul_mem, sizeof(vmul_mem));
		m_middle.push_back(midstr);//操作码压入中间表示	
	}
	
	int i = 0;
	for (; i < REGNUMS; i++)
	{
		if (memtype.BaseRegister == tempreg[i].index)
		{
			char vpushreg4[] = "vPushReg4 ";
			//printf("%s\n", vpushreg4);
			memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
			m_middle.push_back(midstr);//操作码压入中间表示	

			DATATABLE datatbl;
			datatbl.data = i;
			datatbl.recodeOaddr = disAsm.VirtualAddr;
			m_datatable.push_back(datatbl);//数据压入数据表
			break;
		}
	}

	if ((memtype.IndexRegister != 0) && (i != REGNUMS))
	{
		char vadd[] = "vAdd4 ";
		//printf("%s\n", vadd);
		memcpy(midstr.vmfunc, vadd, sizeof(vadd));
		m_middle.push_back(midstr);//操作码压入中间表示
	}
	

	if (0 != memtype.Displacement)
	{
		char vpushreg4[] = "vPushImm4 ";
		//printf("%s\n", vpushreg4);
		memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
		m_middle.push_back(midstr);//操作码压入中间表示	

		datatbl.data = memtype.Displacement;
		m_datatable.push_back(datatbl);//数据压入数据表
	}
	

	if ((0 != memtype.Displacement) && (i != REGNUMS))
	{
		char vadd[] = "vAdd4 ";
		//printf("%s\n", vadd);
		memcpy(midstr.vmfunc, vadd, sizeof(vadd));
		m_middle.push_back(midstr);//操作码压入中间表示
	}
}

//解析要保护的指令，翻译为中间表示
void VMLoader2::MiddleRepresent(DISASM disAsm)
{
	MIDDLESTRUCT midstr;
	midstr.originaddr = disAsm.VirtualAddr;
	bool IsSimulation = true;

	//vm的环境准备
	if (start_bool)
	{		

		char vresumestart1[] = "VMStartVM ";
		printf("%s\n", vresumestart1);
		memcpy(midstr.vmfunc, vresumestart1, sizeof(vresumestart1));
		m_middle.push_back(midstr);//压入VMStartVM

		char vresumestart[] = "VMStartVM_2 ";
		printf("%s\n", vresumestart);
		memcpy(midstr.vmfunc, vresumestart, sizeof(vresumestart));
		m_middle.push_back(midstr);//压入VMStartVM_2
		start_bool = false;
	}
	

/*----------------------------------------------------------------------------------*/
/*	0、判断是否是不可模拟指令		      				     */
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
	//0、1 如果是不可模拟指令,处理好后直接返回
	if (IsSimulation)
	{
		char retnotaddr[] = "vRetnNOT_ ";
		//printf("%s\n", retnotaddr);
		memcpy(midstr.vmfunc, retnotaddr, sizeof(retnotaddr));
		m_middle.push_back(midstr);//先压入vRetnNOT

		char notsimulate[] = "vNotSimulate ";
		char vresumestartaddr[OPECODELENGTH];
		sprintf(vresumestartaddr, "%X", m_vmps.vresumestartaddr);

		//printf("%s\n", notsimulate);
		memcpy(midstr.vmfunc, notsimulate, sizeof(notsimulate));
		memcpy(midstr.param1, disAsm.CompleteInstr, STRUCTIONLENGTH);
		memcpy(midstr.param2, vresumestartaddr, OPECODELENGTH);
		m_middle.push_back(midstr);//再压入不可模拟指令的handler
		
		char vresumestart[] = "vResumeStart_2 ";
		//printf("%s\n", vresumestart);
		memcpy(midstr.vmfunc, vresumestart, sizeof(vresumestart));
		m_middle.push_back(midstr);//压入vResumeStart_2

		return;
	}

/*----------------------------------------------------------------------------------*/
/*		1、是否有操作数3		      	
		暂时没有处理操作3，因为一条汇编指令绝大多数情况下，都是零地址指令、一地址指令、二地址指令，很少有三地址指令。
			    
/*----------------------------------------------------------------------------------*/
	if (NO_ARGUMENT != disAsm.Argument3.ArgType)
	{
		switch (disAsm.Argument3.ArgType & 0xF0000000)
		{
		case REGISTER_TYPE: //寄存器
			break;
		case MEMORY_TYPE: //内存
			break;
		case CONSTANT_TYPE://常数
			break;
		default:
			break;
		}
	}

/*----------------------------------------------------------------------------------*/
/*	2、是否有操作数2		      				     */
/*----------------------------------------------------------------------------------*/
	if (NO_ARGUMENT != disAsm.Argument2.ArgType)
	{
		switch (disAsm.Argument2.ArgType & 0xF0000000)
		{
		case REGISTER_TYPE: //寄存器
		{
			for (int i = 0; i < REGNUMS; i++)
			{
				if (tempreg[i].index == (disAsm.Argument2.ArgType & 0xFFFF))
				{
					if (0x20 == disAsm.Argument2.ArgSize)
					{//32位
						char vpushreg4[] = "vPushReg4 ";
						//printf("%s\n", vpushreg4);
						memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
						m_middle.push_back(midstr);//操作码压入中间表示							
					}
					else if (0x10 == disAsm.Argument2.ArgSize)
					{//16位
						char vpushreg4[] = "vPushReg2 ";
						//printf("%s\n", vpushreg4);
						memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
						m_middle.push_back(midstr);//操作码压入中间表示	
					}
					else
					{//8位(要判断高位还是低位)
						for (int i = 0; i < 14; i++)
						{
							if (_stricmp(disAsm.Argument2.ArgMnemonic, regname_[0][i]) == 0)
							{
								if (i<=3)//小于等于3是低位
								{
									char vpushreg4[] = "vPushReg1_low ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//操作码压入中间表示	
								}
								else
								{
									char vpushreg4[] = "vPushReg1_above ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//操作码压入中间表示	
								}
								break;
							}
						}
					}
					DATATABLE datatbl;
					datatbl.data = i;
					datatbl.recodeOaddr = disAsm.VirtualAddr;
					m_datatable.push_back(datatbl);//数据压入数据表
					break;
				}
			}
			
		}
			break;
		case MEMORY_TYPE: //内存
		{
			if (0 == _stricmp("pop ", disAsm.Instruction.Mnemonic)) break;
			if (0 == _stricmp("ret ", disAsm.Instruction.Mnemonic)) break;

			MemoryMiddle(disAsm, disAsm.Argument2.Memory);
			
			if (0x20 == disAsm.Argument2.ArgSize)
			{//32位
				char vReadMem[] = "vReadMemDs4 ";
				//printf("%s\n", vReadMem);
				memcpy(midstr.vmfunc, vReadMem, sizeof(vReadMem));
			}
			else if (0x10 == disAsm.Argument2.ArgSize)
			{//16位
				char vReadMem[] = "vReadMemDs2 ";
				//printf("%s\n", vReadMem);
				memcpy(midstr.vmfunc, vReadMem, sizeof(vReadMem));
			}
			else
			{//8位
				char vReadMem[] = "vReadMemDs1 ";
				//printf("%s\n", vReadMem);
				memcpy(midstr.vmfunc, vReadMem, sizeof(vReadMem));
			}

			memcpy(midstr.param1, disAsm.Instruction.Mnemonic, 16);//参数一
			memcpy(midstr.param2, disAsm.Argument2.ArgMnemonic, 32);//参数二
			m_middle.push_back(midstr);//操作码压入中间表示
		}
			break;
		case CONSTANT_TYPE://常数
		{
			DWORD constnums = 0;
			sscanf(disAsm.Argument2.ArgMnemonic, "%X", &constnums);

			char vpushimm4[] = "vPushImm4 ";
			//printf("%s\n", vpushimm4);

			memcpy(midstr.vmfunc, vpushimm4, sizeof(vpushimm4));
			m_middle.push_back(midstr);//操作码压入中间表示

			DATATABLE datatbl;
			datatbl.data = constnums;
			datatbl.recodeOaddr = disAsm.VirtualAddr;
			m_datatable.push_back(datatbl);//数据压入数据表

		}
			break;
		default:
			break;
		}
	}

/*----------------------------------------------------------------------------------*/
/*	3、是否有操作数1		      				     */
/*----------------------------------------------------------------------------------*/
	if (NO_ARGUMENT != disAsm.Argument1.ArgType)
	{
		switch (disAsm.Argument1.ArgType & 0xF0000000)
		{
		case REGISTER_TYPE: //寄存器
		{
			for (int i = 0; i < REGNUMS; i++)
			{
				if (tempreg[i].index == (disAsm.Argument1.ArgType & 0xFFFF))
				{
					if (0x20 == disAsm.Argument1.ArgSize)
					{//32位
						char vpushreg4[] = "vPushReg4 ";
						//printf("%s\n", vpushreg4);
						memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
						m_middle.push_back(midstr);//操作码压入中间表示						
					}
					else if (0x10 == disAsm.Argument1.ArgSize)
					{//16位
						char vpushreg4[] = "vPushReg2 ";
						//printf("%s\n", vpushreg4);
						memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
						m_middle.push_back(midstr);//操作码压入中间表示
					}
					else
					{//8位(要判断高位还是低位)
						for (int i = 0; i < 14; i++)
						{
							if (_stricmp(disAsm.Argument1.ArgMnemonic, regname_[0][i]) == 0)
							{
								if (i <= 3)//小于等于3是低位
								{
									char vpushreg4[] = "vPushReg1_low ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//操作码压入中间表示	
								}
								else
								{
									char vpushreg4[] = "vPushReg1_above ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//操作码压入中间表示	
								}
								break;
							}
						}
					}
					DATATABLE datatbl;
					datatbl.data = i;
					datatbl.recodeOaddr = disAsm.VirtualAddr;
					m_datatable.push_back(datatbl);//数据压入数据表
					break;
				}
			}
		}
			break;
		case MEMORY_TYPE: //内存(操作数1如果是内存，且操作数2不为空，则处理好后直接退出)
		{
			if (0 == _stricmp("push ",disAsm.Instruction.Mnemonic)) break;
	
			MemoryMiddle(disAsm, disAsm.Argument1.Memory);			

			if (NO_ARGUMENT == disAsm.Argument2.ArgType) break;//这步是对待单个操作数的

			if (0x20 == disAsm.Argument1.ArgSize)
			{//32位
				char vWriteMem[] = "vWriteMemDs4 ";
				//printf("%s\n", vWriteMem);
				memcpy(midstr.vmfunc, vWriteMem, sizeof(vWriteMem));				
			}
			else if (0x10 == disAsm.Argument1.ArgSize)
			{//16位
				char vWriteMem[] = "vWriteMemDs2 ";
				//printf("%s\n", vWriteMem);
				memcpy(midstr.vmfunc, vWriteMem, sizeof(vWriteMem));
			}
			else
			{//8位
				char vWriteMem[] = "vWriteMemDs1 ";
				//printf("%s\n", vWriteMem);
				memcpy(midstr.vmfunc, vWriteMem, sizeof(vWriteMem));
			}
			
			memcpy(midstr.param1, disAsm.Instruction.Mnemonic, 16);//参数一
			memcpy(midstr.param2, disAsm.Argument1.ArgMnemonic, 32);//参数二
			m_middle.push_back(midstr);//操作码压入中间表示

			return;
		}
			break;
		case CONSTANT_TYPE://常数
		{
			DWORD constnums = 0;
			sscanf(disAsm.Argument1.ArgMnemonic, "%X", &constnums);

			char vpushimm4[] = "vPushImm4 ";
			//printf("%s\n", vpushimm4);

			memcpy(midstr.vmfunc, vpushimm4, sizeof(vpushimm4));
			m_middle.push_back(midstr);//操作码压入中间表示

			DATATABLE datatbl;
			datatbl.data = constnums;
			datatbl.recodeOaddr = disAsm.VirtualAddr;
			m_datatable.push_back(datatbl);//数据压入数据表
		}
			break;
		default:
			break;
		}
	}

/*----------------------------------------------------------------------------------*/
/*	4、处理普通handler		      				     */
/*----------------------------------------------------------------------------------*/
	//4、1 如果是ret，直接返回
	if (0 == _stricmp("ret ", disAsm.Instruction.Mnemonic))
	{
		if (disAsm.Argument1.ArgType == 0x10000000)
		{
			char vpushreg[] = "vPushImm4 ";
			//printf("%s\n", vpushreg);
			memcpy(midstr.vmfunc, vpushreg, sizeof(vpushreg));
			m_middle.push_back(midstr);//压入vPushReg4

			DATATABLE datatbl;
			datatbl.data = 0;
			datatbl.recodeOaddr = disAsm.VirtualAddr;
			m_datatable.push_back(datatbl);//数据压入数据表
		}
		char callmem[] = "vRETN ";
		//printf("%s\n", callmem);
		memcpy(midstr.vmfunc, callmem, sizeof(callmem));
		m_middle.push_back(midstr);//压入vRETN
		return;
	}


	//4、2 如果操作数2是内存，m_middle链表最后两个元素互换
	if ((disAsm.Argument2.ArgType & 0xF0000000) == MEMORY_TYPE)
	{
		if (0 != _stricmp("pop ", disAsm.Instruction.Mnemonic))
		{
			swap(m_middle[m_middle.size()-1], m_middle[m_middle.size() - 2]);
			
		}
		goto ttttt__;
	}

	//4、3 如果是调用函数，则处理好后，直接返回
	if (
		0 == _stricmp("call ", disAsm.Instruction.Mnemonic) ||
		(0 == _stricmp(disAsm.Instruction.Mnemonic, "jmp ") && ((disAsm.Argument1.ArgType & 0xF0000000) == MEMORY_TYPE)) ||
		(0 == _stricmp(disAsm.Instruction.Mnemonic, "jmp ") && ((disAsm.Argument1.ArgType & 0xF0000000) == REGISTER_TYPE))
		)//这儿解决是的jmp代替call的功能，如果jmp在函数内跳转，整个解析器都无法解析，在修复数据表的时候会出错
	{	
		//如果是jmp则要提升堆栈
		if (0 == _stricmp(disAsm.Instruction.Mnemonic, "jmp "))
		{
			char addesp[] = "vAddEsp ";
			//printf("%s\n", addesp);
			memcpy(midstr.vmfunc, addesp, sizeof(addesp));
			m_middle.push_back(midstr);//先压入vCallMem
		}

		
		if ((disAsm.Argument1.ArgType & 0xF0000000) == MEMORY_TYPE)
		{//如果操作数1是内存			
			char callmem[] = "vCallMem ";
			//printf("%s\n", callmem);
			memcpy(midstr.vmfunc, callmem, sizeof(callmem));
			m_middle.push_back(midstr);//先压入vCallMem
		}
		else if ((disAsm.Argument1.ArgType & 0xF0000000) == REGISTER_TYPE)
		{//如果操作数1是寄存器
			char callreg[] = "vCallReg ";
			//printf("%s\n", callreg);
			memcpy(midstr.vmfunc, callreg, sizeof(callreg));
			m_middle.push_back(midstr);//先压入vCallMem
		}
		else
		{
			//删除vPushImm4
			m_middle.pop_back();
		}
		char retnotaddr[] = "vRetnNOT_ ";
		//printf("%s\n", retnotaddr);
		memcpy(midstr.vmfunc, retnotaddr, sizeof(retnotaddr));
		m_middle.push_back(midstr);//压入vRetnNOT

		char vcall[] = "vCALL ";
		//printf("%s\n", vcall);
		memcpy(midstr.vmfunc, vcall, sizeof(vcall));
		m_middle.push_back(midstr);

		char vresumestart[] = "vResumeStart_2 ";
		//printf("%s\n", vresumestart);
		memcpy(midstr.vmfunc, vresumestart, sizeof(vresumestart));
		m_middle.push_back(midstr);//压入vResumeStart_2
		return;
	}

	//4、4 处理普通handler
	for (int i = 0; i < FUNNUMS; i++)
	{
		if (0 == _stricmp(g_FunName[i].s_opecode,disAsm.Instruction.Mnemonic))
		{
			//printf("%s\n", g_FunName[i].vm_opcoed);
			memcpy(midstr.vmfunc, g_FunName[i].vm_opcoed,OPECODELENGTH);
			m_middle.push_back(midstr);//操作码压入中间表示
			break;
		}

	}


/*----------------------------------------------------------------------------------*/
/*	5、判断是否有辅助handler		      				     */
/*----------------------------------------------------------------------------------*/
	if (
		0x10000000 != disAsm.Argument1.ArgType ||
		0x10000000 != disAsm.Argument2.ArgType ||
		0x10000000 != disAsm.Argument3.ArgType
		)
	{
		ttttt__:
		//5.1 判断这条指令是否改变操作数1的值，比如cmp和test操作数就不会改变操作数1的值，直接返回	
		for (int i = 0; i < NOTREGNUMS; i++)
		{
			if (_stricmp(cmp_opecode[i], disAsm.Instruction.Mnemonic) == 0)
			{
				return;
			}
		}
		
		//5.2 判断是否是JCC或者JMP指令，是则处理好后直接返回
		for (int i = 0; i < JCCNUMS; i++)
		{//判断是否是JCC指令
			if (0 == _stricmp(disAsm.Instruction.Mnemonic, JCCstr[i]))
			{
				//改变数据表的数据
				m_datatable.at(m_datatable.size() - 1).originaddr = disAsm.VirtualAddr;
				
				//多压入一个空数据到数据表
				DATATABLE datatbl;
				datatbl.data = 0;
				m_datatable.push_back(datatbl);//数据压入数据表

				//再压入一个空数据到数据表
				datatbl.data = 0;
				m_datatable.push_back(datatbl);//数据压入数据表

				//JCC指令前插入两个vPushImm4
				char vpushimm4[] = "vPushImm4 ";
				//printf("%s\n", vpushimm4);
				memcpy(midstr.vmfunc, vpushimm4, sizeof(vpushimm4));
				m_middle.insert(m_middle.end()-1, midstr);//操作码压入中间表示
				m_middle.insert(m_middle.end() - 1, midstr);//操作码压入中间表示
				return;
			}
		}

		//5.2 处理普通辅助handler
		switch (disAsm.Argument1.ArgType & 0xF0000000)
		{
		case REGISTER_TYPE://寄存器
		{
			for (int i = 0; i < REGNUMS; i++)
			{
				if (tempreg[i].index == (disAsm.Argument1.ArgType & 0xFFFF))
				{
					if (0x20 == disAsm.Argument1.ArgSize)
					{//32位
						char vpopreg4[] = "vPopReg4 ";
						//printf("%s\n", vpopreg4);
						memcpy(midstr.vmfunc, vpopreg4, sizeof(vpopreg4));
						m_middle.push_back(midstr);//操作码压入中间表示						
					}
					else if (0x10 == disAsm.Argument1.ArgSize)
					{//16位
						char vpopreg4[] = "vPopReg2 ";
						//printf("%s\n", vpopreg4);
						memcpy(midstr.vmfunc, vpopreg4, sizeof(vpopreg4));
						m_middle.push_back(midstr);//操作码压入中间表示	
					}
					else
					{//8位(要判断高位还是低位)
						for (int i = 0; i < 14; i++)
						{
							if (_stricmp(disAsm.Argument1.ArgMnemonic, regname_[0][i]) == 0)
							{
								if (i <= 3)//小于等于3是低位
								{
									char vpushreg4[] = "vPopReg1_low ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//操作码压入中间表示	
								}
								else
								{
									char vpushreg4[] = "vPopReg1_above ";
									//printf("%s\n", vpushreg4);
									memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
									m_middle.push_back(midstr);//操作码压入中间表示	
								}
								break;
							}
						}
					}
					DATATABLE datatbl;
					datatbl.data = i;
					m_datatable.push_back(datatbl);//数据压入数据表
				}
			}

			//如果操作数1是esp，即改变栈大小，比如指令sub esp,0x100,
			//则要调用vCheckESP()函数，检查VMContext是否被覆盖
			if (16 == (disAsm.Argument1.ArgType & 0xFFFF))
			{//16在BegEngine反汇编引擎约定的是esp寄存器
				char vcheckesp[] = "VCheckESP ";
				//printf("%s\n", vcheckesp);
				memcpy(midstr.vmfunc, vcheckesp, sizeof(vcheckesp));
				m_middle.push_back(midstr);//操作码压入中间表示
			}
			//如果操作码是pop,把vPOP添加到m_middle链表,然后返回
			if (0 == _stricmp("pop ",disAsm.Instruction.Mnemonic))
			{
				if (0x20 == disAsm.Argument1.ArgSize)
				{
					char vpushreg4[] = "vPOP4 ";
					//printf("%s\n", vpushreg4);
					memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
					m_middle.push_back(midstr);//操作码压入中间表示	
				}
				else
				{
					char vpushreg4[] = "vPOP2 ";
					//printf("%s\n", vpushreg4);
					memcpy(midstr.vmfunc, vpushreg4, sizeof(vpushreg4));
					m_middle.push_back(midstr);//操作码压入中间表示	
				}
				return;
			}
		}
			break;

		case MEMORY_TYPE://内存
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

