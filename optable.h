#ifndef _OPTABLE_H
#define _OPTABLE_H

typedef struct instr {
	int32_t op, arg, len, pos;
} instr, *X86_p;

enum
{
	_nop = 0,	_ldi,		_ld0,
	_ld1,		_ld2,		_ld3,
	_st0,		_st1,		_st2,
	_st3,		_add,		_sub,
	_mul,		_div,		_mod,
	_orr,		_xor,		_and,
	_neg,		_cmp,		_beq,
	_bne,		_bgt,		_blt,
	_bge,		_ble,		_cal,
	_jmp,		_ret,		_end
};

#if __SIZEOF_LONG__ == 4
#define LONG
#elif __SIZEOF_LONG__ == 8
#define LONG 0x48,
#else
#error "Unsupported bit size"
#endif


#define X86(op, pad, arg, jmp, bytes...) [op]={ sizeof((char[]){ bytes }) + pad, arg, jmp, { bytes } }

struct
{
	int32_t size, arg, jmp;
	char data[16];
}


*op, optable[] =
{
	X86(_nop, 0, 0, 0, 0x90), //nop
	X86(_ldi, 4, 1, 0, 0x68), //push   
	X86(_ld0, 0, 0, 0, 0x53), //push   ebx
	X86(_ld1, 0, 0, 0, 0x56), //push   esi
	X86(_ld2, 0, 0, 0, 0x57), //push   edi
	X86(_ld3, 0, 0, 0, 0x51), //push   ecx
	X86(_st0, 0, 0, 0, 0x5b), //pop    ebx
	X86(_st1, 0, 0, 0, 0x5e), //pop    esi
	X86(_st2, 0, 0, 0, 0x5f), //pop    edi
	X86(_st3, 0, 0, 0, 0x59), //pop    ecx

	/*
		58		pop    eax
		010424	add    DWORD PTR [esp],eax
	*/
	X86(_add, 0, 0, 0, 0x58, LONG 0x01, 0x04, 0x24),


	/*
		58		pop    eax
		290424	sub    DWORD PTR [esp],eax
	*/
	X86(_sub, 0, 0, 0, 0x58, LONG 0x29, 0x04, 0x24),

	/*
		5a		pop    edx
	  	8b0424	mov    eax,DWORD PTR [esp]
	  	0fafc2	imul   eax,edx
	  	890424	mov    DWORD PTR [esp],eax
	*/
	X86(_mul, 0, 0, 0, 0x5a, LONG 0x8b, 0x04, 0x24, LONG 0x0f,0xaf, 0xc2, LONG 0x89, 0x04, 0x24),

	/*
		5a		pop    edx
  			89d0	mov    eax,edx
  			c1fa1f	sar    edx,0x1f
  			f73c24	idiv   DWORD PTR [esp]
  			890424	mov    DWORD PTR [esp],eax
	*/
	X86(_div, 0, 0, 0, 0x5a, LONG 0x89, 0xd0, LONG 0xc1, 0xfa,0x1f, LONG 0xf7, 0x3c, 0x24, LONG 0x89, 0x04, 0x24),

	/*
		5a		pop    edx
  			89d0	mov    eax,edx
  			c1fa1f	sar    edx,0x1f
  			f73c24	idiv   DWORD PTR [esp]
  			890424	mov    DWORD PTR [esp],edx
	*/
	X86(_mod, 0, 0, 0, 0x5a, LONG 0x89, 0xd0, LONG 0xc1, 0xfa,0x1f, LONG 0xf7, 0x3c, 0x24, LONG 0x89, 0x14, 0x24),

	/*
		58		pop    eax
		090424	or     DWORD PTR [esp],eax
	*/
	X86(_orr, 0, 0, 0, 0x58, LONG 0x09, 0x04, 0x24),

	/*
		58		pop    eax
		310424	xor    DWORD PTR [esp],eax
	*/
	X86(_xor, 0, 0, 0, 0x58, LONG 0x31, 0x04, 0x24),

	/*
		58		pop    eax
		210424	and    DWORD PTR [esp],eax
	*/
	X86(_and, 0, 0, 0, 0x58, LONG 0x21, 0x04, 0x24),

	/*
		58		pop    eax
		f7d8	neg    eax
		50		push   eax
	*/
	X86(_neg, 0, 0, 0, 0x58, LONG 0xf7, 0xd8, 0x50),

	/*
		58		pop    eax
		5a		pop    edx
		39c2	cmp    edx,eax
	*/
	X86(_cmp, 0, 0, 0, 0x58, 0x5a, LONG 0x39, 0xc2),
	X86(_beq, 4, 0, 2, 0x0f, 0x84), //je
	X86(_bne, 4, 0, 2, 0x0f, 0x85), //jne
	X86(_bgt, 4, 0, 2, 0x0f, 0x8f), //jg
	X86(_blt, 4, 0, 2, 0x0f, 0x8c), //jl
	X86(_bge, 4, 0, 2, 0x0f, 0x8d), //jge
	X86(_ble, 4, 0, 2, 0x0f, 0x8e), //jle
	X86(_cal, 0, 0, 0, 0x58,0xff,0xd0, LONG 0x89, 0x04, 0x24),
	X86(_jmp, 4, 0, 1, 0xe9), //jmp
	X86(_ret, 0, 0, 0, 0xc3), //ret
	X86(_end, 0, 0, 0, 0xc3),
};
#endif // _OPTABLE_H
