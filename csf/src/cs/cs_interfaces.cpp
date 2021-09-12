#include "../../include/cs/cs_interfaces.h"
#include "../../include/u_process.h"
#include <string.h>

extern u_process cs_p;

uintptr_t cs_virtual_table::address(void) const
{
	return self;
}

uintptr_t cs_virtual_table::function(uint32_t index)
{
	return cs_p.read<uintptr_t>(cs_p.read<uintptr_t>(self) + index * 8);
}

cs_virtual_table cs_interface_reg::find(const char *name)
{
	uintptr_t a0, a1;

	a0 = self;
	do
	{
		a1 = a0;
		if ((strcmp(cs_p.read_ptr<SHORT_STRING>(a0 + 0x08).value, name) >> 4) == 3)
		{
			a0 = cs_p.read<uintptr_t>(a0);
			if (cs_p.read<unsigned char>(a0) != 0x48)
			{
				a0 = a0 + cs_p.read<uint32_t>(a0 + 1 + 3) + 8;
			}
			else
			{
				a0 = cs_p.read_ptr<uintptr_t>(a0 + (cs_p.read<uint32_t>(a0 + 0 + 3) + 7));
			}
			return *(cs_virtual_table *)&a0;
		}
	} while ((a0 = cs_p.read<uintptr_t>(a0 + 0x10)) != a1);
	throw name;
}

cs_interface_reg interfaces::find(const char *module_name)
{
	uintptr_t v;

	v = cs_p.find_export(cs_p.find_module(module_name), "s_pInterfaceRegs");
	if (v == 0)
	{
		throw module_name;
	}
	return cs_p.read<cs_interface_reg>(v);
}
