#include "../include/cs/cs_engine.h"
#include "../include/u_mouse.h"
#include "../../rx/rx.h"
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define MOUSE_1 107
#define MOUSE_2 108
#define MOUSE_3 109
#define MOUSE_4 110
#define MOUSE_5 111

#define AIMMODE_FACEIT   0
#define AIMMODE_STANDARD 1

#define SETTINGS_AIMFOV 30.0f
#define SETTINGS_AIMMODE AIMMODE_FACEIT
#define SETTINGS_AIMKEY MOUSE_1
#define SETTINGS_TRIGGERKEY MOUSE_5

/*
 * this is just mini demo for the sdk,
 * code is from ExtCSGO project
 * and is ugly and messy.
 * you should use this code just an example
 */
static void demo_aimassist(void);

/*
 * u_mouse is in send mode only, listening position / button presses is not possible
 * if you want listen the mouse, change mode -> RX_INPUT_MODE_ALL or RX_INPUT_MODE_RECEIVE
 * >  u_mouse mouse( RX_INPUT_MODE_ALL )  <
 */
static u_mouse   mouse( RX_INPUT_MODE_SEND );

static cs_convar sensitivity, mp_teammates_are_enemies;
static cs_player target;

int main(void)
{
    if (getuid() != 0) {
        printf("[!]run as sudo\n");
        return -1;
    }
    if (!cs_initialize()) {
        printf("[!]unknown error\n");
        return -1;
    }
    sensitivity = cvar::find("sensitivity");
    mp_teammates_are_enemies = cvar::find("mp_teammates_are_enemies");
    while ( engine::IsRunning() ) {
        if (engine::IsInGame()) {
            demo_aimassist();
        }
    }
    return 0;
}

static bool  best_target(vec3 vangle, cs_player self, cs_player *target);
static vec3  target_angle(cs_player self, cs_player target);
static float angle_fov(vec3 vangle, vec3 angle);
static void aim_standard(vec3 vangle, vec3 angle);
static void aim_faceit(vec3 vangle, vec3 angle);
static void  (*aim[2])(vec3, vec3) = { aim_faceit, aim_standard };
static int   crosshair_id(vec3 vangle, cs_player self, cs_player target);
static float vec_length(vec3);

static void demo_aimassist(void)
{
    cs_player   s;
    vec3        v, t;

    if (inputsystem::IsButtonDown(SETTINGS_TRIGGERKEY)) {
        s = entity::GetClientEntity(engine::GetLocalPlayer());
        v = engine::GetViewAngles();

        if (!best_target(v, s, &target)) {
            return;
        }
        if (crosshair_id(v, s, target)) {
            mouse.click(RX_BUTTON_MOUSE1);
        }
        return;
    }

    if (inputsystem::IsButtonDown(SETTINGS_AIMKEY)) {
        s = entity::GetClientEntity(engine::GetLocalPlayer());
        v = engine::GetViewAngles();

        if (!target.IsValid() && !best_target(v, s, &target)) {
            return;
        }
        t = target_angle(s, target);
        if (angle_fov(v, t) <= SETTINGS_AIMFOV ) {
            aim[SETTINGS_AIMMODE](v, t);
        }
    } else {
        target = {};
    }
}

static bool best_target(vec3 vangle, cs_player self, cs_player *target)
{
    float b; int i; cs_player e; float c;
    b = 9999.0f;
    for (i = 1; i < 33; i++) {
        e = entity::GetClientEntity(i);
        if (!e.IsValid())
            continue;
        if ( !mp_teammates_are_enemies.GetInt() && self.GetTeam() == e.GetTeam())
            continue;
        c = angle_fov(vangle, target_angle(self, e));
        if (c < b) {
            b = c;
            *target = e;
        }
    }
    return b != 9999.0f;
}

static void vec_normalize(vec3 *vec)
{
    float radius;

    radius = 1.f / (float)(sqrt(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z) + 1.192092896e-07f);
    vec->x *= radius, vec->y *= radius, vec->z *= radius;
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

static vec3 target_angle(cs_player self, cs_player target)
{
    matrix3x4_t m;
    vec3        c, p;

    target.GetBoneMatrix(8, &m);
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

static float angle_fov(vec3 vangle, vec3 angle)
{
    vec3 a0, a1;

    angle_vec(vangle, &a0);
    angle_vec(angle, &a1);
    return RAD2DEG(acos(vec_dot(a0, a1) / vec_length(a0)));
}

typedef struct {
    int   bone;
    float radius;
    vec3  min;
    vec3  max;
} HITBOX ;

static HITBOX hitbox_list[2][6] = {
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

static vec3 vec_transform(vec3 p0, matrix3x4_t p1)
{
    vec3 v;

    v.x = (p0.x * p1[0][0] + p0.y * p1[0][1] + p0.z * p1[0][2]) + p1[0][3];
    v.y = (p0.x * p1[1][0] + p0.y * p1[1][1] + p0.z * p1[1][2]) + p1[1][3];
    v.z = (p0.x * p1[2][0] + p0.y * p1[2][1] + p0.z * p1[2][2]) + p1[2][3];
    return v;
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

double vec_distance(vec3 p0, vec3 p1)
{
    return vec_length_sqrt( vec_sub(p0, p1) ) ;
}

static void aim_faceit(vec3 vangle, vec3 angle)
{
    vec3 px;

    px = vec_sub(vangle, angle);
    vec_clamp(&px);
    px.x = ((px.x / sensitivity.GetFloat() ) / -0.022f);
    px.y = ((px.y / sensitivity.GetFloat() ) /  0.022f);

    mouse.move((int)px.y, (int)px.x);
    usleep(5000); // 5ms delay
}

static void aim_standard(vec3, vec3 angle)
{
    engine::SetViewAngles(angle);
}

bool vec_min_max(vec3 eye, vec3 dir, vec3 min, vec3 max, float radius)
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

static int crosshair_id(vec3 vangle, cs_player self, cs_player target)
{
    int         id, i;
    vec3        dir;
    matrix3x4_t matrix;
    bool        status;

    dir = vec_atd(vangle);
    id = target.GetTeam() - 2;
    for ( i = 6; i-- ; ) {
        target.GetBoneMatrix(hitbox_list[id][i].bone, &matrix);
        status = vec_min_max(
            self.GetEyePos(),
            dir,
            vec_transform(hitbox_list[id][i].min, matrix),
            vec_transform(hitbox_list[id][i].max, matrix),
            hitbox_list[id][i].radius
            ) ;
        if (status == true) {
            return i;
        }
    }
    return 0;
}

