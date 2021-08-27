#pragma once
#include "holder.h"
//Handler命令字符串长度
#define VMNAMELEN	32
//VMTABEL表的个数
#define VMTABLEMAXLEN				0x1000
//用来生成handler的汇编代码的结构

//没有用到寄存器
#define NONE	-1

//寄存器
enum RegType
{
	RT_Eax,
	RT_Ecx,
	RT_Edx,
	RT_Ebx,
	RT_Esp,
	RT_Ebp,
	RT_Esi,
	RT_Edi,
	RT_CS,
	RT_DS,
	RT_ES,
	RT_FS,
	RT_GS,
	RT_SS,

	RT_EFlag,//必须是最后一个(为什么?我忘了)
	//
	RT_AH = 20,
	RT_CH,
	RT_DH,
	RT_BH,
};

//操作数类型
enum optype
{
	NONETYPE,
	IMMTYPE,
	REGTYPE,
	MEMTYPE,
	CSTYPE,
	DSTYPE,
	ESTYPE,
	SSTYPE,
	FSTYPE,
	GSTYPE,
};


struct VMTable
{
	char	VMInstrName[VMNAMELEN];		//VM命令名称
	char	strInstruction[16];			//相对的汇编指令
	int		OperandNum;					//操作数个数
	int		Segment;					//段前缀
	int		optype[2];					//操作类型(寄存器,立即数,内存数)
	int		bitnum[2];					//位数

	int		NeedReg[4];					//执行命令前要使用的寄存器
	int		SaveReg[4];					//执行命令后要保存的指令
	BOOL	Reg2Esp;					//第2个寄存器是否恢复,一般为0不恢复
};

//描述一些标准Handler的行为表
extern VMTable	vmtable32[VMTABLEMAXLEN];