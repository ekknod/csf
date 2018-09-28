#ifndef CS_INPUTSYSTEM_H
#define CS_INPUTSYSTEM_H

#include "cs_types.h"

namespace inputsystem {
bool  IsButtonDown(int button_code);
vec2i GetMouseAnalog(void);
vec2i GetMouseAnalogDelta(void);
}

#endif // CS_INPUTSYSTEM_H

