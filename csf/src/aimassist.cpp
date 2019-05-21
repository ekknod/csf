#include "../include/u_config.h"
#include "../include/cs/cs_engine.h"
#include "../../rx/include/rx_input.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

class config {
    const char *_path;
public:
    struct {
        bool                       enable;
        inputsystem::CS_BUTTONCODE button;
        float                      fov;
        float                      smooth;
        int                        bone;
    } aimbot ;

    struct {
        bool                       enable;
        inputsystem::CS_BUTTONCODE button;
    } triggerbot ;

    config(const char *path) : _path(path)
    {
        u_config       c(path);
        u_config_table t;

        if (!c.exists()) {
            aimbot.enable     = true;
            aimbot.button     = inputsystem::MOUSE_1;
            aimbot.fov        = 2.0f;
            aimbot.smooth     = 2.0f;
            aimbot.bone       = 8;
            triggerbot.enable = true;
            triggerbot.button = inputsystem::MOUSE_5;
        } else {
            c.open_config();
            t = c.table("[aimbot]");
            aimbot.enable     = t.GetInt("enable");
            aimbot.button     = (inputsystem::CS_BUTTONCODE)t.GetInt("button");
            aimbot.fov        = t.GetFloat("fov");
            aimbot.smooth     = t.GetFloat("smooth");
            aimbot.bone       = t.GetInt("bone");
            t = c.table("[triggerbot]");
            triggerbot.enable = t.GetInt("enable");
            triggerbot.button = (inputsystem::CS_BUTTONCODE)t.GetInt("button");
            c.close_config();
        }
        aimbot.fov = aimbot.fov / 180.0f;
        // casts between pointers / enums, really c++ :DDDDDDDDD
    }
    ~config(void)
    {
        if ( exists() ) {
            update();
        }
    }
    bool exists(void)
    {
        return u_config(_path).exists();
    }
    void update(void)
    {
        u_config c(_path);
        int  length;
        char buffer[512];

        aimbot.fov *= 180.0f;
        length = snprintf(
            buffer,
            sizeof(buffer),
            "[aimbot]\n{\n"
            "    enable: %d\n"
            "    button: %d\n"
            "    fov:    %f\n"
            "    smooth: %f\n"
            "    bone:   %d\n}\n"
            "[triggerbot]\n{\n"
            "    enable: %d\n"
            "    button: %d\n}\n",
            aimbot.enable,
            aimbot.button,
            aimbot.fov,
            aimbot.smooth,
            aimbot.bone,
            triggerbot.enable,
            triggerbot.button
            ) ;
        aimbot.fov /= 180.0f;
        c.open_config();
        c.write_config(buffer, length);
        c.close_config();
    }
    void print(void)
    {
        aimbot.fov *= 180.0f;
        printf(
            "[aimbot]\n{\n"
            "    enable: %d\n"
            "    button: %d\n"
            "    fov:    %f\n"
            "    smooth: %f\n"
            "    bone:   %d\n}\n"
            "[triggerbot]\n{\n"
            "    enable: %d\n"
            "    button: %d\n}\n",
            aimbot.enable,
            aimbot.button,
            aimbot.fov,
            aimbot.smooth,
            aimbot.bone,
            triggerbot.enable,
            triggerbot.button
            );
        aimbot.fov /= 180.0f;
    }
} ;


static config      _cfg("config.cfg");
static rx_handle   _mouse;
static cs_player   _target;
static int         _target_bone;
static cs_convar   _sensitivity;
static cs_convar   _mp_teammates_are_enemies;
static float       _flsensitivity;
static int         _current_tick, _previous_tick;

static void sincos(float radians, float *sine, float *cosine)
{
    *sine = (float)sin(radians);
    *cosine = (float)cos(radians);
}

#define RAD2DEG(x) ((float)(x) * (float)(180.f / 3.14159265358979323846f))
#define DEG2RAD(x) ((float)(x) * (float)(3.14159265358979323846f / 180.f))

static void angle_vec(vec3 angles, vec3 *forward)
{
    float sp, sy, cp, cy;
    sincos(DEG2RAD(angles.x), &sp, &cp);
    sincos(DEG2RAD(angles.y), &sy, &cy);
    forward->x = cp * cy;
    forward->y = cp * sy;
    forward->z = -sp;
}

static float vec_dot(vec3 v0, vec3 v1)
{
    return (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);
}

static float vec_length(vec3 v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}

static vec3 vec_sub(vec3 p0, vec3 p1)
{
    vec3 r;

    r.x = p0.x - p1.x;
    r.y = p0.y - p1.y;
    r.z = p0.z - p1.z;
    return r;
}

static float vec_length_sqrt(vec3 p0)
{
    return sqrt(p0.x * p0.x + p0.y * p0.y + p0.z * p0.z);
}

static vec3 vec_delta(vec3 p0, vec3 p1)
{
    vec3  d;
    float l;

    d   = vec_sub(p0, p1);
    l   = (float)vec_length_sqrt(d);
    d.x /= l; d.y /= l; d.z /= l;
    return d;
}

static float vec_distance(vec3 p0, vec3 p1)
{
    return vec_length_sqrt( vec_sub(p0, p1) ) ;
}

static void vec_clamp(vec3 *v)
{
    if ( v->x > 89.0f && v->x <= 180.0f ) {
        v->x = 89.0f;
    }
    if ( v->x > 180.0f ) {
        v->x = v->x - 360.0f;
    }
    if( v->x < -89.0f ) {
        v->x = -89.0f;
    }
    v->y = fmodf(v->y + 180, 360) - 180;
    v->z = 0;
}

static void vec_normalize(vec3 *vec)
{
    float radius;

    radius = 1.f / (float)(sqrt(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z) + 1.192092896e-07f);
    vec->x *= radius, vec->y *= radius, vec->z *= radius;
}

static vec3 vec_transform(vec3 p0, matrix3x4_t p1)
{
    vec3 v;

    v.x = (p0.x * p1[0][0] + p0.y * p1[0][1] + p0.z * p1[0][2]) + p1[0][3];
    v.y = (p0.x * p1[1][0] + p0.y * p1[1][1] + p0.z * p1[1][2]) + p1[1][3];
    v.z = (p0.x * p1[2][0] + p0.y * p1[2][1] + p0.z * p1[2][2]) + p1[2][3];
    return v;
}

vec3 vec_atd(vec3 vangle)
{
    double y[2], p[2];

    vangle.x *= (3.14159265358979323846f / 180.f);
    vangle.y *= (3.14159265358979323846f / 180.f);
    y[0]     = sin(vangle.y), y[1] = cos(vangle.y);
    p[0]     = sin(vangle.x), p[1] = cos(vangle.x);
    vangle.x = (float)(p[1] * y[1]);
    vangle.y = (float)(p[1] * y[0]);
    vangle.z = (float)-p[0];
    return vangle;
}

typedef struct {
    int   bone;
    float radius;
    vec3  min;
    vec3  max;
} HITBOX ;

static HITBOX _hitbox_list[2][6] = {
    {
        {5, 6.200000f, {4.800000f,  0.150000f,  -4.100000f}, {4.800000f,  0.150000f, 4.100000f}},
        {4, 6.500000f, {3.800000f,  0.800000f,  -2.400000f}, {3.800000f,  0.400000f, 2.400000f}},
        {3, 6.000000f, {1.400000f,  0.800000f,  3.100000f},  {1.400000f,  0.800000f, -3.100000f}},
        {0, 6.000000f, {-2.700000f, 1.100000f,  -3.200000f}, {-2.700000f, 1.100000f, 3.200000f}},
        {7, 3.500000f, {-1.300000f, -0.200000f, 0.000000f},  {1.400000f,  0.600000f, 0.000000f}},
        {8, 4.300000f, {-1.100000f, 1.400000f,  0.100000f},  {3.000000f,  0.800000f, 0.100000f}}
    },

    {
        {5, 6.200000f, {4.800000f,  0.150000f,  -4.100000f}, {4.800000f,  0.150000f, 4.100000f}},
        {4, 6.500000f, {3.800000f,  0.800000f,  -2.400000f}, {3.800000f,  0.400000f, 2.400000f}},
        {3, 6.000000f, {1.400000f,  0.800000f,  3.100000f},  {1.400000f,  0.800000f, -3.100000f}},
        {0, 6.000000f, {-2.700000f, 1.100000f,  -3.200000f}, {-2.700000f, 1.100000f, 3.200000f}},
        {7, 3.500000f, {-1.300000f, 0.900000f,  0.000000f},  {1.400000f,  1.300000f, 0.000000f}},
        {8, 3.200000f, {-0.200000f, 1.100000f,  0.000000f},  {3.600000f,  0.100000f, 0.000000f}}
    }
};

static bool vec_min_max(vec3 eye, vec3 dir, vec3 min, vec3 max, float radius)
{
    vec3     delta;
    uint32_t i;
    vec3     q;
    float    v;


    //
    // original maths by superdoc1234
    //
    delta = vec_delta(max, min);
    for ( i = 0; i < vec_distance(min, max); i++ ) {
        q.x = min.x + delta.x * (float)i - eye.x;
        q.y = min.y + delta.y * (float)i - eye.y;
        q.z = min.z + delta.z * (float)i - eye.z;
        if ((v = vec_dot(q, dir)) < 1.0f) {
            return false;
        }
        v = radius * radius - (vec_length(q) - v * v);
        if ( v <= -100.f ) {
            return false;
        }
        if (v >= 1.19209290E-07F) {
            return true;
        }
    }
    return false;
}

static int crosshair_id(vec3 vangle, cs_player self)
{
    vec3        dir, eye;
    int         i;
    cs_player   entity;
    int         id, j;
    matrix3x4_t matrix;
    bool        status;


    dir = vec_atd(vangle);
    eye = self.GetEyePos();
    for (i = 1; i < engine::GetMaxClients(); i++) {
        entity = entity::GetClientEntity(i);
        if (!entity.IsValid())
            continue;
        id = entity.GetTeam();
        if (!_mp_teammates_are_enemies.GetInt() && self.GetTeam() == id)
            continue;

        id -= 2;
        for (j = 6; j-- ;) {
            entity.GetBoneMatrix(_hitbox_list[id][j].bone, &matrix);
            status = vec_min_max(
                eye,
                dir,
                vec_transform(_hitbox_list[id][j].min, matrix),
                vec_transform(_hitbox_list[id][j].max, matrix),
                _hitbox_list[id][j].radius
                ) ;
            if (status) {
                return i;
            }
        }
    }
    return 0;
}

static void vec_angles(vec3 forward, vec3 *angles)
{
    float tmp, yaw, pitch;

    if (forward.y == 0.f && forward.x == 0.f) {
        yaw = 0;
        if (forward.z > 0) {
            pitch = 270;
        } else {
            pitch = 90.f;
        }
    } else {
        yaw = (float)(atan2(forward.y, forward.x) * 180.f / 3.14159265358979323846f);
        if (yaw < 0) {
            yaw += 360.f;
        }
        tmp = (float)sqrt(forward.x * forward.x + forward.y * forward.y);
        pitch = (float)(atan2(-forward.z, tmp) * 180.f / 3.14159265358979323846f);
        if (pitch < 0) {
            pitch += 360.f;
        }
    }
    angles->x = pitch;
    angles->y = yaw;
    angles->z = 0.f;
}

static float get_fov(vec3 vangle, vec3 angle)
{
    vec3 a0, a1;

    angle_vec(vangle, &a0);
    angle_vec(angle, &a1);
    return RAD2DEG(acos(vec_dot(a0, a1) / vec_length(a0)));
}

static vec3 get_target_angle(cs_player self, cs_player target, int bone_id)
{
    matrix3x4_t m;
    vec3        c, p;

    target.GetBoneMatrix(bone_id, &m);
    c = self.GetEyePos();
    m[0][3] -= c.x, m[1][3] -= c.y, m[2][3] -= c.z;
    c.x = m[0][3], c.y = m[1][3], c.z = m[2][3];
    vec_normalize(&c);
    vec_angles(c, &c);
    if (self.GetShotsFired() > 1) {
        p = self.GetVecPunch();
        c.x -= p.x * 2.0f, c.y -= p.y * 2.0f, c.z -= p.z * 2.0f;
    }
    vec_clamp(&c);
    return c;
}

static bool get_target(cs_player self, vec3 vangle)
{
    float     best_fov;
    int       i;
    cs_player entity;
    float     fov;
    int       j;

    best_fov = 9999.0f;
    for (i = 1; i < engine::GetMaxClients(); i++) {

        entity = entity::GetClientEntity(i);
        if (!entity.IsValid())
            continue;

        if (!_mp_teammates_are_enemies.GetInt() && self.GetTeam() == entity.GetTeam())
            continue;

        if (_cfg.aimbot.bone == 0) {
            for (j = 7; j--;) {
                fov = get_fov(vangle, get_target_angle(self, entity, _hitbox_list[0][j].bone));
                if (fov < best_fov) {
                    best_fov = fov;
                    _target  = entity;
                    _target_bone = _hitbox_list[0][j].bone;
                }
            }
        } else {
            fov = get_fov(vangle, get_target_angle(self, entity, _cfg.aimbot.bone));
            if (fov < best_fov) {
                best_fov = fov;
                _target  = entity;
                _target_bone = _cfg.aimbot.bone;
             }
        }
    }
    return best_fov != 9999.0f;
}

static void mouse_move(int x, int y)
{
    rx_send_input_axis(_mouse, RX_MOUSE_X, x);
    rx_send_input_axis(_mouse, RX_MOUSE_Y, y);
}

static void mouse1_down(void)
{
    rx_send_input_button(_mouse, RX_BTN_LEFT, 1);
}

static void mouse1_up(void)
{
    rx_send_input_button(_mouse, RX_BTN_LEFT, 0);
}

static void aim_at_target(vec3 vangle, vec3 angle)
{
    float x, y, sx, sy;


    y = vangle.x - angle.x, x = vangle.y - angle.y;
    if (y > 89.0f)   y = 89.0f;   else if (y < -89.0f)  y = -89.0f;
    if (x > 180.0f)  x -= 360.0f; else if (x < -180.0f) x += 360.0f;

    if (abs(x) / 180.0f >= _cfg.aimbot.fov)
        return;
    if (abs(y) / 89.0f >= _cfg.aimbot.fov)
        return;
    x = ((x / _flsensitivity) / 0.022f);
    y = ((y / _flsensitivity) / -0.022f);
    if (_cfg.aimbot.smooth) {
        sx = 0.0f, sy = 0.0f;
        if (sx < x) {
            sx += 1.0f + (x / _cfg.aimbot.smooth);
        } else if (sx > x) {
            sx -= 1.0f - (x / _cfg.aimbot.smooth);
        }
        if (sy < y) {
            sy += 1.0f + (y / _cfg.aimbot.smooth);
        } else if (sy > y) {
            sy -= 1.0f - (y / _cfg.aimbot.smooth);
        }
    } else {
        sx = x, sy = y;
    }
    if ( _current_tick - _previous_tick > 0 ) {
        _previous_tick = _current_tick;
        mouse_move((int)sx, (int)sy);
    }
}

static void aim(void)
{
    cs_player self;
    vec3      vangle;
    bool      a;


    self           = entity::GetClientEntity(engine::GetLocalPlayer());
    vangle         = engine::GetViewAngles();
    _current_tick  = self.GetTickCount();
    _flsensitivity = _sensitivity.GetFloat();

    if (self.IsScoped()) {
        _flsensitivity = (self.GetFov() / 90.0f) * _flsensitivity;
    }
    if (_cfg.triggerbot.enable && inputsystem::IsButtonDown(_cfg.triggerbot.button)) {
        a = false;
        if (crosshair_id(vangle, self)) {
            mouse1_down();
            usleep(10000);
            mouse1_up();
        }
    } else {
        a = true;
    }
    if (_cfg.aimbot.enable) {
        if (a == true && inputsystem::IsButtonDown(_cfg.aimbot.button)) {
            if (!_target.IsValid() && !get_target(self, vangle))
                return;
            aim_at_target(vangle, get_target_angle(self, _target, _target_bone));
        } else {
            _target = {};
        }
    }
}

static int parameters(int argc, char **argv)
{
    int i;

    if (argc == 1) {
        if (!_cfg.exists()) {
            printf("[!]config file not found!\n");
            goto help;
        }
        if (getuid() != 0) {
            printf("[!]root is required\n");
            return -1;
        }
        return 0;
    }

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help")) {
        help:
            printf(
                "[*]commands:\n"
                "   --read:  print config\n"
                "   --write: write new config file to disk\n");
            return -1;
        }
        if (!strcmp(argv[i], "--write")) {
            if (getuid() == 0) {
                printf("[!]writing config file with root access is not allowed!\n");
            } else {
                _cfg.update();
            }
            return -1;
        }
        if (!strcmp(argv[i], "--read")) {
            if (_cfg.exists()) {
                _cfg.print();
            } else {
                printf(
                    "[!]config file not found!\n"
                    "   --write: write new config file to disk\n"
                    );
            }
            return -1;
        }
    }
    printf("[*]commands: --help\n");
    return -1;
}

int main(int argc, char **argv)
{
    if (parameters(argc, argv) == -1)
        return 0;
    if (!cs_initialize()) {
        printf("[!]game is not running!\n");
        return -1;
    }
    _mouse = rx_open_input(RX_INPUT_TYPE_MOUSE, RX_INPUT_MODE_SEND);
    if (_mouse == 0) {
        printf("[!]mouse device not found\n");
        return -1;
    }
    _sensitivity              = cvar::find("sensitivity");
    _mp_teammates_are_enemies = cvar::find("mp_teammates_are_enemies");
    while (engine::IsRunning()) {
        if (engine::IsInGame()) {
            aim();
            usleep(1000);
        }
    }
    rx_close_handle(_mouse);
}

