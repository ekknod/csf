#ifndef CS_TYPES_H
#define CS_TYPES_H

#include <inttypes.h>

typedef struct {
    float x, y, z;
} vec3 ;

typedef struct {
    int x, y;
} vec2i ;

typedef float matrix3x4_t[3][4];

#endif // CS_TYPES_H

