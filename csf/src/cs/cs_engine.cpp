#include "../../include/cs/cs_engine.h"
#include "../../include/u_process.h"
#include <stdio.h>

u_process        cs_p("csgo_linux64");
cs_virtual_table cs_vt_client;
cs_virtual_table cs_vt_entity;
cs_virtual_table cs_vt_engine;
cs_virtual_table cs_vt_cvar;
cs_virtual_table cs_vt_inputsystem;
uintptr_t        cs_nv_dwEntityList;
uintptr_t        cs_nv_dwClientState;
uint32_t         cs_nv_m_dwGetLocalPlayer;
uint32_t         cs_nv_m_dwViewAngles;
uint32_t         cs_nv_m_dwMaxClients;
uint32_t         cs_nv_m_dwState;
uint32_t         cs_nv_m_dwButton;
uint32_t         cs_nv_m_dwAnalogDelta;
uint32_t         cs_nv_m_dwAnalog;
uint32_t         cs_nv_m_iHealth;
uint32_t         cs_nv_m_vecViewOffset;
uint32_t         cs_nv_m_lifeState;
uint32_t         cs_nv_m_nTickBase;
uint32_t         cs_nv_m_vecVelocity;
uint32_t         cs_nv_m_vecPunch;
uint32_t         cs_nv_m_iFOV;
uint32_t         cs_nv_m_iTeamNum;
uint32_t         cs_nv_m_vecOrigin;
uint32_t         cs_nv_m_hActiveWeapon;
uint32_t         cs_nv_m_iShotsFired;
uint32_t         cs_nv_m_bIsScoped;
uint32_t         cs_nv_m_dwBoneMatrix;

static void initialize_vt(void);
static void initialize_nv(void);

bool cs_initialize(void)
{
    if (!cs_p.attach()) {
        return false;
    }
    initialize_vt();
    initialize_nv();

    printf(
        "[*]vtables:\n"
        "    VClient:                         0x%lx\n"
        "    VClientEntityList:               0x%lx\n"
        "    VEngineClient:                   0x%lx\n"
        "    VEngineCvar:                     0x%lx\n"
        "    InputSystemVersion:              0x%lx\n"
        "[*]offsets:\n"
        "    dwEntityList:                    0x%lx\n"
        "    dwClientState:                   0x%lx\n"
        "    m_dwGetLocalPlayer:              0x%x\n"
        "    m_dwViewAngles:                  0x%x\n"
        "    m_dwMaxClients:                  0x%x\n"
        "    m_dwState:                       0x%x\n"
        "    m_dwButton:                      0x%x\n"
        "    m_dwAnalog:                      0x%x\n"
        "    m_dwAnalogDelta:                 0x%x\n"
        "[*]netvars:\n"
        "    DT_BasePlayer:                   m_iHealth:           0x%x\n"
        "    DT_BasePlayer:                   m_vecViewOffset:     0x%x\n"
        "    DT_BasePlayer:                   m_lifeState:         0x%x\n"
        "    DT_BasePlayer:                   m_nTickBase:         0x%x\n"
        "    DT_BasePlayer:                   m_vecVelocity:       0x%x\n"
        "    DT_BasePlayer:                   m_vecPunch:          0x%x\n"
        "    DT_BasePlayer:                   m_iFOV:              0x%x\n"
        "    DT_BaseEntity:                   m_iTeamNum:          0x%x\n"
        "    DT_BaseEntity:                   m_vecOrigin:         0x%x\n"
        "    DT_CSPlayer:                     m_hActiveWeapon:     0x%x\n"
        "    DT_CSPlayer:                     m_iShotsFired:       0x%x\n"
        "    DT_CSPlayer:                     m_bIsScoped:         0x%x\n"
        "    DT_BaseAnimating:                m_dwBoneMatrix:      0x%x\n"
        "[*]convar demo:\n"
        "    sensitivity:                     %ff\n"
        "    volume:                          %ff\n"
        "    cl_crosshairsize:                %ff\n"
        "[*]sdk info:\n"
        "   creator:                          ekknod\n"
        "   repo:                             github.com/ekknod/csf\n"
        ,
        cs_vt_client.address(),
        cs_vt_entity.address(),
        cs_vt_engine.address(),
        cs_vt_cvar.address(),
        cs_vt_inputsystem.address(),
        cs_nv_dwEntityList,
        cs_nv_dwClientState,
        cs_nv_m_dwGetLocalPlayer,
        cs_nv_m_dwViewAngles,
        cs_nv_m_dwMaxClients,
        cs_nv_m_dwState,
        cs_nv_m_dwButton,
        cs_nv_m_dwAnalog,
        cs_nv_m_dwAnalogDelta,
        cs_nv_m_iHealth,
        cs_nv_m_vecViewOffset,
        cs_nv_m_lifeState,
        cs_nv_m_nTickBase,
        cs_nv_m_vecVelocity,
        cs_nv_m_vecPunch,
        cs_nv_m_iFOV,
        cs_nv_m_iTeamNum,
        cs_nv_m_vecOrigin,
        cs_nv_m_hActiveWeapon,
        cs_nv_m_iShotsFired,
        cs_nv_m_bIsScoped,
        cs_nv_m_dwBoneMatrix,
        cvar::find("sensitivity").GetFloat(),
        cvar::find("volume").GetFloat(),
        cvar::find("cl_crosshairsize").GetFloat()
        ) ;
    return true;
}

int engine::IsRunning(void)
{
    return cs_p.exists();
}

int engine::GetLocalPlayer(void)
{
    return cs_p.read<int>(cs_nv_dwClientState + cs_nv_m_dwGetLocalPlayer) + 1;
}

vec3 engine::GetViewAngles(void)
{
    return cs_p.read<vec3>(cs_nv_dwClientState + cs_nv_m_dwViewAngles);
}

void engine::SetViewAngles(vec3 v)
{
    cs_p.write<vec3>(cs_nv_dwClientState + cs_nv_m_dwViewAngles, v);
}

int engine::GetMaxClients(void)
{
    return cs_p.read<int>(cs_nv_dwClientState + cs_nv_m_dwMaxClients);
}

bool engine::IsInGame(void)
{
    return cs_p.read<unsigned char>(cs_nv_dwClientState + cs_nv_m_dwState) >> 2;
}

cs_player entity::GetClientEntity(int index)
{
    return cs_p.read<cs_player>(cs_nv_dwEntityList + index * 0x20);
}

static void initialize_vt(void)
{
    cs_interface_reg t;

    t = interfaces::find("client_client.so");
        cs_vt_client      = t.find("VClient");
        cs_vt_entity      = t.find("VClientEntityList");
    t = interfaces::find("engine_client.so");
        cs_vt_engine      = t.find("VEngineClient");
    t = interfaces::find("materialsystem_client.so");
        cs_vt_cvar        = t.find("VEngineCvar");
    t = interfaces::find("inputsystem_client.so");
        cs_vt_inputsystem = t.find("InputSystemVersion");
}

static uintptr_t offset_entitylist(void)
{
    return cs_vt_entity.address() - cs_p.read<uint32_t>(cs_vt_entity.function(4) + 3) + 0x08;
}

static uintptr_t offset_clientstate(void)
{
    uintptr_t a0;
    uint32_t  a1, a2;

    a0 = cs_vt_engine.function(18);
    a1 = cs_p.read<uint32_t>(a0 + 0x11 + 1) + 0x16;
    a2 = cs_p.read<uint32_t>(a0 + a1 + 5 + 3) + 0xC;
    return cs_p.read<uintptr_t>(a0 + a1 + a2 + 0x8) + 0x8;
}

static void initialize_nv(void)
{
    cs_netvar_table t;

    cs_nv_dwEntityList        = offset_entitylist();
    cs_nv_dwClientState       = offset_clientstate();
    cs_nv_m_dwGetLocalPlayer  = cs_p.read<uint32_t>(cs_vt_engine.function(12) + 0x11);
    cs_nv_m_dwViewAngles      = cs_p.read<uint32_t>(cs_vt_engine.function(18) + 0x1A);
    cs_nv_m_dwMaxClients      = cs_p.read<uint32_t>(cs_vt_engine.function(20) + 0x0C);
    cs_nv_m_dwState           = cs_p.read<uint32_t>(cs_vt_engine.function(26) + 0x0C);
    cs_nv_m_dwButton          = cs_p.read<uint32_t>(cs_vt_inputsystem.function(15) + 0x019);
    cs_nv_m_dwAnalogDelta     = cs_p.read<uint32_t>(cs_vt_inputsystem.function(18) + 0x01a);
    cs_nv_m_dwAnalog          = cs_p.read<uint32_t>(cs_vt_inputsystem.function(19) + 0x170);

    t = netvars::find("DT_BasePlayer");  
        cs_nv_m_iHealth       = t.offset("m_iHealth");
        cs_nv_m_vecViewOffset = t.offset("m_vecViewOffset[0]");
        cs_nv_m_lifeState     = t.offset("m_lifeState");
        cs_nv_m_nTickBase     = t.offset("m_nTickBase");
        cs_nv_m_vecVelocity   = t.offset("m_vecVelocity[0]");
        cs_nv_m_vecPunch      = t.offset("m_aimPunchAngle");
        cs_nv_m_iFOV          = t.offset("m_iFOV");
    t = netvars::find("DT_BaseEntity");
        cs_nv_m_iTeamNum      = t.offset("m_iTeamNum");
        cs_nv_m_vecOrigin     = t.offset("m_vecOrigin");
    t = netvars::find("DT_CSPlayer");
        cs_nv_m_hActiveWeapon = t.offset("m_hActiveWeapon");
        cs_nv_m_iShotsFired   = t.offset("m_iShotsFired");
        cs_nv_m_bIsScoped     = t.offset("m_bIsScoped");
    t = netvars::find("DT_BaseAnimating");
        cs_nv_m_dwBoneMatrix  = t.offset("m_nForceBone") + 0x2C;
}

