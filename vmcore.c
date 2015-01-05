/*
	Pulse Virtual Machine (c) 2015, Dimitris Tsiktsiris
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include "optable.h"

#if __SIZEOF_LONG__ == 4
#define LONG
#elif __SIZEOF_LONG__ == 8
#define LONG 0x48,
#else
#error "Unsupported bit size"
#endif

double get_time()
{
    LARGE_INTEGER t, f;
    QueryPerformanceCounter(&t);
    QueryPerformanceFrequency(&f);
    return (double)t.QuadPart/(double)f.QuadPart;
}


int test_func(char *  str)
{
	printf("%s\n",str);
	return 4;
}

int vm(instr *i)
{
	register int r0 asm ("ebx");

	int length, *m;
	char *pn, *pr;
	instr *ip;

	for (m = 0, ip = i; ; ip++)
	{
		ip->len = optable[ip->op].size;
		ip->pos = length;
		length += ip->len;

		if (ip->op == _end) break;
	}

	if (!(pn = VirtualAlloc(NULL, length, MEM_COMMIT, PAGE_EXECUTE_READWRITE))) return 0;

	for (pr = pn, ip = i;; ip++)
	{
		op = &optable[ip->op];
		memcpy(pr, op->data, ip->len);

		if (op->arg)
		{
			m = (int*)(pr + op->arg);
			*m = ip->arg;
		}

		if (op->jmp)
		{
			m = (int*)(pr + op->jmp);
			*m = (i[ip->arg].pos - ip->pos) - op->size;
		}

		pr += ip->len;

		if (ip->op == _end) break;
	}

	((void(*)())pn)();

	return 0;
}

int main(int argc, char **argv)
{
	instr program[] =
	{
		{_ldi,     1},
		{_st0		},
		{_ldi,1000000000},
		{_st1		},
		{_ldi,     1},
		{_st2		},
		{_ld2		},
		{_ld0		},
		{_add		},
		{_st0		},
		{_ld0		},
		{_ld1		},
		{_cmp		},
		{_blt,     4},
		{_ldi,       (size_t)"Hello world!"},
		{_ldi,       (size_t)&test_func },
		{_cal       },
		{_st0		},
		{_ld0    	},
		{_ldi,       (size_t)"function returned %d\n"},
		{_ldi,       (size_t)&printf },
		{_cal       },
		{_st3		},
		{_st0		},
		{_end       },
	};

	double start = get_time();

	vm(program);

    printf("\nFinished in  %f ms\n", get_time() - start);

    system("PAUSE");
	return 0;
}
