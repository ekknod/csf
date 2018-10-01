#ifndef CS_PLAYER_H
#define CS_PLAYER_H

#include "cs_types.h"

class cs_player {
    uintptr_t self;
public:
    int       GetTeam(void);
    int       GetHealth(void);
    int       GetLifeState(void);
    int       GetTickCount(void);
    int       GetShotsFired(void);
    bool      IsScoped(void);
    bool      IsDormant(void);
    uintptr_t GetWeapon(void);
    vec3      GetOrigin(void);
    vec3      GetVecView(void);
    vec3      GetEyePos(void);
    vec3      GetVecVelocity(void);
    vec3      GetVecPunch(void);
    int       GetFov(void);
    void      GetBoneMatrix(int index, matrix3x4_t *out);
    bool      IsValid(void);
} ;

#endif // CS_PLAYER_H

