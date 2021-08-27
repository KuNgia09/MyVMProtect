/*
* ע��
* 1��ָ��ǰ����һ��z����ʾ�ǲ��ɷָ�ָ����ָܷ�
* 2��ָ��ǰ����һ��-����ʾһ��JCC��ǣ������޸����ݱ��������

*/

#include "vmtest.h"
#include "holder.h"
extern VMPStruct g_vmstruct;

CString vINC(char* VR0, char* VR1)
{
	CString str = "";
	str = str + "mov " + VR0 + ",dword ptr[esp]\n";
	str = str + "add " + VR0 + ",1\n";
	str = str + "add esp,4\n";
	str = str + "push " + VR0 + "\n";
	return str;
}

CString vMOV4(char* VR0,char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[esp]\n";
	str = str + "mov " + VR1 + ",dword ptr[esp+4]\n";
	str = str + "mov " + VR0 +","+ VR1 + "\n";
	str = str + "add esp,8\n";
	str = str + "push " + VR0+"\n";

	return str;
}

CString vAdd4(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[esp]\n";
	str = str + "mov " + VR1 + ",dword ptr[esp+4]\n";

	str = str + "push dword ptr[edi+0x20]\n";

	str = str + "zpopfd\n";
	str = str + "zadd " + VR0 + "," + VR1 + "\n";
	str = str + "zpushfd\n";

	str = str + "pop dword ptr[edi+0x20]\n";
	str = str + "add esp,8\n";
	str = str + "push " + VR0 + "\n";
	return str;
}

CString vSUB4(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[esp]\n";
	str = str + "mov " + VR1 + ",dword ptr[esp+4]\n";

	str = str + "push dword ptr[edi+0x20]\n";

	str = str + "zpopfd\n";
	str = str + "zsub " + VR0 + "," + VR1 + "\n";
	str = str + "zpushfd\n";

	str = str + "pop dword ptr[edi+0x20]\n";

	str = str + "add esp,8\n";
	str = str + "push " + VR0 + "\n";
	return str;
}

CString vXOR4(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[esp]\n";
	str = str + "mov " + VR1 + ",dword ptr[esp+4]\n";
	str = str + "xor " + VR0 + "," + VR1 + "\n";
	str = str + "add esp,8\n";
	str = str + "push " + VR0 + "\n";
	return str;
}

CString vPUSH(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[esp]\n";
	str = str + "mov " + VR1 + ",dword ptr[edi+0x00]\n";
	str = str + "sub " + VR1 + ",4\n";
	str = str + "mov dword ptr[" + VR1 + "]," + VR0 + "\n";
	str = str + "sub dword ptr[edi+0x00],4\n";
	str = str + "add esp,4\n";
	
	return str;
}

CString vPOP4(char* VR0, char* VR1)
{
	CString str = "";	
	str = str + "mov " + VR1 + ",dword ptr[edi+0x00]\n";
	str = str + "mov " + VR0 + ",dword ptr[" + VR1 + "]\n";
	str = str + "add dword ptr[edi+0x00],4\n";


	str = str +"mov " + VR1 + ",dword ptr[ebp-4]\n";
	str = str + "xor " + VR1 + "," + dataencrypt + "\n";

	str = str + "mov dword ptr [edi+" + VR1 + "*4],"+VR0+"\n";
	
	//str = str + "add esp,4\n";
	return str;
}

//������ջ
CString vAddEsp(char* VR0, char* VR1)
{
	CString str = "";
	str = str + "add dword ptr[edi+0x00],4\n";
	return str;
}


CString vLEA(char* VR0, char* VR1)
{

	return CString();
}

CString vCMP(char* VR0, char* VR1)
{
	CString str = "mov ";;
	str = str + VR0 + ",dword ptr[esp]\n";
	str = str + "push dword ptr[edi + 0x20]\n";

	//���ɷָ�ָ��
	str = str + "zpopfd\n";
	str = str + "zcmp " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zpushfd\n";

	str = str + "pop dword ptr[edi+0x20]\n";
	str = str + "add esp,8\n";
	return str;
}

CString vTEST(char* VR0, char* VR1)
{
	CString str = "mov ";;
	str = str + VR0 + ",dword ptr[esp]\n";
	str = str + "push dword ptr[edi + 0x20]\n";

	//���ɷָ�ָ��
	str = str + "zpopfd\n";
	str = str + "ztest " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zpushfd\n";

	str = str + "pop dword ptr[edi+0x20]\n";
	str = str + "add esp,8\n";
	return str;
}


CString vJAE(char* VR0, char* VR1)//jae jnc jnb(�޷��� ���ڵ�����ת CF=0)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "pop "+ VR0 +"\n";
	str = str + "mov " + VR1 + ",0\n";
	//���ɷָ�ָ��
	str = str + "zand "+ VR0 +",1\n";
	str = str + "zmov "+ VR0 +",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove "+ VR0 +",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi,"+ VR0 +"\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJA(char* VR0, char* VR1)//ja jnbe (�޷��� ������ת CF=0 or ZF=0)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "mov " + VR1 + ",0\n";

	//���ɷָ�ָ��	
	str = str + "zpopfd\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmova " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmova " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;

}

CString vJB(char* VR0, char* VR1) //jb jnae jc (�޷��� С����ת CF=1)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "mov " + VR1 + ",0\n";
	str = str + "and " + VR0 + ",1\n";
	//���ɷָ�ָ��
	str = str + "zcmp " + VR0 + ",1\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJBE(char* VR0, char* VR1)//jbe jna (�޷��� С�ڵ�����ת CF=1 or ZF=1)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "mov " + VR1 + ",0\n";

	//���ɷָ�ָ��	
	str = str + "zpopfd\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmovbe " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmovbe " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}


CString vJGE(char* VR0, char* VR1)//jnl jge(�з��� ���ڵ�����ת SF=OF)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "mov " + VR1 + ",0\n";

	//���ɷָ�ָ��	
	str = str + "zpopfd\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmovge " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmovge " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJG(char* VR0, char* VR1)//jg jnle (�з��� ������ת ZF=0 and SF=OF)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "mov " + VR1 + ",0\n";

	//���ɷָ�ָ��	
	str = str + "zpopfd\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmovg " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmovg " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJL(char* VR0, char* VR1)//jl jnge (�з��� С����ת SF!=OF)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "mov " + VR1 + ",0\n";

	//���ɷָ�ָ��	
	str = str + "zpopfd\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmovl " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmovl " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJLE(char* VR0, char* VR1)//jle jng (�з��� С�ڵ�����ת ZF=1 or SF!=OF)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "mov " + VR1 + ",0\n";

	//���ɷָ�ָ��	
	str = str + "zpopfd\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmovle " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmovle " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJZ(char* VR0, char* VR1)//jz je (Ϊ����ת ZF=1)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "mov " + VR1 + ",0\n";
	str = str + "and " + VR0 + ",40\n";
	//���ɷָ�ָ��
	str = str + "zcmp " + VR0 + ",40\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJNZ(char* VR0, char* VR1)//jnz jne(������ת ZF=0)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "mov " + VR1 + ",0\n";
	//���ɷָ�ָ��
	str = str + "zand " + VR0 + ",40\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJO(char* VR0, char* VR1)//jo (�����ת OF=1)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "mov " + VR1 + ",0\n";
	str = str + "and " + VR0 + ",800\n";
	//���ɷָ�ָ��
	str = str + "zcmp " + VR0 + ",800\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJNO(char* VR0, char* VR1)//jno (�������ת OF=0)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "mov " + VR1 + ",0\n";
	//���ɷָ�ָ��
	str = str + "zand " + VR0 + ",800\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJS(char* VR0, char* VR1)//js (�з�����ת SF=1)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "mov " + VR1 + ",0\n";
	str = str + "and " + VR0 + ",80\n";
	//���ɷָ�ָ��
	str = str + "zcmp " + VR0 + ",80\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJNS(char* VR0, char* VR1)//jns (�޷�����ת SF=0)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "mov " + VR1 + ",0\n";
	//���ɷָ�ָ��
	str = str + "zand " + VR0 + ",80\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJPE(char* VR0, char* VR1)//jp jpe (żУ����ת PF=1)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "mov " + VR1 + ",0\n";
	str = str + "and " + VR0 + ",4\n";
	//���ɷָ�ָ��
	str = str + "zcmp " + VR0 + ",4\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJPO(char* VR0, char* VR1)//jnp jpo (��У����ת PF=0)
{
	CString str = "push dword ptr[edi+0x20]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "mov " + VR1 + ",0\n";
	//���ɷָ�ָ��
	str = str + "zand " + VR0 + ",4\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJCXZ(char* VR0, char* VR1)// CX=0 ��ת
{
	CString str = "push dword ptr[edi+0x8]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "and " + VR0 + ",FFFF\n";
	str = str + "mov " + VR1 + ",0\n";
	//���ɷָ�ָ��
	str = str + "zxor " + VR0 + ",0\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJECXZ(char* VR0, char* VR1)//ECX=0 ��ת
{
	CString str = "push dword ptr[edi+0x8]\n";
	str = str + "pop " + VR0 + "\n";
	str = str + "mov " + VR1 + ",0\n";

	//���ɷָ�ָ��
	str = str + "zxor " + VR0 + ",0\n";
	str = str + "zmov " + VR0 + ",dword ptr[esp+4]\n";
	str = str + "zcmove " + VR1 + ",dword ptr[esp]\n";
	str = str + "zcmove " + VR0 + ",dword ptr[esp+8]\n";

	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JCC����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi," + VR0 + "\n";

	str = str + "add esp,0xC\n";
	return str;
}

CString vJMP(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR1 + ",dword ptr[esp]\n";
	str = str + "mov " +VR0 + ",dword ptr[esp+8]\n";
	str = str + "add ebp," + VR1 + "\n";

	//�ؼ�ָ���¼JMP����ָ���ԭʼ��ַ�������޸����ݱ��е�����
	str = str + "-add esi,"+VR0+"\n";

	str = str + "add esp,0xC\n";
	return str;
}


CString vPushReg4(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 +",dword ptr[ebp]\n" ;
	str = str + "add ebp,4\n";
	str = str + "xor " + VR0 + "," + dataencrypt + "\n";
	str = str + "mov "+ VR0 +",dword ptr [edi+"+ VR0 +"*4]\n";
	str = str + "push "+ VR0 +"\n";
	return str;
}

CString vPushReg2(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[ebp]\n";
	str = str + "add ebp,4\n";
	str = str + "xor " + VR0 + "," + dataencrypt + "\n";
	str = str + "mov " + VR0 + ",dword ptr [edi+" + VR0 + "*4]\n";
	str = str + "and " + VR0 + ",0xFFFF\n";
	str = str + "push " + VR0 + "\n";
	return str;
}

CString vPushReg1_low(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[ebp]\n";
	str = str + "add ebp,4\n";
	str = str + "xor " + VR0 + "," + dataencrypt + "\n";
	str = str + "mov " + VR0 + ",dword ptr [edi+" + VR0 + "*4]\n";
	str = str + "and " + VR0 + ",0xFF\n";
	str = str + "push " + VR0 + "\n";
	return str;
}

CString vPushReg1_above(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[ebp]\n";
	str = str + "add ebp,4\n";
	str = str + "xor " + VR0 + "," + dataencrypt + "\n";
	str = str + "mov " + VR0 + ",dword ptr [edi+" + VR0 + "*4]\n";
	str = str + "and " + VR0 + ",0xFF00\n";
	str = str + "shr " + VR0 + ",8\n";
	str = str + "push " + VR0 + "\n";
	return str;
}

CString vPopReg1_low(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[ebp]\n";
	str = str + "xor " + VR0 + "," + dataencrypt + "\n";
	str = str + "add ebp,4\n";

	str = str + "pop " + VR1 + "\n";
	str = str + "and " + VR1 + ",0xFF\n";
	str = str + "push " + VR1 + "\n";

	str = str + "push dword ptr[edi+" + VR0 + "*4]\n";
	str = str + "pop " + VR1 + "\n";
	str = str + "and " + VR1 + ",0xFFFFFF00\n";
	str = str + "or " + VR1 + ",dword ptr[esp]\n";
	str = str + "mov dword ptr[esp]," + VR1 + "\n";
	str = str + "pop dword ptr[edi+" + VR0 + "*4]\n";
	return str;
}

CString vPopReg1_above(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[ebp]\n";
	str = str + "xor " + VR0 + "," + dataencrypt + "\n";
	str = str + "add ebp,4\n";

	str = str + "pop " + VR1 + "\n";
	str = str + "and " + VR1 + ",0xFF\n";
	str = str + "shl " + VR1 + ",8\n";
	str = str + "push " + VR1 + "\n";

	str = str + "push dword ptr[edi+" + VR0 + "*4]\n";
	str = str + "pop " + VR1 + "\n";
	str = str + "and " + VR1 + ",0xFFFF00FF\n";
	str = str + "or " + VR1 + ",dword ptr[esp]\n";
	str = str + "mov dword ptr[esp]," + VR1 + "\n";
	str = str + "pop dword ptr[edi+" + VR0 + "*4]\n";
	return str;
}

CString vPopReg2(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[ebp]\n";
	str = str + "xor " + VR0 + "," + dataencrypt + "\n";
	str = str + "add ebp,4\n";
	str = str + "pop word ptr[edi+" + VR0 + "*4]\n";
	return str;
}

CString vPopReg4(char* VR0, char* VR1)
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[ebp]\n";
	str = str + "xor " + VR0 + "," + dataencrypt + "\n";
	str = str + "add ebp,4\n";
	str = str + "pop dword ptr[edi+" + VR0 + "*4]\n";
	return str;
}

CString vPushImm4(char* VR0, char* VR1  )
{
	CString str = "mov ";
	str = str + VR0 + ",dword ptr[ebp]\n";
	str = str + "xor " + VR0 + "," + dataencrypt + "\n";
	str = str + "add ebp,4\n";
	str = str + "push " + VR0+"\n";
	return str;
}


#define MEMOPTREG 3
const char* memoptreg[MEMOPTREG][MEMOPTREG] = { 
	{"al","cl","dl"},
	{"ax","cx","dx"},
	{"eax","ecx","edx" }	
};
//�����ȡ�Ĵ���
void MemOptReg(int &x,int &y)
{
	x = rand() % MEMOPTREG;
	if (2==x)
	{
		y = x - 1;
	}
	else
	{
		y = x + 1;
	}
}

CString vPOP2(char* VR0, char* VR1)
{
	int x = 0; int y = 0;
	MemOptReg(x, y);
	CString str = "";
	str = str + "mov " + memoptreg[2][x] + ",dword ptr[edi+0x00]\n";
	str = str + "mov " + memoptreg[2][y] + ",dword ptr[" + memoptreg[2][x] + "]\n";
	str = str + "add dword ptr[edi+0x00],4\n";


	str = str + "mov " + memoptreg[2][x] + ",dword ptr[ebp-4]\n";
	str = str + "xor " + memoptreg[2][x] + "," + dataencrypt + "\n";

	str = str + "mov word ptr [edi+" + memoptreg[2][x] + "*4]," + memoptreg[1][y] + "\n";

	str = str + "add esp,4\n";
	return str;
}


//Ҫ�ı��־�Ĵ����Ĳ�����
#define CHAGEREG 6
const char* chagereg[CHAGEREG] = { "and ","add ","sub ","cmp ","test ","xor " };

//��д�ڴ�ר�ó˷�handler
CString vMUL_MEM(char* VR0, char* VR1)
{
	CString str = "mov eax,dword ptr[esp]\n";
	str = str + "mul dword ptr[esp+4]\n";
	str = str + "add esp,8\n";
	str = str + "push eax\n";
	return str;
}

//���ڴ� ������8λ
CString vReadMemDs1(char* constn, char* dst)
{
	int x = 0; int y = 0;
	MemOptReg(x, y);

	CString str = "mov ";
	str = str + memoptreg[2][x] + ",dword ptr[esp+4]\n";
	str = str + "mov " + memoptreg[2][y] + ",dword ptr[esp]\n";

	int i = 0;
	for (; i < CHAGEREG; i++)
	{
		if (0 == _stricmp(chagereg[i], constn))
		{
			break;
		}
	}
	if (i == CHAGEREG)
	{
		str = str + constn + memoptreg[0][y] + ",byte ptr[" + memoptreg[2][x] + "]" + "\n";
	}
	else
	{
		str = str + "push dword ptr[edi + 0x20]\n";

		//���ɷָ�ָ��
		str = str + "zpopfd\n";
		str = str + "z" + constn + memoptreg[0][y] + ",byte ptr[" + memoptreg[2][x] + "]" + "\n";
		str = str + "zpushfd\n";

		str = str + "pop dword ptr[edi+0x20]\n";
	}

	
	str = str + "add esp,8\n";
	str = str + "push " + memoptreg[2][y] + "\n";
	printf("%s\n", str.GetString());
	return str;
}
//���ڴ� ������16λ
CString vReadMemDs2(char* constn, char* dst)
{
	int x = 0; int y = 0;
	MemOptReg(x, y);

	CString str = "mov ";
	str = str + memoptreg[2][x] + ",dword ptr[esp+4]\n";
	str = str + "mov " + memoptreg[2][y] + ",dword ptr[esp]\n";

	int i = 0;
	for (; i < CHAGEREG; i++)
	{
		if (0 == _stricmp(chagereg[i], constn))
		{
			break;
		}
	}
	if (i == CHAGEREG)
	{
		str = str + constn + memoptreg[1][y] + ",word ptr[" + memoptreg[2][x] + "]" + "\n";
	}
	else
	{
		str = str + "push dword ptr[edi + 0x20]\n";

		//���ɷָ�ָ��
		str = str + "zpopfd\n";
		str = str + "z" + constn + memoptreg[1][y] + ",word ptr[" + memoptreg[2][x] + "]" + "\n";
		str = str + "zpushfd\n";

		str = str + "pop dword ptr[edi+0x20]\n";
	}

	str = str + "add esp,8\n";
	str = str + "push " + memoptreg[1][y] + "\n";
	printf("%s\n", str.GetString());
	return str;
}
//���ڴ� ������32λ
CString vReadMemDs4(char* constn, char* dst)
{
	int x = 0; int y = 0;
	MemOptReg(x, y);

	CString str = "mov ";
	str = str + memoptreg[2][x] + ",dword ptr[esp+4]\n";
	str = str + "mov " + memoptreg[2][y] + ",dword ptr[esp]\n";
	int i = 0;
	for (; i < CHAGEREG; i++)
	{
		if (0 == _stricmp(chagereg[i], constn))
		{
			break;
		}
	}
	if (i == CHAGEREG)
	{
		str = str + constn + memoptreg[2][y] + ",dword ptr[" + memoptreg[2][x] + "]" + "\n";
	}
	else
	{
		str = str + "push dword ptr[edi + 0x20]\n";

		//���ɷָ�ָ��
		str = str + "zpopfd\n";
		str = str + "z" + constn + memoptreg[2][y] + ",dword ptr[" + memoptreg[2][x] + "]" + "\n";
		str = str + "zpushfd\n";

		str = str + "pop dword ptr[edi+0x20]\n";
	}
	
	str = str + "add esp,8\n";
	str = str + "push " + memoptreg[2][y] + "\n";
	printf("%s\n", str.GetString());
	return str;
}

//д�ڴ� ������8λ
CString vWriteMemDs1(char* constn, char* dst)
{
	int x = 0; int y = 0;
	MemOptReg(x, y);

	CString str = "mov ";
	str = str + memoptreg[2][x] + ",dword ptr[esp+4]\n";
	str = str + "mov " + memoptreg[2][y] + ",dword ptr[esp]\n";

	int i = 0;
	for (; i < CHAGEREG; i++)
	{
		if (0 == _stricmp(chagereg[i], constn))
		{
			break;
		}
	}
	if (i == CHAGEREG)
	{
		str = str + constn + "byte ptr[" + memoptreg[2][y] + "]," + memoptreg[0][x] + "\n";
	}
	else
	{
		str = str + "push dword ptr[edi + 0x20]\n";

		//���ɷָ�ָ��
		str = str + "zpopfd\n";		
		str = str + "z"+ constn  + "byte ptr[" + memoptreg[2][y] + "]," + memoptreg[0][x] + "\n";
		str = str + "zpushfd\n";

		str = str + "pop dword ptr[edi+0x20]\n";
	}
	
	str = str + "add esp,8\n";
	printf("%s\n", str.GetString());
	return str;
}
//д�ڴ� ������16λ
CString vWriteMemDs2(char* constn, char* dst)
{
	int x = 0; int y = 0;
	MemOptReg(x, y);
	
	CString str = "mov ";
	str = str + memoptreg[2][x] + ",dword ptr[esp+4]\n";
	str = str + "mov " + memoptreg[2][y] + ",dword ptr[esp]\n";


	int i = 0;
	for (; i < CHAGEREG; i++)
	{
		if (0 == _stricmp(chagereg[i], constn))
		{
			break;
		}
	}
	if (i == CHAGEREG)
	{
		//str = str + constn + "dword ptr[" + memoptreg[2][y] + "]," + memoptreg[2][x] + "\n";
		str = str + constn + "word ptr[" + memoptreg[2][y] + "]," + memoptreg[1][x] + "\n";
	}
	else
	{
		str = str + "push dword ptr[edi + 0x20]\n";

		//���ɷָ�ָ��
		str = str + "zpopfd\n";
		//str = str + "z" + constn + "dword ptr[" + memoptreg[2][y] + "]," + memoptreg[2][x] + "\n";
		str = str + "z"+constn + "word ptr[" + memoptreg[2][y] + "]," + memoptreg[1][x] + "\n";
		str = str + "zpushfd\n";

		str = str + "pop dword ptr[edi+0x20]\n";
	}



	str = str + "add esp,8\n";
	printf("%s\n", str.GetString());
	return str;
}
//д�ڴ� ������32λ
CString vWriteMemDs4(char* constn,char*dst)
{//dst������ʱû��
	int x = 0; int y = 0;
	MemOptReg(x, y);

	CString str = "mov ";
	str = str + memoptreg[2][x] + ",dword ptr[esp+4]\n";
	str = str + "mov "+memoptreg[2][y] + ",dword ptr[esp]\n";
	int i = 0;
	for (; i < CHAGEREG; i++)
	{
		if (0 == _stricmp(chagereg[i], constn))
		{
			break;
		}
	}
	if (i == CHAGEREG)
	{
		str = str + constn + "dword ptr[" + memoptreg[2][y] + "]," + memoptreg[2][x] + "\n";
	}
	else
	{
		str = str + "push dword ptr[edi + 0x20]\n";

		//���ɷָ�ָ��
		str = str + "zpopfd\n";
		str = str + "z"+constn + "dword ptr[" + memoptreg[2][y] + "]," + memoptreg[2][x] + "\n";		
		str = str + "zpushfd\n";

		str = str + "pop dword ptr[edi+0x20]\n";
	}
	str = str + "add esp,8\n";
	printf("%s\n", str.GetString());
	return str;
}


//���pop�ǲ����ĸ��ֽڵ��ڴ�
CString vPopWirteMem4(char* constn, char* dst)
{
	int x = 0; int y = 0;
	MemOptReg(x, y);
	CString str = "mov ";
	str = str + memoptreg[2][x] + ",dword ptr[esp]\n";
	str = str + constn + "dword ptr[" + memoptreg[2][x] + "]\n";
	return str;
}

//���CALLָ���Ǽ����ת(����call dword ptr[eax])
CString vCallMem(char* constn, char* dst)
{	
	int x = 0; int y = 0;
	MemOptReg(x, y);

	CString str ="";
	str = str + "pop "+memoptreg[2][x] + "\n";
	str = str + "mov "+memoptreg[2][y] + ",dword ptr["+ memoptreg[2][x] +"]\n";
	str = str + "xor " + memoptreg[2][y] + "," + dataencrypt + "\n";
	str = str + "sub ebp,4\n";
	str = str + "mov dword ptr[ebp]," + memoptreg[2][y] + "\n";
	return str;
}

//���CALLָ������ǼĴ���(���� call eax)
CString vCallReg(char* constn, char* dst)
{
	int x = 0; int y = 0;
	MemOptReg(x, y);

	CString str = "";
	str = str + "pop " + memoptreg[2][x] + "\n";
	str = str + "xor " + memoptreg[2][x] + "," + dataencrypt + "\n";
	str = str + "sub ebp,4\n";
	str = str + "mov dword ptr[ebp]," + memoptreg[2][x] + "\n";

	return str;
}

//�˳������
CString vRETN(char* VR0, char* VR1)
{	
	CString str = "zmov ";
	str = str + VR0 + ",dword ptr[esp]\n";

	str = str + "zadd esp, 4\n";
	str = str + "zmov "+ VR1 +", dword ptr[edi+0x00]\n";
	str = str +	"zadd "+ VR1 +", "+ VR0 +"\n" ;
	str = str + "zpush dword ptr[edi+0x00]\n";
	str = str + "zpush "+ VR1 +"\n";

	str = str + "zpush [edi + 0x20]\n";//���ѹ������Ĵ�����ֵ
	str = str + "zpush [edi + 0x1C]\n";
	str = str + "zpush [edi + 0x18]\n";
	str = str + "zpush [edi + 0x14]\n";
	str = str + "zpush [edi + 0x10]\n";
	str = str + "zpush [edi + 0xC]\n";
	str = str + "zpush [edi + 0x8]\n";
	str = str + "zpush [edi + 0x4]\n";

	//���ɷָ�ָ��
	str = str + "zpop eax\n";
	str = str + "zpop ecx\n";
	str = str + "zpop edx\n";
	str = str + "zpop ebx\n";
	str = str + "zpop ebp\n";
	str = str + "zpop esi\n";
	str = str + "zpop edi\n";
	str = str + "zpopfd\n";

	str = str + "zpop ecx\n";
	str = str + "zpop edx\n";
	str = str + "zmov esp,ecx\n";

	
	str = str + "zadd esp,4\n";//��ԭespջ
	str = str + "zxor ecx,ecx\n";
	str = str + "zpush dword ptr[edx]\n";//ѹ�뷵�ص�ַ
	str = str + "zretn\n";
	
	return str;
}

//����ģ��ָ���
CString vNotSimulate(char* instruction,char* vresumestartaddr)
{
	CString strrepl = instruction;
	string sins(instruction);
	string newstr= strrepl.replace(sins, "h]", "]");

	CString str = "zpop ecx\n";
	str = str + "zpop edx\n";
	str = str + "zpop eax\n";		
	str = str +"z"+ newstr.c_str()+"\n";
	str = str + "zjmp " + g_vmstruct.resume_esiandebpaddr + "\n";
	return str;
}

//callָ�����ģ��ָ��
CString vCALL(char* esistateaddr, char* vresumestartaddr)
{

	DWORD nextcall = g_vmstruct.ebpstateaddr + 4;
	CString str = "zmov eax,";
	str = str + g_vmstruct.ebpstateaddr + "\n";

	str = str + "zmov edx,eax\n";
	str = str + "zmov eax,[eax]\n";
	str = str + "zmov ecx,eax\n";
	str = str + "zmov eax,[eax]\n";
	str = str + "zxor eax," + dataencrypt + "\n";
	str = str + "zmov [edx+4],eax\n";
	str = str + "zadd ecx,4\n";
	str = str + "zmov [edx],ecx\n";
	str = str + "zxor ecx,ecx\n";
	str = str + "zpop ecx\n";
	str = str + "zpop edx\n";
	str = str + "zpop eax\n";
	str = str + "zpush " + g_vmstruct.resume_esiandebpaddr + "\n";
	str = str + "zjmp dword ptr [" + nextcall + "]\n";
	return str;
}


//ִ�в���ģ��ָ���,�ٴν��������1
CString vResumeStart_1(char* instruction, char* vresumestartaddr)
{
	CString str = "zpush esp\n";
	str = str + "zpush eax\n";
	str = str + "zpush ecx\n";
	str = str + "zpush edx\n";
	str = str + "zpush ebx\n";
	str = str + "zpush ebp\n";
	str = str + "zpush esi\n";
	str = str + "zpush edi\n";
	str = str + "zpushfd\n";

	str = str + "zmov ebp,esp\n";
	str = str + "zsub esp,0x200\n";
	str = str + "zmov edi,esp\n";
	str = str + "zsub esp,0x40\n";
	str = str + "zmov eax," + g_vmstruct.esistateaddr + "\n";//�ҵ�����esi��״̬�ǿ��ڴ�
	str = str + "zmov esi,[eax]\n";//�ָ�
	str = str + "zjmp " + g_vmstruct.dispatcheraddr + "\n";
	return str;
}
//ִ�в���ģ��ָ���,�ٴν��������2
CString vResumeStart_2(char* VR0, char* VR1)
{
	CString str = "";	
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x20]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x1c]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x18]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x14]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x10]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0xc]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x8]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x4]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x00]," + VR0 + "\n"; 
	str = str + "add ebp,4\n";

	str = str + "mov " + VR1 + "," + g_vmstruct.ebpstateaddr + "\n";//�ҵ�����ebp��״̬�ǿ��ڴ�
	str = str + "mov ebp,[" + VR1 + "]\n";//�ָ�
	str = str + "jmp " + g_vmstruct.dispatcheraddr + "\n";//��ת��������
	return str;
}


//���������1
CString VMStartVM(char* instruction, char* vresumestartaddr)
{

	CString str = "";
	str = str + "zmov eax," + g_vmstruct.esistateaddr + "\n";
	str = str + "zmov dword ptr[eax],esi\n";
	str = str + "zpopfd\n";
	str = str + "zmov eax,esi\n";
	str = str + "zpopad\n";
	str = str + "zpush esp\n";
	str = str + "zpush eax\n";
	str = str + "zpush ecx\n";
	str = str + "zpush edx\n";
	str = str + "zpush ebx\n";
	str = str + "zpush ebp\n";
	str = str + "zpush esi\n";
	str = str + "zpush edi\n";
	str = str + "zpushfd\n";

	str = str + "zmov ebp,esp\n";
	str = str + "zsub esp,0x200\n";
	str = str + "zmov edi,esp\n";
	str = str + "zsub esp,0x40\n";
	str = str + "zmov esi,dword ptr[" + g_vmstruct.esistateaddr + "]\n";//��ת���ַ
	str = str + "zjmp " + g_vmstruct.dispatcheraddr + "\n";//��ת��������
	return str;
}
//���������2
CString VMStartVM_2(char* VR0, char* VR1)
{
	CString str = "";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x20]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x1c]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x18]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x14]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x10]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0xc]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x8]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x4]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov " + VR0 + ",dword ptr[ebp]\n";
	str = str + "mov dword ptr[edi+0x00]," + VR0 + "\n";
	str = str + "add ebp,4\n";
	str = str + "mov ebp," + g_vmstruct.datatbladdr + "\n";//���ݱ��ַ
	str = str + "jmp " + g_vmstruct.dispatcheraddr + "\n";//��ת��������
	return str;
}


//VStartVM ���������
void __declspec(naked) VStartVM()
{
	__asm {
		push esp
		push eax
		push ecx
		push edx
		push ebx
		push ebp
		push esi
		push edi
		pushfd

		mov ebp, esp			//ebp���Ƕ�ջ��
		sub esp, 0x200
		mov edi, esp			//edi����VMContext
		sub esp, 0x40			//��ʱ��esp����VMʹ�õ�ջ

		//vBegin
		mov		eax, dword ptr[ebp]	//v_efl
		mov[edi + 0x20], eax
		add		ebp, 4
		mov		eax, dword ptr[ebp]	//v_edi
		mov[edi + 0x1C], eax
		add		ebp, 4
		mov		eax, dword ptr[ebp]	//v_esi
		mov		[edi + 0x18], eax
		add		ebp, 4
		mov		eax, dword ptr[ebp]	//e_ebp
		mov		[edi + 0x14], eax
		add		ebp, 4
		mov		eax, dword ptr[ebp] //e_ebx 
		mov		[edi + 0x10], eax
		add		ebp, 4
		mov		eax, dword ptr[ebp]
		mov		[edi + 0xC], eax	//v_edx
		add		ebp, 4
		mov		eax, dword ptr[ebp]
		mov		[edi + 0x8], eax	//v_ecx
		add		ebp, 4
		mov		eax, dword ptr[ebp]
		mov		[edi + 0x04], eax	//v_eax
		add		ebp, 4
		mov		eax, dword ptr[ebp]
		mov		[edi + 0x00], eax	//v_esp
		add		ebp, 4

		mov		ebp, 0x88888888		//���ݱ��ַ
		mov		esi, 0x88888888		//��ת��ĵ�ַ
		mov		eax, 0x88888888		//��������ַ
		//push	eax
		//ret
		jmp		eax
		VM_END    //VM�������
	}
}


//���VMContext�Ƿ񱻸���
void __declspec(naked) VCheckESP()
{
	__asm {
		lea eax, dword ptr[edi + 0x100]
		mov ebx, dword ptr[edi + 0x00]//[edi+0x00]��esp��vm��Լ����ֵ
		cmp eax, ebx		//�Ƚ�
		jl  VMDispatcher_	//С�������ִ��

		mov edx, eax			//����
		mov eax, dword ptr[edi + 0x14] //[edi+0x00]��ebp��vm��Լ����ֵ
		sub eax, ebx

		mov ecx, edx
		sub ecx, esp

		push esi  //����IPָ��
		mov esi, esp
		sub esp, eax //����ebp-esp��ֵ��������esp��ȥ����
		mov edi, esp
		push edi		//�����µ�edi����ַ

		cld
		rep movsb   //����
		pop edi
		pop esi
		add edi, 0x44
		VMDispatcher_:
		mov eax, 0x88888888
			jmp eax

			VM_END   //�������
	}
}


//vRetnNOT �˳��������ִ�в���ģ��ָ��
CString vRetnNOT_(char* VR0, char* VR1)
{
	CString str = "push dword ptr[edi+0x00]\n";	
	str = str + "push dword ptr[edi+0x20]\n";
	str = str + "push dword ptr[edi+0x1c]\n";
	str = str + "push dword ptr[edi+0x18]\n";
	str = str + "push dword ptr[edi+0x14]\n";
	str = str + "push dword ptr[edi+0x10]\n";
	str = str + "push dword ptr[edi+0xc]\n";
	str = str + "push dword ptr[edi+0x8]\n";
	str = str + "push dword ptr[edi+0x4]\n";

	//���ɷָ�ָ��
	str = str + "zmov " + VR0 + "," + g_vmstruct.esistateaddr + "\n";
	str = str + "zmov " + "dword ptr[" + VR0 + "],esi\n";
	str = str +	"zmov " + VR1 + "," + g_vmstruct.ebpstateaddr + "\n";
	str = str + "zmov " + "dword ptr[" + VR1 + "],ebp\n";
	str = str + "zpop eax\n";
	str = str + "zpop ecx\n";
	str = str + "zpop edx\n";
	str = str + "zpop ebx\n";
	str = str + "zpop ebp\n";
	str = str + "zpop esi\n";
	str = str + "zpop edi\n";
	str = str + "zpopfd\n";
	str = str + "zpop esp\n";
	str = str + "zpush eax\n";
	str = str + "zpush edx\n";
	str = str + "zpush ecx\n";
	str = str + "zmov eax," + g_vmstruct.esistateaddr + "\n";
	str = str + "zmov edx,eax\n";
	str = str + "zmov eax,dword ptr[eax]\n";
	str = str + "zmov ecx,eax\n";
	str = str + "zadd ecx,4\n";
	str = str + "zmov dword [edx],ecx\n";
	str = str + "zpush dword ptr[eax]\n";
	str = str + "zret\n";
	return str;
}



//ִ�в���ģ��ָ���,�ٴν��������
void __declspec(naked) vResumeStart()
{
	__asm {
		push esp
		push eax
		push ecx
		push edx
		push ebx
		push ebp
		push esi
		push edi
		pushfd

		mov ebp, esp			//ebp���Ƕ�ջ��
		sub esp, 0x200
		mov edi, esp      //edi����VMContext
		sub esp, 0x40	//��ʱ��esp����VMʹ�õ�ջ

		//vBegin
		mov		eax, dword ptr[ebp]	//v_efl
		mov		[edi + 0x20], eax
		add		ebp, 4
		mov		eax, dword ptr[ebp]	//v_edi
		mov		[edi + 0x1C], eax
		add		ebp, 4
		mov		eax, dword ptr[ebp]	//v_esi
		mov		[edi + 0x18], eax
		add		ebp, 4
		mov		eax, dword ptr[ebp]	//e_ebp
		mov		[edi + 0x14], eax
		add		ebp, 4
		mov		eax, dword ptr[ebp] //e_ebx 
		mov		[edi + 0x10], eax
		add		ebp, 4
		mov		eax, dword ptr[ebp]
		mov		[edi + 0xC], eax	//v_edx
		add		ebp, 4
		mov		eax, dword ptr[ebp]
		mov		[edi + 0x8], eax	//v_ecx
		add		ebp, 4
		mov		eax, dword ptr[ebp]
		mov		[edi + 0x04], eax	//v_eax
		add		ebp, 4
		mov		eax, dword ptr[ebp]
		mov		[edi + 0x00], eax	//v_esp
		add		ebp, 4

		mov		eax, 0x88886666		//�ҵ�����ebp��״̬�ǿ��ڴ�
		mov		ebp, [eax]			//�ָ�

		mov		eax,0x66668888		//�ҵ�����esi��״̬�ǿ��ڴ�
		mov		esi, [eax]			//�ָ�


		mov		eax, 0x88888888		//��ת��vCheckESP()�������ж�esp�Ƿ�Ӱ����VMContext�ṹ
		jmp		eax
		VM_END    //VM�������
	}
}

//������
void __declspec(naked) VMDispatcher()
{	
	__asm {
		push ecx
		mov ecx, dword ptr[esi]
		add esi,4
		mov dword ptr[edi-0x34],ecx
		pop ecx
		push dword ptr[edi - 0x34]
		ret
		VM_END    //VM�������
	}
}

//��ú�������
int GetFunSize(FUNC Func)
{
	char TargetCode[] = { 0x56,0x4D,0x45,0x4E,0x44 };
	CString str;
	char* findaddr = (char*)str.Find_Memory_Bytes((char*)Func, TargetCode, 0x100, sizeof(TargetCode));

	int nLen = findaddr - (char*)Func;
	return nLen;
}

//��ת�����������ʼ��ַ
void __declspec(naked) JmpToVMStart()
{
	__asm {
		pushad
		pushfd
		mov esi,0x66668888
		mov edi,esp
		mov ebp, 0x88888888
		//jmp eax
		push ebp
		ret
		VM_END		//�������
	}
}
