0054782B | 8B4D 00                  | mov ecx,dword ptr ss:[ebp]              |
0054782E | E9 CD67FDFF              
00547833 | E9 C867FDFF              
00547838 | 894F 20                  | mov dword ptr ds:[edi+20],ecx           | eflag
00547840 | E9 BB67FDFF              
00547845 | 83C5 04                  | add ebp,4                               |
00547848 | E9 B367FDFF              
0054784D | E9 AE67FDFF              
00547852 | 8B4D 00                  | mov ecx,dword ptr ss:[ebp]              |
00547855 | E9 A667FDFF              
0054785A | E9 A167FDFF              
0054785F | 894F 1C                  | mov dword ptr ds:[edi+1C],ecx           | edi
00547862 | E9 9967FDFF              
00547867 | E9 9467FDFF              
0054786C | 83C5 04                  | add ebp,4                               |
0054786F | E9 8C67FDFF              
00547874 | E9 8767FDFF              
00547879 | 8B4D 00                  | mov ecx,dword ptr ss:[ebp]              |
0054787C | E9 7F67FDFF              
00547881 | E9 7A67FDFF              
00547886 | 894F 18                  | mov dword ptr ds:[edi+18],ecx           | esi
00547889 | E9 7267FDFF              
0054788E | E9 6D67FDFF              
00547893 | 83C5 04                  | add ebp,4                               |
00547896 | E9 6567FDFF              
0054789B | E9 6067FDFF              
005478A0 | 8B4D 00                  | mov ecx,dword ptr ss:[ebp]              |
005478A3 | E9 5867FDFF              
005478A8 | E9 5367FDFF              
005478AD | 894F 14                  | mov dword ptr ds:[edi+14],ecx           |ebp
005478B0 | E9 4B67FDFF              
005478B5 | E9 4667FDFF              
005478BA | 83C5 04                  | add ebp,4                               |
005478BD | E9 3E67FDFF              
005478C2 | E9 3967FDFF              
005478C7 | 8B4D 00                  | mov ecx,dword ptr ss:[ebp]              |
005478CA | E9 3167FDFF              
005478CF | E9 2C67FDFF              
005478D4 | 894F 10                  | mov dword ptr ds:[edi+10],ecx           |ebx
005478D7 | E9 2467FDFF              
005478DC | E9 1F67FDFF              
005478E1 | 83C5 04                  | add ebp,4                               |
005478E4 | E9 1767FDFF              
005478E9 | E9 1267FDFF              
005478EE | 8B4D 00                  | mov ecx,dword ptr ss:[ebp]              |
005478F1 | E9 0A67FDFF              
005478F6 | E9 0567FDFF              
005478FB | 894F 0C                  | mov dword ptr ds:[edi+C],ecx            |edx
005478FE | E9 FD66FDFF              
00547903 | E9 F866FDFF              
00547908 | 83C5 04                  | add ebp,4                               |
0054790B | E9 F066FDFF              
00547910 | E9 EB66FDFF              
00547915 | 8B4D 00                  | mov ecx,dword ptr ss:[ebp]              |
00547918 | E9 E366FDFF              
0054791D | E9 DE66FDFF              
00547922 | 894F 08                  | mov dword ptr ds:[edi+8],ecx            | ecx
00547925 | E9 D666FDFF              
0054792A | E9 D166FDFF              
0054792F | 83C5 04                  | add ebp,4                               |
00547932 | E9 C966FDFF              
00547937 | E9 C466FDFF              
0054793C | 8B4D 00                  | mov ecx,dword ptr ss:[ebp]              |
0054793F | E9 BC66FDFF              
00547944 | E9 B766FDFF              
00547949 | 894F 04                  | mov dword ptr ds:[edi+4],ecx            |eax
0054794C | E9 AF66FDFF              
00547951 | E9 AA66FDFF              
00547956 | 83C5 04                  | add ebp,4                               |
00547959 | E9 A266FDFF              
0054795E | E9 9D66FDFF              
00547963 | 8B4D 00                  | mov ecx,dword ptr ss:[ebp]              |
00547966 | E9 9566FDFF              
0054796B | E9 9066FDFF              
00547970 | 894F 00                  | mov dword ptr ds:[edi],ecx              |esp
00547973 | E9 8866FDFF              
00547978 | E9 8366FDFF              
0054797D | 83C5 04                  | add ebp,4                               |
00547980 | E9 7B66FDFF              
00547985 | E9 7666FDFF              
0054798A | BD 00F05100              | mov ebp,vmprotect - 副本_vmp.51F000       |
0054798F | E9 6C66FDFF              
00547994 | E9 6766FDFF              
00547999 | E9 6266FDFF              
0054799E | E9 5D66FDFF              
005479A3 | E9 5866FDFF              
005479A8 | 8B1424                   | mov edx,dword ptr ss:[esp]              |
005479AB | E9 5066FDFF              
005479B0 | E9 4B66FDFF              
005479B5 | 8B47 00                  | mov eax,dword ptr ds:[edi]              | 获取vm_esp
005479B8 | E9 4366FDFF              
005479BD | E9 3E66FDFF              
005479C2 | 83E8 04                  | sub eax,4                               |vm_esp-4
005479C5 | E9 3666FDFF              
005479CA | E9 3166FDFF              
005479CF | 8910                     | mov dword ptr ds:[eax],edx              |保存[esp]->[vm_esp-4]
005479D1 | E9 2A66FDFF              
005479D6 | E9 2566FDFF              
005479DB | 836F 00 04               | sub dword ptr ds:[edi],4                |
005479DF | E9 1C66FDFF              
005479E4 | E9 1766FDFF              
005479E9 | 83C4 04                  | add esp,4                               |
005479EC | E9 0F66FDFF              
005479F1 | E9 0A66FDFF              
005479F6 | 8B1C24                   | mov ebx,dword ptr ss:[esp]              |
005479F9 | E9 0266FDFF              
005479FE | E9 FD65FDFF              
00547A03 | 8B4424 04                | mov eax,dword ptr ss:[esp+4]            |
00547A07 | E9 F465FDFF              



	CString str = "mov ";
	str = str + VR0 +",dword ptr[ebp]\n" ;
	str = str + "add ebp,4\n";
	str = str + "xor " + VR0 + "," + dataencrypt + "\n";
	str = str + "mov "+ VR0 +",dword ptr [edi+"+ VR0 +"*4]\n";
	str = str + "push "+ VR0 +"\n";


    0054798A | BD 00F05100              | mov ebp,vmprotect - 副本_vmp.51F000       |

    取数据表的地址
0054753C | 8B45 00                  | mov eax,dword ptr ss:[ebp]             



vPushReg4
0054753C | 8B45 00                  | mov eax,dword ptr ss:[ebp]              | 取数据表地址
0054753F | E9 BC6AFDFF              
00547544 | E9 B76AFDFF              
00547549 | 83C5 04                  | add ebp,4                               |
0054754C | E9 AF6AFDFF              
00547551 | E9 AA6AFDFF              
00547556 | 35 0C213603              | xor eax,336210C                         |
0054755B | E9 A06AFDFF              
00547560 | E9 9B6AFDFF              
00547565 | 8B0487                   | mov eax,dword ptr ds:[edi+eax*4]        |获取vm_ebp
00547568 | E9 936AFDFF              
0054756D | E9 8E6AFDFF              
00547572 | 50                       | push eax                                | push vm_ebp


vPush 
CString str = "mov ";
	str = str + VR0 + ",dword ptr[esp]\n";
	str = str + "mov " + VR1 + ",dword ptr[edi+0x00]\n";
	str = str + "sub " + VR1 + ",4\n";
	str = str + "mov dword ptr[" + VR1 + "]," + VR0 + "\n";
	str = str + "sub dword ptr[edi+0x00],4\n";
	str = str + "add esp,4\n";


    mov edx,dword ptr [esp] 获取vm_ebp

    mov eax,dword ptr [edi] 获取vm_esp

    sub eax,4           eax=vm_esp-4
    mov dword ptr [eax],edx [vm_esp-4]=vm_ebp
    sub dword ptr [edi],4 vm_esp-4
    add esp,4          esp+4还原堆栈