#include "../../include/cs/cs_inputsystem.h"
#include "../../include/u_process.h"
#include "../../include/cs/cs_interfaces.h"

extern u_process        cs_p;
extern cs_virtual_table cs_vt_inputsystem;
extern uint32_t         cs_nv_m_dwButton;
extern uint32_t         cs_nv_m_dwAnalog;
extern uint32_t         cs_nv_m_dwAnalogDelta;

bool inputsystem::IsButtonDown(int button_code)
{
    uint32_t v;
    v = cs_p.read<uint32_t>(cs_vt_inputsystem.address() + (((button_code >> 5 ) * 4) + cs_nv_m_dwButton));
    return (v >> (button_code & 31)) & 1;
}

vec2i inputsystem::GetMouseAnalog(void)
{
    return cs_p.read<vec2i>(cs_vt_inputsystem.address() + cs_nv_m_dwAnalog);
}

vec2i inputsystem::GetMouseAnalogDelta(void)
{
    return cs_p.read<vec2i>(cs_vt_inputsystem.address() + cs_nv_m_dwAnalogDelta);
}

