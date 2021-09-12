#include "../../include/cs/cs_netvars.h"
#include "../../include/u_process.h"
#include "../../include/cs/cs_interfaces.h"
#include <string.h>

extern u_process cs_p;
extern cs_virtual_table cs_vt_client;

uint32_t cs_netvar_table::offset(const char *name)
{
	return offset_ex(self, name);
}

uint32_t cs_netvar_table::offset_ex(uintptr_t address, const char *name)
{
	uint32_t a0;
	uint32_t a1;
	uintptr_t a2;
	uint32_t a3;
	uintptr_t a4;
	uint32_t a5;

	a0 = 0;
	for (a1 = cs_p.read<uint32_t>(address + 0x8); a1--;)
	{
		a2 = a1 * 96 + cs_p.read<uintptr_t>(address);
		a3 = cs_p.read<uint32_t>(a2 + 0x48);
		if ((a4 = cs_p.read<uintptr_t>(a2 + 0x40)) && cs_p.read<uint32_t>(a4 + 0x8))
		{
			if ((a5 = offset_ex(a4, name)))
			{
				a0 += a3 + a5;
			}
		}
		if (strcmp(cs_p.read_ptr<SHORT_STRING>(a2).value, name) == 0)
		{
			return a3 + a0;
		}
	}
	return a0;
}

cs_netvar_table netvars::find(const char *name)
{
	uintptr_t a0, a1;

	a0 = cs_vt_client.function(8);
	a0 = cs_p.read_ptr<uintptr_t>(a0 + cs_p.read<uint32_t>(a0 + 0 + 3) + 7);
	do
	{
		a1 = cs_p.read<uintptr_t>(a0 + 0x18);
		if (strcmp(cs_p.read_ptr<SHORT_STRING>(a1 + 0x18).value, name) == 0)
		{
			return *(cs_netvar_table *)&a1;
		}
	} while ((a0 = cs_p.read<uintptr_t>(a0 + 0x20)));
	throw name;
}

/*
 *  0:  48 8b 05 69 77 71 01    mov    rax,QWORD PTR [rip+0x1717769] # 0x1717770
 *  7:  55                      push   rbp
 *  8:  48 89 e5                mov    rbp,rsp
 *  b:  5d                      pop    rbp
 *  c:  48 8b 00                mov    rax,QWORD PTR [rax]
 *  f:  c3                      ret
 *
 */
