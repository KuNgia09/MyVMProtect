#pragma once
#include "holder.h"
//Handler�����ַ�������
#define VMNAMELEN	32
//VMTABEL��ĸ���
#define VMTABLEMAXLEN				0x1000
//��������handler�Ļ�����Ľṹ

//û���õ��Ĵ���
#define NONE	-1

//�Ĵ���
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

	RT_EFlag,//���������һ��(Ϊʲô?������)
	//
	RT_AH = 20,
	RT_CH,
	RT_DH,
	RT_BH,
};

//����������
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
	char	VMInstrName[VMNAMELEN];		//VM��������
	char	strInstruction[16];			//��ԵĻ��ָ��
	int		OperandNum;					//����������
	int		Segment;					//��ǰ׺
	int		optype[2];					//��������(�Ĵ���,������,�ڴ���)
	int		bitnum[2];					//λ��

	int		NeedReg[4];					//ִ������ǰҪʹ�õļĴ���
	int		SaveReg[4];					//ִ�������Ҫ�����ָ��
	BOOL	Reg2Esp;					//��2���Ĵ����Ƿ�ָ�,һ��Ϊ0���ָ�
};

//����һЩ��׼Handler����Ϊ��
extern VMTable	vmtable32[VMTABLEMAXLEN];