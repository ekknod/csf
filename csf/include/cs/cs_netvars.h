#ifndef CS_NETVARS_H
#define CS_NETVARS_H

#include <inttypes.h>

class cs_netvar_table {
    uintptr_t self;
public:
    uint32_t offset(const char *name);
private:
    uint32_t offset_ex(uintptr_t, const char *);
} ;

namespace netvars {
cs_netvar_table find(const char *name);
}

#endif // CS_NETVARS_H

