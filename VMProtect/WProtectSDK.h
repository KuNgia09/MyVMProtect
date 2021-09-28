#ifdef  __GNUC__
#define WProtectBegin() __asm__(".byte 0xEB\n\t.byte 0xf\n\t.string \"WProtect Begin\"\n\t");   
#define WProtectEnd() __asm__(".byte 0xEB\n\t.byte 0xd\n\t.string \"WProtect End\"\n\t"); 
#endif

#ifdef _MSC_VER
//_asm {
//	_emit 0xeb
//	_emit 0x0f
//	_emit 0x57
//	_emit 0x50
//	_emit 0x72
//	_emit 0x6f
//	_emit 0x74
//	_emit 0x65
//	_emit 0x63
//	_emit 0x74
//	_emit 0x20
//	_emit 0x42
//	_emit 0x65
//	_emit 0x67
//	_emit 0x69
//	_emit 0x6e
//	_emit 0x00
//
//}


#endif