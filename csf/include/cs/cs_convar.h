#ifndef CS_CONVAR_H
#define CS_CONVAR_H

#include "cs_types.h"
#include "../../../rx/include/rx_types.h"

class cs_convar {
    uintptr_t self;
public:
    SHORT_STRING GetName(void);
    void         SetString(const char *v);
    void         SetInt(int v);
    void         SetFloat(float v);
    SHORT_STRING GetString(void);
    int          GetInt(void);
    float        GetFloat(void);
} ;

namespace cvar {

cs_convar find(const char *name);

}

#endif // CS_CONVAR_H

