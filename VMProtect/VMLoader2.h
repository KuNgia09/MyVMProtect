#pragma once
#include "holder.h"
//指令长度
#define STRUCTIONLENGTH 0x38
//操作数长度
#define OPECODELENGTH 0x16


class VMLoader2
{
public://结构体信息

	//记录基本块的位置
	struct BASICBLOCKADDR
	{
		DWORD nmatch = 0;//暗号
		int index = -1;//索引
		DWORD addr = 0;
		char instruction[STRUCTIONLENGTH] = { 0 };//指令
	};
	vector<BASICBLOCKADDR>m_basic;
	BASICBLOCKADDR temp_basic = { 0 };

	//记录中间表示的结构体
	struct MIDDLESTRUCT
	{
		DWORD originaddr = 0;//原始地址
		char vmfunc[OPECODELENGTH] = { 0 };//handler
		char param1[STRUCTIONLENGTH] = { 0 };//参数一
		char param2[STRUCTIONLENGTH] = { 0 };//参数二
	};
	vector<MIDDLESTRUCT>m_middle;

	//预加载信息的结构体
	struct PREINFORMATION
	{
		char vmfunc[OPECODELENGTH] = { 0 };//handler
		vector<JMPTABLE>temp_jmptbl;
	};
	vector<PREINFORMATION>m_preinformation;


	//跳转表
	vector<JMPTABLE>m_jmptbl;

	//数据表
	vector<DATATABLE>m_datatable;


public:

	VMLoader2(VMPStruct vmps)
	{//初始化		
		InitialVM(vmps);
	}
	VMLoader2() {};
	~VMLoader2() {};

public:
	//初始化
	void InitialVM(VMPStruct vmps);

	//求vmtable32结构体数组的长度
	int VMLength();

	//解析指令并把指令存放到m_basic链表
	void XEParseIns(char* str);
public:
	//垃圾块
	void Construct_Rub_Instruction(char* jmpdispatcher, int index,int nmatch=0);

	//JCC指令块
	void Construct_Jcc_Instruction(char* jccstr, int index);

	//基本块一和基本块二
	void Construct_BasicBlock_Instruction(char* instruction, char* jmpdispatcher, int nbool=3);

	//设置垃圾块的数目
	void Construct_Rub_Instruction_Num(char* jmpdispatcher, int index);

	//修复m_basic链表里的数据
	void Modify_BasicData();

	//把m_basic存放的指令放到缓存里
	void SaveInstructionToBuffer();

	
	//构造基本块和垃圾指令块
	void Const_BasicAndRub_Block(char* instruction, int index);

	//判断指令是否能够解析
	bool IsAnalysis(char* str);

	//解决指令的黏滞问题
	void ColdTreatment(char* str);

	//加载handler(预加载函数)
	void PreLoadHandler();

	//扫描要保护的硬编码，翻译为汇编
	void DisassembleEngine();


	//乱序垃圾指令，用于vm环境开始前
	void Out_Of_Order_RubInstruction();

	//普通handler处理和特殊handler处理
	void TransformToHandler_1(int index, bool isGeneral);

	//把中间表示换成handler，并加垃圾指令
	void TransformToHandler();

	//修复数据表的数据
	void ModifyDataTbl();

public:
	//恢复ebp和esi在vm里的数据的地址的函数
	void ResumeEsiAndEbp();

	//主函数
	void MainVMP();

	//生成随机垃圾指令
	CString ProduceRubbishOpecode(char* reg03, char* reg04);

	//处理内存，把内存翻译位中间表示
	void MemoryMiddle(DISASM disAsm,MEMORYTYPE memtype);

	//解析要保护的指令，翻译为中间表示
	void MiddleRepresent(DISASM disAsm);

public:
	//获取随机数(范围在Min_到MAX_)
	DWORD SrandNum(int Min_, int Max_)
	{
		return rand() % (Max_ - Min_) + Min_;
	}
public:
	//vmtable32结构体数组的长度
	int m_vmlength = 0;
	VMPStruct m_vmps = { 0 };

	//可选寄存器
	char* m_reg04;
	char* m_reg05;

	//记录指令存放到了哪里
	int m_szcalc=0;
	int m_szcalc_2 = 0;

	//开关
	bool m_bool = true;

	//黏滞指令的开关,作用是把黏滞指令的地址存入跳转表中
	bool cold_bool = true;
	//指令原始地址
	DWORD m_origin = 0;

	//开始指令
	bool start_bool = true;
};

