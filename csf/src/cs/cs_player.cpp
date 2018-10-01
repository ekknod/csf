#include "../../include/cs/cs_player.h"
#include "../../include/u_process.h"
#include "../../include/cs/cs_interfaces.h"

extern u_process cs_p;
extern uintptr_t cs_nv_dwEntityList;
extern uint32_t  cs_nv_m_iHealth;
extern uint32_t  cs_nv_m_vecViewOffset;
extern uint32_t  cs_nv_m_lifeState;
extern uint32_t  cs_nv_m_nTickBase;
extern uint32_t  cs_nv_m_vecVelocity;
extern uint32_t  cs_nv_m_vecPunch;
extern uint32_t  cs_nv_m_iFOV;
extern uint32_t  cs_nv_m_iTeamNum;
extern uint32_t  cs_nv_m_vecOrigin;
extern uint32_t  cs_nv_m_hActiveWeapon;
extern uint32_t  cs_nv_m_iShotsFired;
extern uint32_t  cs_nv_m_bIsScoped;
extern uint32_t  cs_nv_m_dwBoneMatrix;

int cs_player::GetTeam(void)
{
    return cs_p.read<int>(self + cs_nv_m_iTeamNum);
}

int cs_player::GetHealth(void)
{
    return cs_p.read<int>(self + cs_nv_m_iHealth);
}

int cs_player::GetLifeState(void)
{
    return cs_p.read<int>(self + cs_nv_m_lifeState);
}

int cs_player::GetTickCount(void)
{
    return cs_p.read<int>(self + cs_nv_m_nTickBase);
}

int cs_player::GetShotsFired(void)
{
    return cs_p.read<int>(self + cs_nv_m_iShotsFired);
}

bool cs_player::IsScoped(void)
{
    return cs_p.read<bool>(self + cs_nv_m_bIsScoped);
}

bool cs_player::IsDormant(void)
{
    uintptr_t a;

    a = (uintptr_t)(self + 0x8);
    return cs_p.read<bool>(a + cs_p.read<unsigned char>((*(cs_virtual_table*)&a).function(9) + 0x8));
}

uintptr_t cs_player::GetWeapon(void)
{
    uint32_t v;

    v = cs_p.read<uintptr_t>(self + cs_nv_m_hActiveWeapon);
    return cs_p.read<uintptr_t>(cs_nv_dwEntityList + ((v & 0xFFF) - 1) * 0x10);
}

vec3 cs_player::GetOrigin(void)
{
    return cs_p.read<vec3>(self + cs_nv_m_vecOrigin);
}

vec3 cs_player::GetVecView(void)
{
    return cs_p.read<vec3>(self + cs_nv_m_vecViewOffset);
}
vec3 cs_player::GetEyePos(void)
{
    vec3 v, o, r;
    v = this->GetVecView();
    o = this->GetOrigin();
    r.x = v.x += o.x; r.y = v.y += o.y; r.z = v.z += o.z;
    return r;
}

vec3 cs_player::GetVecVelocity(void)
{
    return cs_p.read<vec3>(self + cs_nv_m_vecViewOffset);
}

vec3 cs_player::GetVecPunch(void)
{
    return cs_p.read<vec3>(self + cs_nv_m_vecPunch);
}

int cs_player::GetFov(void)
{
    return cs_p.read<int>(self + cs_nv_m_iFOV);
}

void cs_player::GetBoneMatrix(int index, matrix3x4_t *out)
{
    cs_p.read(cs_p.read<uintptr_t>(self + cs_nv_m_dwBoneMatrix) + 0x30 * index, out, sizeof(matrix3x4_t));
}

bool cs_player::IsValid(void)
{
    int health;

    health = GetHealth();
    return self && GetLifeState() == 0 && health > 0 && health < 1337;
}

