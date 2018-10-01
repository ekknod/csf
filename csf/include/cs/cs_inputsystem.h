#ifndef CS_INPUTSYSTEM_H
#define CS_INPUTSYSTEM_H

#include "cs_types.h"
namespace inputsystem {
typedef enum _CS_BUTTONCODE : int {
    MOUSE_1 = 107,
    MOUSE_2 = 108,
    MOUSE_3 = 109,
    MOUSE_4 = 110,
    MOUSE_5 = 111
} CS_BUTTONCODE ;
bool  IsButtonDown(CS_BUTTONCODE button);
vec2i GetMouseAnalog(void);
vec2i GetMouseAnalogDelta(void);
}

#endif // CS_INPUTSYSTEM_H
