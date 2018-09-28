#ifndef CS_INTERFACES_H
#define CS_INTERFACES_H

#include <inttypes.h>

class cs_virtual_table {
    uintptr_t self;
public:
    uintptr_t address(void) const;
    uintptr_t function(uint32_t index);
};

class cs_interface_reg {
    uintptr_t self;
public:
    cs_virtual_table find(const char *name);
};

namespace interfaces {
cs_interface_reg find(const char *module_name);
}

#endif // CS_INTERFACES_H
