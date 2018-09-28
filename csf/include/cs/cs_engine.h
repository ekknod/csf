#ifndef CS_ENGINE_H
#define CS_ENGINE_H

#include "cs_interfaces.h"
#include "cs_netvars.h"
#include "cs_convar.h"
#include "cs_inputsystem.h"
#include "cs_player.h"

/*
 * initializes everything, must be called first
 */
bool cs_initialize(void);

namespace engine {
int       IsRunning(void);
int       GetLocalPlayer(void);
vec3      GetViewAngles(void);
void      SetViewAngles(vec3 v);
int       GetMaxClients(void);
bool      IsInGame(void);
}

namespace entity {
cs_player GetClientEntity(int index);
}

#endif // CS_ENGINE_H

