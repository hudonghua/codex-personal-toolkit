#include "Arm200A_Kine.h"
#include "config.h"

typedef struct
{
    float m[3][3];
} ARM200A_MAT3;

typedef struct
{
    unsigned char has_o0;
    ARM200A_POINT3 o0_body;
    ARM200A_POINT3 o0o1;
    ARM200A_POINT3 o1o;
    ARM200A_POINT3 ob;
    ARM200A_POINT3 ba;
    ARM200A_POINT3 ac;
    ARM200A_POINT3 d1e;
    ARM200A_POINT3 ee1;
    ARM200A_POINT3 e1f;
    ARM200A_POINT3 fg;
    ARM200A_POINT3 g1h;
    ARM200A_POINT3 e1e2;
    float dd1_mm;
} ARM200A_BASE;

typedef struct
{
    unsigned char use;
    float root_raw;
    float base_raw;
    float boom_raw;
    float cd_raw;
    float d1_raw;
    float e1_raw;
    float f_raw;
    float gg1_raw;
    float body_roll_raw;
    float body_pitch_raw;
} ARM200A_ZERO_SAMPLE;

typedef struct
{
    unsigned char ok;
    unsigned char count;
    float rms_mm;
    float max_mm;
} ARM200A_ZERO_FIT_INFO;

#define ARM200A_ZERO_SAMPLE_MAX  1
#define ARM200A_ZERO_FIT_NUM     8

ARM200A_INPUT gArmLeftIn;
ARM200A_INPUT gArmMidIn;
ARM200A_INPUT gArmRightIn;

ARM200A_RAW_INPUT gArmLeftRaw;
ARM200A_RAW_INPUT gArmMidRaw;
ARM200A_RAW_INPUT gArmRightRaw;

ARM200A_ZERO_CFG gArmLeftZero;
ARM200A_ZERO_CFG gArmMidZero;
ARM200A_ZERO_CFG gArmRightZero;

ARM200A_RESULT gArmLeftBodyOut;
ARM200A_RESULT gArmMidBodyOut;
ARM200A_RESULT gArmRightBodyOut;

ARM200A_RESULT gArmLeftOut;
ARM200A_RESULT gArmMidOut;
ARM200A_RESULT gArmRightOut;

ARM200A_H1_INFO gArmLeftH1Info;
ARM200A_H1_INFO gArmMidH1Info;
ARM200A_H1_INFO gArmRightH1Info;

ARM200A_FIX gArmLeftFix;
ARM200A_FIX gArmMidFix;
ARM200A_FIX gArmRightFix;

ARM200A_SAMPLE gArmLeftSample[ARM200A_SAMPLE_MAX];
ARM200A_SAMPLE gArmMidSample[ARM200A_SAMPLE_MAX];
ARM200A_SAMPLE gArmRightSample[ARM200A_SAMPLE_MAX];

ARM200A_FIT_INFO gArmLeftFit;
ARM200A_FIT_INFO gArmMidFit;
ARM200A_FIT_INFO gArmRightFit;

ARM200A_COORD_TRANS gArmCoordTrans;

static ARM200A_ZERO_SAMPLE gArmLeftZeroSample[ARM200A_ZERO_SAMPLE_MAX];
static ARM200A_ZERO_SAMPLE gArmMidZeroSample[ARM200A_ZERO_SAMPLE_MAX];
static ARM200A_ZERO_SAMPLE gArmRightZeroSample[ARM200A_ZERO_SAMPLE_MAX];

static ARM200A_ZERO_FIT_INFO gArmLeftZeroFit;
static ARM200A_ZERO_FIT_INFO gArmMidZeroFit;
static ARM200A_ZERO_FIT_INFO gArmRightZeroFit;

float gArmQ1WorldX = 0.0f;
float gArmQ1WorldY = 0.0f;
float gArmQ1WorldZ = 0.0f;
float gArmQ2WorldX = 0.0f;
float gArmQ2WorldY = 0.0f;
float gArmQ2WorldZ = 0.0f;
float gArmQ3WorldX = 0.0f;
float gArmQ3WorldY = 0.0f;
float gArmQ3WorldZ = 0.0f;
float gArmQ4WorldX = 0.0f;
float gArmQ4WorldY = 0.0f;
float gArmQ4WorldZ = 0.0f;

unsigned char gArmFitReqLeft = 0;
unsigned char gArmFitReqMid = 0;
unsigned char gArmFitReqRight = 0;

unsigned int gArmZeroMaskLeft = 0;
unsigned int gArmZeroMaskMid = 0;
unsigned int gArmZeroMaskRight = 0;

unsigned char gArmCoordReq = 0;

unsigned char gArmSampleReqLeft = 0;
unsigned char gArmSampleReqMid = 0;
unsigned char gArmSampleReqRight = 0;

unsigned char gArmUseZeroComp = 0;

float gArmLeftSampleHx = 0.0f;
float gArmLeftSampleHy = 0.0f;
float gArmLeftSampleHz = 0.0f;

float gArmMidSampleHx = 0.0f;
float gArmMidSampleHy = 0.0f;
float gArmMidSampleHz = 0.0f;

float gArmRightSampleHx = 0.0f;
float gArmRightSampleHy = 0.0f;
float gArmRightSampleHz = 0.0f;

static ARM200A_ZERO_CFG arm_make_zero_default(void);
static float arm_absf(float v);
static float arm_clamp_unit(float v);
static float arm_deg_to_rad(float deg);
static ARM200A_POINT3 arm_p(float x, float y, float z);
static ARM200A_POINT3 arm_add(ARM200A_POINT3 a, ARM200A_POINT3 b);
static ARM200A_POINT3 arm_sub(ARM200A_POINT3 a, ARM200A_POINT3 b);
static ARM200A_POINT3 arm_scale(ARM200A_POINT3 a, float s);
static float arm_dot(ARM200A_POINT3 a, ARM200A_POINT3 b);
static ARM200A_POINT3 arm_cross(ARM200A_POINT3 a, ARM200A_POINT3 b);
static float arm_len(ARM200A_POINT3 a);
static float arm_dist(ARM200A_POINT3 a, ARM200A_POINT3 b);
static ARM200A_POINT3 arm_norm(ARM200A_POINT3 a);
static ARM200A_MAT3 arm_i(void);
static ARM200A_MAT3 arm_mul(ARM200A_MAT3 a, ARM200A_MAT3 b);
static ARM200A_POINT3 arm_mul_p(ARM200A_MAT3 m, ARM200A_POINT3 p);
static ARM200A_MAT3 arm_rx(float deg);
static ARM200A_MAT3 arm_ry(float deg);
static ARM200A_MAT3 arm_rz(float deg);
static ARM200A_MAT3 arm_ra(ARM200A_POINT3 axis, float deg);
static ARM200A_POINT3 arm_add_fix(ARM200A_POINT3 base, ARM200A_POINT3 fix);
static ARM200A_MAT3 arm_transpose(ARM200A_MAT3 a);
static const ARM200A_BASE *arm_get_base_cfg(unsigned char arm_id);
static ARM200A_INPUT *arm_get_input_buf(unsigned char arm_id);
static ARM200A_RAW_INPUT *arm_get_raw_buf(unsigned char arm_id);
static ARM200A_ZERO_CFG *arm_get_zero_cfg_buf(unsigned char arm_id);
static ARM200A_FIX *arm_get_fix_buf(unsigned char arm_id);
static ARM200A_FIT_INFO *arm_get_fit_buf(unsigned char arm_id);
static ARM200A_ZERO_FIT_INFO *arm_get_zero_fit_buf(unsigned char arm_id);
static ARM200A_SAMPLE *arm_get_sample_buf(unsigned char arm_id);
static ARM200A_ZERO_SAMPLE *arm_get_zero_sample_buf(unsigned char arm_id);
static unsigned char arm_get_has_o0(unsigned char arm_id);
static void arm_mark_save(void);
static void arm_fit_clear(ARM200A_FIT_INFO *fit);
static void arm_zero_capture_mask(ARM200A_RAW_INPUT *raw, ARM200A_ZERO_CFG *zero, unsigned int mask);
static void arm_zero_convert_one(ARM200A_RAW_INPUT *raw, ARM200A_ZERO_CFG *zero, ARM200A_INPUT *in);
static void arm_copy_raw_to_input(ARM200A_RAW_INPUT *raw, ARM200A_INPUT *in);
static void arm_convert_all_input(void);
static ARM200A_POINT3 arm_apply_coord_point(ARM200A_POINT3 p_body, ARM200A_INPUT *in);
static void arm_body_to_world_result(ARM200A_RESULT *body_out, ARM200A_INPUT *in, ARM200A_RESULT *world_out);
static float arm_angle_to_plane_deg(float numer, float denom_len);
static float arm_angle_to_axis_deg(float numer, float denom_len);
static void arm_update_h1_info(ARM200A_RESULT *world_out, ARM200A_H1_INFO *info);
static void arm_calc_world_from_fix(unsigned char arm_id, ARM200A_FIX *fix, ARM200A_INPUT *in, ARM200A_RESULT *world_out);
static void arm_calc_one(const ARM200A_BASE *base, ARM200A_FIX *fix, ARM200A_INPUT *in, ARM200A_RESULT *out);
static void arm_calc_from_fix(unsigned char arm_id, ARM200A_FIX *fix, ARM200A_INPUT *in, ARM200A_RESULT *out);
static void arm_update_sample_one(unsigned char arm_id, ARM200A_FIX *fix, ARM200A_SAMPLE sample[ARM200A_SAMPLE_MAX], ARM200A_FIT_INFO *fit);
static float arm_fit_score(ARM200A_FIT_INFO *fit);
static int arm_find_empty_sample_index(ARM200A_SAMPLE sample[ARM200A_SAMPLE_MAX]);
static int arm_find_worst_sample_index(unsigned char arm_id, ARM200A_SAMPLE sample[ARM200A_SAMPLE_MAX], ARM200A_FIX *fix);
static void arm_init_fix_step(float step[ARM200A_FIX_NUM], unsigned char has_o0);
static void arm_fit_one(unsigned char arm_id, ARM200A_SAMPLE sample[ARM200A_SAMPLE_MAX], ARM200A_FIX *fix, ARM200A_FIT_INFO *fit);

static const ARM200A_BASE gArmLeftBase =
{
    1,
    { -1055.0f,  340.0f, 1030.0f },
    {   0.0f,  672.056f, -1866.707f },
    { 615.0f,   50.000f,  1005.000f },
    { 165.0f,    0.000f,   702.500f },
    { 200.0f,    0.000f,     0.000f },
    { 265.0f,    0.000f,    20.000f },
    { 488.0f,  118.000f,     0.000f },
    {   0.0f,  275.000f,     0.000f },
    {  80.0f,  200.000f,     0.000f },
    {-985.0f,  230.000f,   129.500f },
    {1831.5f,    0.000f,   447.000f },
    {  80.0f,  200.000f,   100.000f },
    275.0f
};

static const ARM200A_BASE gArmMidBase =
{
    0,
    { 0.0f, 0.0f, 0.0f },
    {   0.0f,    0.000f,     0.000f },
    {   0.0f,    0.000f,     0.000f },
    { 165.0f,    0.000f,   702.500f },
    { 200.0f,    0.000f,     0.000f },
    { 265.0f,    0.000f,    20.000f },
    { 488.0f,  118.000f,     0.000f },
    {   0.0f,  275.000f,     0.000f },
    {  80.0f,  200.000f,     0.000f },
    {-985.0f,  230.000f,   129.500f },
    {1831.5f,    0.000f,   447.000f },
    {  80.0f,  200.000f,   100.000f },
    275.0f
};

static const ARM200A_BASE gArmRightBase =
{
    1,
    { -1055.0f, -340.0f, 1030.0f },
    {   0.0f, -672.056f, -1866.707f },
    { 615.0f,  -50.000f,  1005.000f },
    { 165.0f,    0.000f,   702.500f },
    { 200.0f,    0.000f,     0.000f },
    { 265.0f,    0.000f,    20.000f },
    { 488.0f, -118.000f,     0.000f },
    {   0.0f, -275.000f,     0.000f },
    {  80.0f, -200.000f,     0.000f },
    {-985.0f, -230.000f,   129.500f },
    {1831.5f,    0.000f,   447.000f },
    {  80.0f, -200.000f,   100.000f },
    275.0f
};

static ARM200A_ZERO_CFG arm_make_zero_default(void)
{
    ARM200A_ZERO_CFG z;

    memset(&z, 0, sizeof(z));

    z.root_scale = 1.0f;
    z.base_scale = 1.0f;
    z.boom_scale = 1.0f;
    z.cd_scale = 1.0f;
    z.d1_scale = 1.0f;
    z.e1_scale = 1.0f;
    z.f_scale = 1.0f;
    z.gg1_scale = 1.0f;
    z.body_roll_scale = 1.0f;
    z.body_pitch_scale = 1.0f;

    z.root_sign = 1.0f;
    z.base_sign = 1.0f;
    z.boom_sign = 1.0f;
    z.cd_sign = 1.0f;
    z.d1_sign = 1.0f;
    z.e1_sign = 1.0f;
    z.f_sign = 1.0f;
    z.gg1_sign = 1.0f;
    z.body_roll_sign = 1.0f;
    z.body_pitch_sign = 1.0f;

    z.cd_zero_mm = 3600.0f;
    z.gg1_zero_mm = 2480.0f;
    return z;
}

static float arm_absf(float v)
{
    return (v >= 0.0f) ? v : -v;
}

static float arm_clamp_unit(float v)
{
    if(v > 1.0f)
    {
        return 1.0f;
    }
    if(v < -1.0f)
    {
        return -1.0f;
    }
    return v;
}

static float arm_deg_to_rad(float deg)
{
    return deg * 3.14159265358979323846f / 180.0f;
}

static ARM200A_POINT3 arm_p(float x, float y, float z)
{
    ARM200A_POINT3 p;

    p.x = x;
    p.y = y;
    p.z = z;
    return p;
}

static ARM200A_POINT3 arm_add(ARM200A_POINT3 a, ARM200A_POINT3 b)
{
    return arm_p(a.x + b.x, a.y + b.y, a.z + b.z);
}

static ARM200A_POINT3 arm_sub(ARM200A_POINT3 a, ARM200A_POINT3 b)
{
    return arm_p(a.x - b.x, a.y - b.y, a.z - b.z);
}

static ARM200A_POINT3 arm_scale(ARM200A_POINT3 a, float s)
{
    return arm_p(a.x * s, a.y * s, a.z * s);
}

static float arm_dot(ARM200A_POINT3 a, ARM200A_POINT3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static ARM200A_POINT3 arm_cross(ARM200A_POINT3 a, ARM200A_POINT3 b)
{
    return arm_p(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

static float arm_len(ARM200A_POINT3 a)
{
    return (float)sqrt((double)arm_dot(a, a));
}

static float arm_dist(ARM200A_POINT3 a, ARM200A_POINT3 b)
{
    return arm_len(arm_sub(a, b));
}

static ARM200A_POINT3 arm_norm(ARM200A_POINT3 a)
{
    float n;

    n = arm_len(a);
    if(n < 0.000001f)
    {
        return arm_p(0.0f, 0.0f, 0.0f);
    }
    return arm_scale(a, 1.0f / n);
}

static ARM200A_MAT3 arm_i(void)
{
    ARM200A_MAT3 r;

    memset(&r, 0, sizeof(r));
    r.m[0][0] = 1.0f;
    r.m[1][1] = 1.0f;
    r.m[2][2] = 1.0f;
    return r;
}

static ARM200A_MAT3 arm_mul(ARM200A_MAT3 a, ARM200A_MAT3 b)
{
    ARM200A_MAT3 r;
    int i;
    int j;
    int k;

    memset(&r, 0, sizeof(r));
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            for(k = 0; k < 3; k++)
            {
                r.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return r;
}

static ARM200A_POINT3 arm_mul_p(ARM200A_MAT3 m, ARM200A_POINT3 p)
{
    ARM200A_POINT3 r;

    r.x = m.m[0][0] * p.x + m.m[0][1] * p.y + m.m[0][2] * p.z;
    r.y = m.m[1][0] * p.x + m.m[1][1] * p.y + m.m[1][2] * p.z;
    r.z = m.m[2][0] * p.x + m.m[2][1] * p.y + m.m[2][2] * p.z;
    return r;
}

static ARM200A_MAT3 arm_rx(float deg)
{
    ARM200A_MAT3 r;
    float c;
    float s;

    r = arm_i();
    c = (float)cos((double)arm_deg_to_rad(deg));
    s = (float)sin((double)arm_deg_to_rad(deg));
    r.m[1][1] = c;
    r.m[1][2] = -s;
    r.m[2][1] = s;
    r.m[2][2] = c;
    return r;
}

static ARM200A_MAT3 arm_ry(float deg)
{
    ARM200A_MAT3 r;
    float c;
    float s;

    r = arm_i();
    c = (float)cos((double)arm_deg_to_rad(deg));
    s = (float)sin((double)arm_deg_to_rad(deg));
    r.m[0][0] = c;
    r.m[0][2] = s;
    r.m[2][0] = -s;
    r.m[2][2] = c;
    return r;
}

static ARM200A_MAT3 arm_rz(float deg)
{
    ARM200A_MAT3 r;
    float c;
    float s;

    r = arm_i();
    c = (float)cos((double)arm_deg_to_rad(deg));
    s = (float)sin((double)arm_deg_to_rad(deg));
    r.m[0][0] = c;
    r.m[0][1] = -s;
    r.m[1][0] = s;
    r.m[1][1] = c;
    return r;
}

static ARM200A_MAT3 arm_ra(ARM200A_POINT3 axis, float deg)
{
    ARM200A_MAT3 r;
    ARM200A_POINT3 a;
    float n;
    float c;
    float s;
    float t;

    n = arm_len(axis);
    if(n < 0.000001f)
    {
        return arm_i();
    }

    a = arm_norm(axis);
    c = (float)cos((double)arm_deg_to_rad(deg));
    s = (float)sin((double)arm_deg_to_rad(deg));
    t = 1.0f - c;

    r = arm_i();
    r.m[0][0] = c + a.x * a.x * t;
    r.m[0][1] = a.x * a.y * t - a.z * s;
    r.m[0][2] = a.x * a.z * t + a.y * s;
    r.m[1][0] = a.y * a.x * t + a.z * s;
    r.m[1][1] = c + a.y * a.y * t;
    r.m[1][2] = a.y * a.z * t - a.x * s;
    r.m[2][0] = a.z * a.x * t - a.y * s;
    r.m[2][1] = a.z * a.y * t + a.x * s;
    r.m[2][2] = c + a.z * a.z * t;
    return r;
}

static ARM200A_POINT3 arm_add_fix(ARM200A_POINT3 base, ARM200A_POINT3 fix)
{
    return arm_p(base.x + fix.x, base.y + fix.y, base.z + fix.z);
}

static ARM200A_MAT3 arm_transpose(ARM200A_MAT3 a)
{
    ARM200A_MAT3 r;
    int i;
    int j;

    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 3; j++)
        {
            r.m[i][j] = a.m[j][i];
        }
    }
    return r;
}

static const ARM200A_BASE *arm_get_base_cfg(unsigned char arm_id)
{
    if(arm_id == ARM200A_LEFT)
    {
        return &gArmLeftBase;
    }
    if(arm_id == ARM200A_MID)
    {
        return &gArmMidBase;
    }
    return &gArmRightBase;
}

static ARM200A_INPUT *arm_get_input_buf(unsigned char arm_id)
{
    if(arm_id == ARM200A_LEFT)
    {
        return &gArmLeftIn;
    }
    if(arm_id == ARM200A_MID)
    {
        return &gArmMidIn;
    }
    return &gArmRightIn;
}

static ARM200A_RAW_INPUT *arm_get_raw_buf(unsigned char arm_id)
{
    if(arm_id == ARM200A_LEFT)
    {
        return &gArmLeftRaw;
    }
    if(arm_id == ARM200A_MID)
    {
        return &gArmMidRaw;
    }
    return &gArmRightRaw;
}

static ARM200A_ZERO_CFG *arm_get_zero_cfg_buf(unsigned char arm_id)
{
    if(arm_id == ARM200A_LEFT)
    {
        return &gArmLeftZero;
    }
    if(arm_id == ARM200A_MID)
    {
        return &gArmMidZero;
    }
    return &gArmRightZero;
}

static ARM200A_FIX *arm_get_fix_buf(unsigned char arm_id)
{
    if(arm_id == ARM200A_LEFT)
    {
        return &gArmLeftFix;
    }
    if(arm_id == ARM200A_MID)
    {
        return &gArmMidFix;
    }
    return &gArmRightFix;
}

static ARM200A_FIT_INFO *arm_get_fit_buf(unsigned char arm_id)
{
    if(arm_id == ARM200A_LEFT)
    {
        return &gArmLeftFit;
    }
    if(arm_id == ARM200A_MID)
    {
        return &gArmMidFit;
    }
    return &gArmRightFit;
}

static ARM200A_ZERO_FIT_INFO *arm_get_zero_fit_buf(unsigned char arm_id)
{
    if(arm_id == ARM200A_LEFT)
    {
        return &gArmLeftZeroFit;
    }
    if(arm_id == ARM200A_MID)
    {
        return &gArmMidZeroFit;
    }
    return &gArmRightZeroFit;
}

static ARM200A_SAMPLE *arm_get_sample_buf(unsigned char arm_id)
{
    if(arm_id == ARM200A_LEFT)
    {
        return gArmLeftSample;
    }
    if(arm_id == ARM200A_MID)
    {
        return gArmMidSample;
    }
    return gArmRightSample;
}

static ARM200A_ZERO_SAMPLE *arm_get_zero_sample_buf(unsigned char arm_id)
{
    if(arm_id == ARM200A_LEFT)
    {
        return gArmLeftZeroSample;
    }
    if(arm_id == ARM200A_MID)
    {
        return gArmMidZeroSample;
    }
    return gArmRightZeroSample;
}

static unsigned char arm_get_has_o0(unsigned char arm_id)
{
    if(arm_id == ARM200A_MID)
    {
        return 0;
    }
    return 1;
}

static void arm_mark_save(void)
{
    gLCDPage.vSave = 1;
}

static void arm_fit_clear(ARM200A_FIT_INFO *fit)
{
    memset(fit, 0, sizeof(*fit));
}

/*
 * 坐标转换层初始化。
 * 这里只清空 Body -> World 变换结果，不改正解几何参数。
 */
void Arm200A_CoordInit(void)
{
    memset(&gArmCoordTrans, 0, sizeof(gArmCoordTrans));
}

/*
 * 坐标转换层标定。
 * 当前口径固定为：
 * 1. 用 Q1 Q2 Q3 求 Body -> World 刚体变换
 * 2. 用 Q4 只做校验，不参与求解
 * 3. 输出 q4_err_mm / rms_mm / max_mm / valid
 */
void Arm200A_CoordCalib(void)
{
    ARM200A_POINT3 q1_body;
    ARM200A_POINT3 q2_body;
    ARM200A_POINT3 q3_body;
    ARM200A_POINT3 q4_body;
    ARM200A_POINT3 q1_world;
    ARM200A_POINT3 q2_world;
    ARM200A_POINT3 q3_world;
    ARM200A_POINT3 q4_world;
    ARM200A_POINT3 ex_body;
    ARM200A_POINT3 ey_body;
    ARM200A_POINT3 ez_body;
    ARM200A_POINT3 ex_world;
    ARM200A_POINT3 ey_world;
    ARM200A_POINT3 ez_world;
    ARM200A_POINT3 temp_body;
    ARM200A_POINT3 temp_world;
    ARM200A_MAT3 body_basis;
    ARM200A_MAT3 world_basis;
    ARM200A_MAT3 body_basis_t;
    ARM200A_POINT3 q_pred;
    ARM200A_POINT3 q_err;
    float sum_sq;
    float d;
    float max_mm;
    int i;

    q1_body = arm_p(0.0f, 1160.0f, 380.0f);
    q2_body = arm_p(0.0f, -1160.0f, 380.0f);
    q3_body = arm_p(-1000.0f, 1010.0f, -220.0f);
    q4_body = arm_p(-1000.0f, -1010.0f, -220.0f);

    q1_world = arm_p(gArmQ1WorldX, gArmQ1WorldY, gArmQ1WorldZ);
    q2_world = arm_p(gArmQ2WorldX, gArmQ2WorldY, gArmQ2WorldZ);
    q3_world = arm_p(gArmQ3WorldX, gArmQ3WorldY, gArmQ3WorldZ);
    q4_world = arm_p(gArmQ4WorldX, gArmQ4WorldY, gArmQ4WorldZ);

    ex_body = arm_norm(arm_sub(q2_body, q1_body));
    temp_body = arm_sub(q3_body, q1_body);
    ez_body = arm_norm(arm_cross(ex_body, temp_body));
    ey_body = arm_cross(ez_body, ex_body);

    ex_world = arm_norm(arm_sub(q2_world, q1_world));
    temp_world = arm_sub(q3_world, q1_world);
    ez_world = arm_norm(arm_cross(ex_world, temp_world));
    ey_world = arm_cross(ez_world, ex_world);

    body_basis = arm_i();
    world_basis = arm_i();

    body_basis.m[0][0] = ex_body.x; body_basis.m[1][0] = ex_body.y; body_basis.m[2][0] = ex_body.z;
    body_basis.m[0][1] = ey_body.x; body_basis.m[1][1] = ey_body.y; body_basis.m[2][1] = ey_body.z;
    body_basis.m[0][2] = ez_body.x; body_basis.m[1][2] = ez_body.y; body_basis.m[2][2] = ez_body.z;

    world_basis.m[0][0] = ex_world.x; world_basis.m[1][0] = ex_world.y; world_basis.m[2][0] = ex_world.z;
    world_basis.m[0][1] = ey_world.x; world_basis.m[1][1] = ey_world.y; world_basis.m[2][1] = ey_world.z;
    world_basis.m[0][2] = ez_world.x; world_basis.m[1][2] = ez_world.y; world_basis.m[2][2] = ez_world.z;

    body_basis_t = arm_transpose(body_basis);
    gArmCoordTrans.r[0][0] = 0.0f;
    gArmCoordTrans.r[0][1] = 0.0f;
    gArmCoordTrans.r[0][2] = 0.0f;
    gArmCoordTrans.r[1][0] = 0.0f;
    gArmCoordTrans.r[1][1] = 0.0f;
    gArmCoordTrans.r[1][2] = 0.0f;
    gArmCoordTrans.r[2][0] = 0.0f;
    gArmCoordTrans.r[2][1] = 0.0f;
    gArmCoordTrans.r[2][2] = 0.0f;

    for(i = 0; i < 3; i++)
    {
        int j;
        int k;
        for(j = 0; j < 3; j++)
        {
            for(k = 0; k < 3; k++)
            {
                gArmCoordTrans.r[i][j] += world_basis.m[i][k] * body_basis_t.m[k][j];
            }
        }
    }

    q_pred.x = gArmCoordTrans.r[0][0] * q1_body.x + gArmCoordTrans.r[0][1] * q1_body.y + gArmCoordTrans.r[0][2] * q1_body.z;
    q_pred.y = gArmCoordTrans.r[1][0] * q1_body.x + gArmCoordTrans.r[1][1] * q1_body.y + gArmCoordTrans.r[1][2] * q1_body.z;
    q_pred.z = gArmCoordTrans.r[2][0] * q1_body.x + gArmCoordTrans.r[2][1] * q1_body.y + gArmCoordTrans.r[2][2] * q1_body.z;
    gArmCoordTrans.t = arm_sub(q1_world, q_pred);
    gArmCoordTrans.valid = 1;

    sum_sq = 0.0f;
    max_mm = 0.0f;

    q_pred = arm_apply_coord_point(q1_body, 0);
    q_err = arm_sub(q_pred, q1_world);
    d = arm_len(q_err);
    sum_sq += d * d;
    if(d > max_mm) max_mm = d;

    q_pred = arm_apply_coord_point(q2_body, 0);
    q_err = arm_sub(q_pred, q2_world);
    d = arm_len(q_err);
    sum_sq += d * d;
    if(d > max_mm) max_mm = d;

    q_pred = arm_apply_coord_point(q3_body, 0);
    q_err = arm_sub(q_pred, q3_world);
    d = arm_len(q_err);
    sum_sq += d * d;
    if(d > max_mm) max_mm = d;

    q_pred = arm_apply_coord_point(q4_body, 0);
    q_err = arm_sub(q_pred, q4_world);
    d = arm_len(q_err);
    gArmCoordTrans.q4_err_mm = d;

    gArmCoordTrans.rms_mm = (float)sqrt((double)(sum_sq / 3.0f));
    gArmCoordTrans.max_mm = max_mm;
    gArmCoordReq = 0;
    arm_mark_save();
}

static void arm_zero_fit_clear(ARM200A_ZERO_FIT_INFO *fit)
{
    memset(fit, 0, sizeof(*fit));
}

/*
 * 最基本的零点标定动作。
 * 只把当前 raw 记成零点基准，不在这里做额外拟合。
 */
static void arm_zero_capture_mask(ARM200A_RAW_INPUT *raw, ARM200A_ZERO_CFG *zero, unsigned int mask)
{
    if(mask & ARM200A_ZERO_MASK_ROOT)       zero->root_zero_raw = raw->root_raw;
    if(mask & ARM200A_ZERO_MASK_BASE)       zero->base_zero_raw = raw->base_raw;
    if(mask & ARM200A_ZERO_MASK_BOOM)       zero->boom_zero_raw = raw->boom_raw;
    if(mask & ARM200A_ZERO_MASK_D1)         zero->d1_zero_raw = raw->d1_raw;
    if(mask & ARM200A_ZERO_MASK_E1)         zero->e1_zero_raw = raw->e1_raw;
    if(mask & ARM200A_ZERO_MASK_F)          zero->f_zero_raw = raw->f_raw;
    if(mask & ARM200A_ZERO_MASK_CD)         zero->cd_zero_raw = raw->cd_raw;
    if(mask & ARM200A_ZERO_MASK_GG1)        zero->gg1_zero_raw = raw->gg1_raw;
    if(mask & ARM200A_ZERO_MASK_BODY_ROLL)  zero->body_roll_zero_raw = raw->body_roll_raw;
    if(mask & ARM200A_ZERO_MASK_BODY_PITCH) zero->body_pitch_zero_raw = raw->body_pitch_raw;
    if(mask & ARM200A_ZERO_MASK_HOLE_DEPTH) zero->hole_depth_zero_raw = raw->hole_depth_raw;
}

/*
 * 外设换算。
 * 作用是把 raw 传感器值换成工程量。
 * 这不是误差修正层，只是输入工程化。
 */
static void arm_zero_convert_one(ARM200A_RAW_INPUT *raw, ARM200A_ZERO_CFG *zero, ARM200A_INPUT *in)
{
    in->root_deg = (raw->root_raw - zero->root_zero_raw) * zero->root_scale * zero->root_sign;
    in->base_deg = (raw->base_raw - zero->base_zero_raw) * zero->base_scale * zero->base_sign;
    in->boom_deg = (raw->boom_raw - zero->boom_zero_raw) * zero->boom_scale * zero->boom_sign;
    in->d1_deg = (raw->d1_raw - zero->d1_zero_raw) * zero->d1_scale * zero->d1_sign;
    in->e1_deg = (raw->e1_raw - zero->e1_zero_raw) * zero->e1_scale * zero->e1_sign;
    in->f_deg = (raw->f_raw - zero->f_zero_raw) * zero->f_scale * zero->f_sign;

    in->cd_mm = zero->cd_zero_mm + (raw->cd_raw - zero->cd_zero_raw) * zero->cd_scale * zero->cd_sign;
    in->gg1_mm = zero->gg1_zero_mm + (raw->gg1_raw - zero->gg1_zero_raw) * zero->gg1_scale * zero->gg1_sign;

    in->body_roll_deg = (raw->body_roll_raw - zero->body_roll_zero_raw) * zero->body_roll_scale * zero->body_roll_sign;
    in->body_pitch_deg = (raw->body_pitch_raw - zero->body_pitch_zero_raw) * zero->body_pitch_scale * zero->body_pitch_sign;
}

/*
 * 调试直通路径。
 * 当 gArmUseZeroComp = 0 时，raw 直接当工程量使用。
 */
static void arm_copy_raw_to_input(ARM200A_RAW_INPUT *raw, ARM200A_INPUT *in)
{
    in->root_deg = raw->root_raw;
    in->base_deg = raw->base_raw;
    in->boom_deg = raw->boom_raw;
    in->cd_mm = raw->cd_raw;
    in->d1_deg = raw->d1_raw;
    in->e1_deg = raw->e1_raw;
    in->f_deg = raw->f_raw;
    in->gg1_mm = raw->gg1_raw;
    in->body_roll_deg = raw->body_roll_raw;
    in->body_pitch_deg = raw->body_pitch_raw;
}

/*
 * 三条臂统一做输入换算。
 * 输出结果都落到 gArmLeftIn / MidIn / RightIn。
 */
static void arm_convert_all_input(void)
{
    if(gArmUseZeroComp)
    {
        arm_zero_convert_one(&gArmLeftRaw, &gArmLeftZero, &gArmLeftIn);
        arm_zero_convert_one(&gArmMidRaw, &gArmMidZero, &gArmMidIn);
        arm_zero_convert_one(&gArmRightRaw, &gArmRightZero, &gArmRightIn);
    }
    else
    {
        arm_copy_raw_to_input(&gArmLeftRaw, &gArmLeftIn);
        arm_copy_raw_to_input(&gArmMidRaw, &gArmMidIn);
        arm_copy_raw_to_input(&gArmRightRaw, &gArmRightIn);
    }
}

/*
 * 坐标转换层里的单点变换。
 * 先把 Body 点按双轴实时姿态做增量修正，再乘 Body -> World 变换。
 */
static ARM200A_POINT3 arm_apply_coord_point(ARM200A_POINT3 p_body, ARM200A_INPUT *in)
{
    ARM200A_POINT3 p_now;
    ARM200A_POINT3 p_world;
    ARM200A_MAT3 r_tilt;

    if(gArmCoordTrans.valid == 0)
    {
        return p_body;
    }

    p_now = p_body;
    if(in != 0)
    {
        r_tilt = arm_mul(arm_ry(in->body_pitch_deg), arm_rx(in->body_roll_deg));
        p_now = arm_mul_p(r_tilt, p_body);
    }

    p_world.x = gArmCoordTrans.r[0][0] * p_now.x + gArmCoordTrans.r[0][1] * p_now.y + gArmCoordTrans.r[0][2] * p_now.z + gArmCoordTrans.t.x;
    p_world.y = gArmCoordTrans.r[1][0] * p_now.x + gArmCoordTrans.r[1][1] * p_now.y + gArmCoordTrans.r[1][2] * p_now.z + gArmCoordTrans.t.y;
    p_world.z = gArmCoordTrans.r[2][0] * p_now.x + gArmCoordTrans.r[2][1] * p_now.y + gArmCoordTrans.r[2][2] * p_now.z + gArmCoordTrans.t.z;
    return p_world;
}

/*
 * 把一整套 Body 点位结果批量变成 World/大地坐标点位结果。
 *
 * 这里的职责非常明确：
 * 1. 不重算正解几何链。
 * 2. 不改局部铰接关系。
 * 3. 只把 Body 坐标结果整体映射到 World/大地坐标。
 *
 * 当前世界系变换链口径：
 * - 先按双轴实时姿态 body_roll_deg / body_pitch_deg 做车体倾斜补偿
 * - 再乘 Q 点标定得到的 Body -> World 刚体变换
 *
 * 补充说明：
 * - h1 和 h 一样，也必须一起做这套变换。
 * - 否则后面用 world.h - world.h1 计算 H1/H 在大地坐标系下的方向和夹角时，
 *   会出现 H 已经进世界系、H1 还停在车体系的口径错误。
 */
static void arm_body_to_world_result(ARM200A_RESULT *body_out, ARM200A_INPUT *in, ARM200A_RESULT *world_out)
{
    *world_out = *body_out;
    world_out->o0 = arm_apply_coord_point(body_out->o0, in);
    world_out->o1 = arm_apply_coord_point(body_out->o1, in);
    world_out->o = arm_apply_coord_point(body_out->o, in);
    world_out->b = arm_apply_coord_point(body_out->b, in);
    world_out->a = arm_apply_coord_point(body_out->a, in);
    world_out->c = arm_apply_coord_point(body_out->c, in);
    world_out->d = arm_apply_coord_point(body_out->d, in);
    world_out->d1 = arm_apply_coord_point(body_out->d1, in);
    world_out->e = arm_apply_coord_point(body_out->e, in);
    world_out->e1 = arm_apply_coord_point(body_out->e1, in);
    world_out->f = arm_apply_coord_point(body_out->f, in);
    world_out->g = arm_apply_coord_point(body_out->g, in);
    world_out->g1 = arm_apply_coord_point(body_out->g1, in);
    world_out->h1 = arm_apply_coord_point(body_out->h1, in);
    world_out->h = arm_apply_coord_point(body_out->h, in);
}

/*
 * 计算向量与某个坐标平面的夹角。
 *
 * 使用的是严格线面角定义：
 * - 线与平面的夹角 = 线与其在平面上投影之间的最小夹角
 * - 取值范围 0 ~ 90 度
 *
 * 这里传入的 numer 含义是：
 * - 与 XY 平面夹角：传 z 分量
 * - 与 XZ 平面夹角：传 y 分量
 * - 与 YZ 平面夹角：传 x 分量
 *
 * denom_len 是整根向量长度。
 */
static float arm_angle_to_plane_deg(float numer, float denom_len)
{
    if(denom_len < 0.000001f)
    {
        return 0.0f;
    }
    return (float)(asin((double)arm_clamp_unit(arm_absf(numer) / denom_len)) * 180.0 / 3.14159265358979323846);
}

/*
 * 计算向量与某个坐标轴的夹角。
 *
 * 公式：
 * angle = acos(axis_component / vector_length)
 *
 * 这里传入的 numer 含义是：
 * - 与 X 轴夹角：传 x 分量
 * - 与 Y 轴夹角：传 y 分量
 * - 与 Z 轴夹角：传 z 分量
 */
static float arm_angle_to_axis_deg(float numer, float denom_len)
{
    if(denom_len < 0.000001f)
    {
        return 0.0f;
    }
    return (float)(acos((double)arm_clamp_unit(numer / denom_len)) * 180.0 / 3.14159265358979323846);
}

/*
 * 根据世界系结果更新 H1/H 派生量。
 *
 * 这一步的输入必须是 World/大地坐标系结果，而不能是 Body 结果。
 * 原因是当前项目要看的量是：
 * - H1->H 在大地坐标系下的向量
 * - H1->H 与大地固定 XY/XZ/YZ 平面的夹角
 * - H1->H 与大地固定 X/Y/Z 轴的夹角
 *
 * 因此这里只接受 world_out，并按下面顺序做：
 * 1. 取出大地系下的 h 与 h1
 * 2. 形成大地系向量 vec = h - h1
 * 3. 用 vec 计算三平面夹角
 * 4. 用 vec 计算三轴夹角
 *
 * 注意：
 * - 这里不再单独处理双轴，因为 world_out 里已经包含双轴补偿。
 * - 这里也不再关心局部 (300,0,0)，因为 h1 已经在前面按局部姿态算完了。
 */
static void arm_update_h1_info(ARM200A_RESULT *world_out, ARM200A_H1_INFO *info)
{
    ARM200A_H1_INFO tmp;
    ARM200A_POINT3 vec;
    float len_mm;

    memset(&tmp, 0, sizeof(tmp));
    if(world_out->ok == 0)
    {
        *info = tmp;
        return;
    }

    tmp.h1 = world_out->h1;
    vec = arm_sub(world_out->h, world_out->h1);
    len_mm = arm_len(vec);

    tmp.h1_to_h_world = vec;
    /* 大地固定平面夹角。 */
    tmp.angle_xy_plane_deg = arm_angle_to_plane_deg(vec.z, len_mm);
    tmp.angle_xz_plane_deg = arm_angle_to_plane_deg(vec.y, len_mm);
    tmp.angle_yz_plane_deg = arm_angle_to_plane_deg(vec.x, len_mm);
    /* 大地固定三轴夹角。 */
    tmp.angle_x_axis_deg = arm_angle_to_axis_deg(vec.x, len_mm);
    tmp.angle_y_axis_deg = arm_angle_to_axis_deg(vec.y, len_mm);
    tmp.angle_z_axis_deg = arm_angle_to_axis_deg(vec.z, len_mm);
    tmp.ok = 1;

    *info = tmp;
}

/*
 * 给样本回代和拟合评估使用的辅助函数。
 * 先算 Body 正解，再转成 World 结果。
 */
static void arm_calc_world_from_fix(unsigned char arm_id, ARM200A_FIX *fix, ARM200A_INPUT *in, ARM200A_RESULT *world_out)
{
    ARM200A_RESULT body_out;

    arm_calc_from_fix(arm_id, fix, in, &body_out);
    arm_body_to_world_result(&body_out, in, world_out);
}

/*
 * 调试/拟合后统一刷新入口。
 * 顺序固定：
 * 1. 输入换算
 * 2. 正解算 Body
 * 3. 坐标转换算 World
 * 4. 更新样本统计
 */
static void arm_refresh_all_now(void)
{
    Arm200A_CalcLeft();
    Arm200A_CalcMid();
    Arm200A_CalcRight();
    Arm200A_UpdateSampleNow();
}

static void arm_zero_sample_to_input(ARM200A_ZERO_SAMPLE *sample, ARM200A_ZERO_CFG *zero, ARM200A_INPUT *in)
{
    in->root_deg = (sample->root_raw - zero->root_zero_raw) * zero->root_scale * zero->root_sign;
    in->base_deg = (sample->base_raw - zero->base_zero_raw) * zero->base_scale * zero->base_sign;
    in->boom_deg = (sample->boom_raw - zero->boom_zero_raw) * zero->boom_scale * zero->boom_sign;
    in->d1_deg = (sample->d1_raw - zero->d1_zero_raw) * zero->d1_scale * zero->d1_sign;
    in->e1_deg = (sample->e1_raw - zero->e1_zero_raw) * zero->e1_scale * zero->e1_sign;
    in->f_deg = (sample->f_raw - zero->f_zero_raw) * zero->f_scale * zero->f_sign;

    in->cd_mm = zero->cd_zero_mm + (sample->cd_raw - zero->cd_zero_raw) * zero->cd_scale * zero->cd_sign;
    in->gg1_mm = zero->gg1_zero_mm + (sample->gg1_raw - zero->gg1_zero_raw) * zero->gg1_scale * zero->gg1_sign;

    in->body_roll_deg = (sample->body_roll_raw - zero->body_roll_zero_raw) * zero->body_roll_scale * zero->body_roll_sign;
    in->body_pitch_deg = (sample->body_pitch_raw - zero->body_pitch_zero_raw) * zero->body_pitch_scale * zero->body_pitch_sign;
}

static ARM200A_POINT3 arm_zero_target_h(unsigned char arm_id, ARM200A_INPUT *sample_in)
{
    ARM200A_INPUT in;
    ARM200A_RESULT out;
    ARM200A_ZERO_CFG *zero;
    ARM200A_FIX *fix;

    memset(&in, 0, sizeof(in));
    zero = arm_get_zero_cfg_buf(arm_id);
    fix = arm_get_fix_buf(arm_id);

    in.cd_mm = zero->cd_zero_mm;
    in.gg1_mm = zero->gg1_zero_mm;
    if(sample_in != 0)
    {
        in.body_roll_deg = sample_in->body_roll_deg;
        in.body_pitch_deg = sample_in->body_pitch_deg;
    }

    arm_calc_from_fix(arm_id, fix, &in, &out);
    return out.h;
}

/*
 * 正解核心。
 *
 * 这里只负责在统一车体坐标系下算点位结果，也就是 Body 坐标。
 * 双轴姿态补偿不在这里做，而是在坐标转换层里做。
 *
 * 关于 h1 的特别说明：
 * 1. h1 不是额外铰接点，不参与原正解链的旋转自由度。
 * 2. h1 是相对 h 的一个局部刚性附加点。
 * 3. 当前约定：
 *    - 在末端局部坐标系里，H1->H = (300, 0, 0)
 * 4. 因此在 Body 坐标里，h1 的求法是：
 *    - 先得到当前末端旋转矩阵 r_f
 *    - 再做 h1 = h - r_f * (300,0,0)
 *
 * 这样处理后：
 * - h1 会跟随 h 一起刚性运动
 * - 后续再统一进入 World/大地坐标系
 * - 最终可直接得到大地系下 H1->H 的方向和角度
 */
static void arm_calc_one(const ARM200A_BASE *base, ARM200A_FIX *fix, ARM200A_INPUT *in, ARM200A_RESULT *out)
{
    ARM200A_MAT3 r_root;
    ARM200A_MAT3 r_b;
    ARM200A_MAT3 r_a;
    ARM200A_MAT3 r_d1;
    ARM200A_MAT3 r_e1;
    ARM200A_MAT3 r_f;
    ARM200A_POINT3 v_o0o1;
    ARM200A_POINT3 v_o1o;
    ARM200A_POINT3 v_ob;
    ARM200A_POINT3 v_ba;
    ARM200A_POINT3 v_ac;
    ARM200A_POINT3 v_d1e;
    ARM200A_POINT3 v_ee1;
    ARM200A_POINT3 v_e1f;
    ARM200A_POINT3 v_fg;
    ARM200A_POINT3 v_g1h;
    ARM200A_POINT3 v_e1e2;
    ARM200A_POINT3 axis_cd_local;
    ARM200A_POINT3 axis_ee1_local;
    ARM200A_POINT3 axis_fe2_local;
    float dd1_mm;

    v_o0o1 = arm_add_fix(base->o0o1, fix->o0o1);
    v_o1o = arm_add_fix(base->o1o, fix->o1o);
    v_ob = arm_add_fix(base->ob, fix->ob);
    v_ba = arm_add_fix(base->ba, fix->ba);
    v_ac = arm_add_fix(base->ac, fix->ac);
    v_d1e = arm_add_fix(base->d1e, fix->d1e);
    v_ee1 = arm_add_fix(base->ee1, fix->ee1);
    v_e1f = arm_add_fix(base->e1f, fix->e1f);
    v_fg = arm_add_fix(base->fg, fix->fg);
    v_g1h = arm_add_fix(base->g1h, fix->g1h);
    v_e1e2 = arm_add_fix(base->e1e2, fix->e1e2);
    dd1_mm = base->dd1_mm + fix->dd1_mm;

    out->has_o0 = base->has_o0;
    out->ok = 0;
    out->o0 = base->o0_body;

    if(base->has_o0)
    {
        out->o1 = arm_add(out->o0, arm_mul_p(arm_rx(in->root_deg), v_o0o1));
        out->o = arm_add(out->o1, v_o1o);
        r_root = arm_i();
    }
    else
    {
        out->o1 = arm_p(0.0f, 0.0f, 0.0f);
        out->o = arm_p(0.0f, 0.0f, 0.0f);
        r_root = arm_i();
    }

    out->b = arm_add(out->o, arm_mul_p(r_root, v_ob));

    r_b = arm_mul(r_root, arm_rz(in->base_deg));
    out->a = arm_add(out->b, arm_mul_p(r_b, v_ba));

    r_a = arm_mul(r_b, arm_ry(in->boom_deg));
    out->c = arm_add(out->a, arm_mul_p(r_a, v_ac));

    out->d = arm_add(out->c, arm_mul_p(r_a, arm_p(in->cd_mm, 0.0f, 0.0f)));
    out->d1 = arm_add(out->d, arm_mul_p(r_a, arm_p(dd1_mm, 0.0f, 0.0f)));

    axis_cd_local = arm_p(1.0f, 0.0f, 0.0f);
    r_d1 = arm_mul(r_a, arm_ra(axis_cd_local, in->d1_deg));
    out->e = arm_add(out->d1, arm_mul_p(r_d1, v_d1e));

    out->e1 = arm_add(out->e, arm_mul_p(r_d1, v_ee1));

    axis_ee1_local = v_ee1;
    r_e1 = arm_mul(r_d1, arm_ra(axis_ee1_local, in->e1_deg));
    out->f = arm_add(out->e1, arm_mul_p(r_e1, v_e1f));

    axis_fe2_local = arm_sub(v_e1e2, v_e1f);
    r_f = arm_mul(r_e1, arm_ra(axis_fe2_local, in->f_deg));
    out->g = arm_add(out->f, arm_mul_p(r_f, v_fg));

    out->g1 = arm_add(out->g, arm_mul_p(r_f, arm_p(in->gg1_mm, 0.0f, 0.0f)));
    out->h = arm_add(out->g1, arm_mul_p(r_f, v_g1h));

    /*
     * h1 的 Body 坐标定义。
     *
     * 这里固定按：
     * - H1->H 局部向量 = (300,0,0)
     * - 也就是 H 到 H1 在局部里是 (-300,0,0)
     *
     * 当前 r_f 就代表末端这一级的局部姿态，因此：
     * - r_f * (300,0,0) = 当前姿态下的 Body 系 H1->H 向量
     * - h1 = h - 该向量
     */
    out->h1 = arm_sub(out->h, arm_mul_p(r_f, arm_p(300.0f, 0.0f, 0.0f)));

    out->ok = 1;
}

static void arm_calc_from_fix(unsigned char arm_id, ARM200A_FIX *fix, ARM200A_INPUT *in, ARM200A_RESULT *out)
{
    const ARM200A_BASE *base;

    base = arm_get_base_cfg(arm_id);
    arm_calc_one(base, fix, in, out);
}

/*
 * 用当前修正量回代样本池。
 * 这里比较的是 world-H，而不是 body-H。
 */
static void arm_update_sample_one(unsigned char arm_id, ARM200A_FIX *fix, ARM200A_SAMPLE sample[ARM200A_SAMPLE_MAX], ARM200A_FIT_INFO *fit)
{
    ARM200A_RESULT out;
    ARM200A_POINT3 err;
    float sum_sq;
    float max_mm;
    float d;
    int i;
    int count;

    count = 0;
    sum_sq = 0.0f;
    max_mm = 0.0f;

    for(i = 0; i < ARM200A_SAMPLE_MAX; i++)
    {
        if(sample[i].use == 0)
        {
            continue;
        }

        arm_calc_world_from_fix(arm_id, fix, &sample[i].in, &out);
        err = arm_sub(out.h, sample[i].h_ref);
        d = arm_len(err);
        sum_sq += d * d;
        if(d > max_mm)
        {
            max_mm = d;
        }
        count++;
    }

    fit->count = (unsigned char)count;
    fit->rms_mm = (count > 0) ? (float)sqrt((double)(sum_sq / (float)count)) : 0.0f;
    fit->max_mm = max_mm;
    fit->ok = (count >= ARM200A_SAMPLE_MIN_FIT && max_mm <= 30.0f) ? 1 : 0;
}

static void arm_update_zero_sample_one(unsigned char arm_id, ARM200A_ZERO_SAMPLE sample[ARM200A_ZERO_SAMPLE_MAX], ARM200A_ZERO_CFG *zero, ARM200A_ZERO_FIT_INFO *fit)
{
    ARM200A_POINT3 href;
    ARM200A_POINT3 err;
    ARM200A_INPUT in;
    ARM200A_RESULT out;
    ARM200A_FIX *fix;
    float sum_sq;
    float max_mm;
    float d;
    int i;
    int count;

    fix = arm_get_fix_buf(arm_id);
    sum_sq = 0.0f;
    max_mm = 0.0f;
    count = 0;

    for(i = 0; i < ARM200A_ZERO_SAMPLE_MAX; i++)
    {
        if(sample[i].use == 0)
        {
            continue;
        }

        arm_zero_sample_to_input(&sample[i], zero, &in);
        href = arm_zero_target_h(arm_id, &in);
        arm_calc_from_fix(arm_id, fix, &in, &out);
        err = arm_sub(out.h, href);
        d = arm_len(err);
        sum_sq += d * d;
        if(d > max_mm)
        {
            max_mm = d;
        }
        count++;
    }

    fit->count = (unsigned char)count;
    fit->rms_mm = (count > 0) ? (float)sqrt((double)(sum_sq / (float)count)) : 0.0f;
    fit->max_mm = max_mm;
    fit->ok = (count > 0 && max_mm <= 3.0f) ? 1 : 0;
}

static float arm_fit_score(ARM200A_FIT_INFO *fit)
{
    if(fit->count == 0)
    {
        return 1000000000.0f;
    }
    return fit->rms_mm + 0.10f * fit->max_mm;
}

static float arm_zero_fit_score(ARM200A_ZERO_FIT_INFO *fit)
{
    if(fit->count == 0)
    {
        return 1000000000.0f;
    }
    return fit->rms_mm + 0.20f * fit->max_mm;
}

static int arm_find_empty_sample_index(ARM200A_SAMPLE sample[ARM200A_SAMPLE_MAX])
{
    int i;

    for(i = 0; i < ARM200A_SAMPLE_MAX; i++)
    {
        if(sample[i].use == 0)
        {
            return i;
        }
    }
    return -1;
}

/*
 * 当样本池已满时，找出当前“最差”的那条加工误差样本。
 * 这里必须和正式拟合评估保持同一口径：
 * - 样本里的 h_ref 是全站仪给的 World-H
 * - 所以这里也必须用 World-H 回代后再比较误差
 * 否则会出现：
 * - 替换逻辑按 Body-H 选最差
 * - 拟合逻辑按 World-H 算误差
 * 两套标准不一致，可能把不该替换的样本替换掉。
 */
static int arm_find_worst_sample_index(unsigned char arm_id, ARM200A_SAMPLE sample[ARM200A_SAMPLE_MAX], ARM200A_FIX *fix)
{
    ARM200A_RESULT out;
    float worst_err;
    float now_err;
    int worst_index;
    int i;

    worst_index = 0;
    worst_err = -1.0f;

    for(i = 0; i < ARM200A_SAMPLE_MAX; i++)
    {
        if(sample[i].use == 0)
        {
            continue;
        }

        arm_calc_world_from_fix(arm_id, fix, &sample[i].in, &out);
        now_err = arm_dist(out.h, sample[i].h_ref);
        if(now_err > worst_err)
        {
            worst_err = now_err;
            worst_index = i;
        }
    }

    return worst_index;
}

static int arm_find_empty_zero_sample_index(ARM200A_ZERO_SAMPLE sample[ARM200A_ZERO_SAMPLE_MAX])
{
    int i;

    for(i = 0; i < ARM200A_ZERO_SAMPLE_MAX; i++)
    {
        if(sample[i].use == 0)
        {
            return i;
        }
    }
    return -1;
}

static int arm_find_worst_zero_sample_index(unsigned char arm_id, ARM200A_ZERO_SAMPLE sample[ARM200A_ZERO_SAMPLE_MAX], ARM200A_ZERO_CFG *zero)
{
    ARM200A_POINT3 href;
    ARM200A_INPUT in;
    ARM200A_RESULT out;
    ARM200A_FIX *fix;
    float worst_err;
    float now_err;
    int worst_index;
    int i;

    fix = arm_get_fix_buf(arm_id);
    worst_index = 0;
    worst_err = -1.0f;

    for(i = 0; i < ARM200A_ZERO_SAMPLE_MAX; i++)
    {
        if(sample[i].use == 0)
        {
            continue;
        }

        arm_zero_sample_to_input(&sample[i], zero, &in);
        href = arm_zero_target_h(arm_id, &in);
        arm_calc_from_fix(arm_id, fix, &in, &out);
        now_err = arm_dist(out.h, href);
        if(now_err > worst_err)
        {
            worst_err = now_err;
            worst_index = i;
        }
    }

    return worst_index;
}

void Arm200A_FixToArray(ARM200A_FIX *fix, float out_scalar[ARM200A_FIX_NUM], unsigned char has_o0)
{
    int i;

    for(i = 0; i < ARM200A_FIX_NUM; i++)
    {
        out_scalar[i] = 0.0f;
    }

    if(has_o0)
    {
        out_scalar[0] = fix->o0o1.y;
        out_scalar[1] = fix->o0o1.z;
        out_scalar[2] = fix->o1o.x;
        out_scalar[3] = fix->o1o.y;
        out_scalar[4] = fix->o1o.z;
    }

    out_scalar[5] = fix->ob.x;
    out_scalar[6] = fix->ob.z;
    out_scalar[7] = fix->ba.x;
    out_scalar[8] = fix->ac.x;
    out_scalar[9] = fix->ac.z;
    out_scalar[10] = fix->d1e.x;
    out_scalar[11] = fix->d1e.y;
    out_scalar[12] = fix->ee1.y;
    out_scalar[13] = fix->e1f.x;
    out_scalar[14] = fix->e1f.y;
    out_scalar[15] = fix->fg.x;
    out_scalar[16] = fix->fg.y;
    out_scalar[17] = fix->fg.z;
    out_scalar[18] = fix->g1h.x;
    out_scalar[19] = fix->g1h.z;
    out_scalar[20] = fix->dd1_mm;
    out_scalar[21] = fix->e1e2.z;
}

void Arm200A_ArrayToFix(float in_scalar[ARM200A_FIX_NUM], ARM200A_FIX *fix, unsigned char has_o0)
{
    memset(fix, 0, sizeof(*fix));

    if(has_o0)
    {
        fix->o0o1.y = in_scalar[0];
        fix->o0o1.z = in_scalar[1];
        fix->o1o.x = in_scalar[2];
        fix->o1o.y = in_scalar[3];
        fix->o1o.z = in_scalar[4];
    }

    fix->ob.x = in_scalar[5];
    fix->ob.z = in_scalar[6];
    fix->ba.x = in_scalar[7];
    fix->ac.x = in_scalar[8];
    fix->ac.z = in_scalar[9];
    fix->d1e.x = in_scalar[10];
    fix->d1e.y = in_scalar[11];
    fix->ee1.y = in_scalar[12];
    fix->e1f.x = in_scalar[13];
    fix->e1f.y = in_scalar[14];
    fix->fg.x = in_scalar[15];
    fix->fg.y = in_scalar[16];
    fix->fg.z = in_scalar[17];
    fix->g1h.x = in_scalar[18];
    fix->g1h.z = in_scalar[19];
    fix->dd1_mm = in_scalar[20];
    fix->e1e2.z = in_scalar[21];
}

static void arm_zero_cfg_to_array(ARM200A_ZERO_CFG *zero, float out_scalar[ARM200A_ZERO_FIT_NUM], unsigned char has_o0)
{
    int i;

    for(i = 0; i < ARM200A_ZERO_FIT_NUM; i++)
    {
        out_scalar[i] = 0.0f;
    }

    if(has_o0)
    {
        out_scalar[0] = zero->root_zero_raw;
    }
    out_scalar[1] = zero->base_zero_raw;
    out_scalar[2] = zero->boom_zero_raw;
    out_scalar[3] = zero->d1_zero_raw;
    out_scalar[4] = zero->e1_zero_raw;
    out_scalar[5] = zero->f_zero_raw;
    out_scalar[6] = zero->cd_zero_raw;
    out_scalar[7] = zero->gg1_zero_raw;
}

static void arm_array_to_zero_cfg(float in_scalar[ARM200A_ZERO_FIT_NUM], ARM200A_ZERO_CFG *zero, unsigned char has_o0)
{
    if(has_o0)
    {
        zero->root_zero_raw = in_scalar[0];
    }
    zero->base_zero_raw = in_scalar[1];
    zero->boom_zero_raw = in_scalar[2];
    zero->d1_zero_raw = in_scalar[3];
    zero->e1_zero_raw = in_scalar[4];
    zero->f_zero_raw = in_scalar[5];
    zero->cd_zero_raw = in_scalar[6];
    zero->gg1_zero_raw = in_scalar[7];
}

static void arm_init_fix_step(float step[ARM200A_FIX_NUM], unsigned char has_o0)
{
    int i;

    for(i = 0; i < ARM200A_FIX_NUM; i++)
    {
        step[i] = 10.0f;
    }

    step[20] = 5.0f;
    step[21] = 5.0f;

    if(has_o0 == 0)
    {
        step[0] = 0.0f;
        step[1] = 0.0f;
        step[2] = 0.0f;
        step[3] = 0.0f;
        step[4] = 0.0f;
    }
}

static float arm_raw_step_from_scale(float scale, float physical_step)
{
    float s;

    s = arm_absf(scale);
    if(s < 0.000001f)
    {
        return physical_step;
    }
    return physical_step / s;
}

static void arm_init_zero_step(ARM200A_ZERO_CFG *zero, float step[ARM200A_ZERO_FIT_NUM], unsigned char has_o0)
{
    step[0] = has_o0 ? arm_raw_step_from_scale(zero->root_scale, 0.2f) : 0.0f;
    step[1] = arm_raw_step_from_scale(zero->base_scale, 0.2f);
    step[2] = arm_raw_step_from_scale(zero->boom_scale, 0.2f);
    step[3] = arm_raw_step_from_scale(zero->d1_scale, 0.2f);
    step[4] = arm_raw_step_from_scale(zero->e1_scale, 0.2f);
    step[5] = arm_raw_step_from_scale(zero->f_scale, 0.2f);
    step[6] = arm_raw_step_from_scale(zero->cd_scale, 5.0f);
    step[7] = arm_raw_step_from_scale(zero->gg1_scale, 5.0f);
}

/*
 * 加工误差拟合。
 * 逻辑是：
 * 1. 至少 20 条样本才允许正式拟合
 * 2. 用 world-H 和全站仪 world-H 比较
 * 3. 修正对象是机械固定参数 fix
 */
static void arm_fit_one(unsigned char arm_id, ARM200A_SAMPLE sample[ARM200A_SAMPLE_MAX], ARM200A_FIX *fix, ARM200A_FIT_INFO *fit)
{
    ARM200A_FIT_INFO best_fit;
    ARM200A_FIT_INFO try_fit;
    ARM200A_FIX try_fix;
    float cur_scalar[ARM200A_FIX_NUM];
    float try_scalar[ARM200A_FIX_NUM];
    float step[ARM200A_FIX_NUM];
    float best_score;
    float try_score;
    float step_sum;
    float max_step;
    float best_value;
    float best_delta;
    unsigned char has_o0;
    int valid_count;
    int round_id;
    int index;
    int active_count;
    int changed;

    valid_count = Arm200A_GetSampleCount(arm_id);
    arm_fit_clear(fit);
    fit->count = (unsigned char)valid_count;
    if(valid_count < ARM200A_SAMPLE_MIN_FIT)
    {
        return;
    }

    has_o0 = arm_get_has_o0(arm_id);
    Arm200A_FixToArray(fix, cur_scalar, has_o0);
    arm_init_fix_step(step, has_o0);
    arm_update_sample_one(arm_id, fix, sample, &best_fit);
    best_score = arm_fit_score(&best_fit);
    fit->last_step_mm = 0.0f;

    for(round_id = 0; round_id < 20; round_id++)
    {
        changed = 0;
        step_sum = 0.0f;
        active_count = 0;

        for(index = 0; index < ARM200A_FIX_NUM; index++)
        {
            if(step[index] < 0.02f)
            {
                continue;
            }

            if(has_o0 == 0 && index < 5)
            {
                continue;
            }

            active_count++;
            best_value = cur_scalar[index];
            best_delta = 0.0f;

            memcpy(try_scalar, cur_scalar, sizeof(try_scalar));
            try_scalar[index] = cur_scalar[index] + step[index];
            Arm200A_ArrayToFix(try_scalar, &try_fix, has_o0);
            arm_update_sample_one(arm_id, &try_fix, sample, &try_fit);
            try_score = arm_fit_score(&try_fit);
            if(try_score + 0.0001f < best_score)
            {
                best_score = try_score;
                best_fit = try_fit;
                *fix = try_fix;
                best_value = try_scalar[index];
                best_delta = step[index];
            }

            memcpy(try_scalar, cur_scalar, sizeof(try_scalar));
            try_scalar[index] = cur_scalar[index] - step[index];
            Arm200A_ArrayToFix(try_scalar, &try_fix, has_o0);
            arm_update_sample_one(arm_id, &try_fix, sample, &try_fit);
            try_score = arm_fit_score(&try_fit);
            if(try_score + 0.0001f < best_score)
            {
                best_score = try_score;
                best_fit = try_fit;
                *fix = try_fix;
                best_value = try_scalar[index];
                best_delta = -step[index];
            }

            if(best_delta != 0.0f)
            {
                cur_scalar[index] = best_value;
                changed = 1;
                step_sum += arm_absf(best_delta);
            }
        }

        if(active_count > 0)
        {
            fit->last_step_mm = step_sum / (float)active_count;
        }

        if(changed == 0)
        {
            max_step = 0.0f;
            for(index = 0; index < ARM200A_FIX_NUM; index++)
            {
                step[index] *= 0.5f;
                if(step[index] > max_step)
                {
                    max_step = step[index];
                }
            }

            if(max_step < 0.02f)
            {
                break;
            }
        }
    }

    Arm200A_ArrayToFix(cur_scalar, fix, has_o0);
    arm_update_sample_one(arm_id, fix, sample, fit);
    fit->ok = (fit->count >= ARM200A_SAMPLE_MIN_FIT && fit->max_mm <= 30.0f) ? 1 : 0;
}

static void arm_zero_fit_one(unsigned char arm_id, ARM200A_ZERO_SAMPLE sample[ARM200A_ZERO_SAMPLE_MAX], ARM200A_ZERO_CFG *zero, ARM200A_ZERO_FIT_INFO *fit)
{
    ARM200A_ZERO_FIT_INFO best_fit;
    ARM200A_ZERO_FIT_INFO try_fit;
    ARM200A_ZERO_CFG try_zero;
    float cur_scalar[ARM200A_ZERO_FIT_NUM];
    float try_scalar[ARM200A_ZERO_FIT_NUM];
    float step[ARM200A_ZERO_FIT_NUM];
    float best_score;
    float try_score;
    float best_value;
    float best_delta;
    float max_step;
    unsigned char has_o0;
    int valid_count;
    int round_id;
    int index;
    int changed;

    valid_count = 0;
    for(index = 0; index < ARM200A_ZERO_SAMPLE_MAX; index++)
    {
        if(sample[index].use)
        {
            valid_count++;
        }
    }

    arm_zero_fit_clear(fit);
    fit->count = (unsigned char)valid_count;
    if(valid_count < 1)
    {
        return;
    }

    has_o0 = arm_get_has_o0(arm_id);
    arm_zero_cfg_to_array(zero, cur_scalar, has_o0);
    arm_init_zero_step(zero, step, has_o0);
    arm_update_zero_sample_one(arm_id, sample, zero, &best_fit);
    best_score = arm_zero_fit_score(&best_fit);

    for(round_id = 0; round_id < 20; round_id++)
    {
        changed = 0;

        for(index = 0; index < ARM200A_ZERO_FIT_NUM; index++)
        {
            if(step[index] < 0.01f)
            {
                continue;
            }

            if(has_o0 == 0 && index == 0)
            {
                continue;
            }

            best_value = cur_scalar[index];
            best_delta = 0.0f;

            memcpy(try_scalar, cur_scalar, sizeof(try_scalar));
            try_scalar[index] = cur_scalar[index] + step[index];
            try_zero = *zero;
            arm_array_to_zero_cfg(try_scalar, &try_zero, has_o0);
            arm_update_zero_sample_one(arm_id, sample, &try_zero, &try_fit);
            try_score = arm_zero_fit_score(&try_fit);
            if(try_score + 0.0001f < best_score)
            {
                best_score = try_score;
                best_fit = try_fit;
                *zero = try_zero;
                best_value = try_scalar[index];
                best_delta = step[index];
            }

            memcpy(try_scalar, cur_scalar, sizeof(try_scalar));
            try_scalar[index] = cur_scalar[index] - step[index];
            try_zero = *zero;
            arm_array_to_zero_cfg(try_scalar, &try_zero, has_o0);
            arm_update_zero_sample_one(arm_id, sample, &try_zero, &try_fit);
            try_score = arm_zero_fit_score(&try_fit);
            if(try_score + 0.0001f < best_score)
            {
                best_score = try_score;
                best_fit = try_fit;
                *zero = try_zero;
                best_value = try_scalar[index];
                best_delta = -step[index];
            }

            if(best_delta != 0.0f)
            {
                cur_scalar[index] = best_value;
                changed = 1;
            }
        }

        if(changed == 0)
        {
            max_step = 0.0f;
            for(index = 0; index < ARM200A_ZERO_FIT_NUM; index++)
            {
                step[index] *= 0.5f;
                if(step[index] > max_step)
                {
                    max_step = step[index];
                }
            }

            if(max_step < 0.01f)
            {
                break;
            }
        }
    }

    arm_array_to_zero_cfg(cur_scalar, zero, has_o0);
    arm_update_zero_sample_one(arm_id, sample, zero, fit);
    fit->ok = (fit->count > 0 && fit->max_mm <= 3.0f) ? 1 : 0;
}

/*
 * 零点与换算参数的默认初始化。
 */
void Arm200A_ZeroInit(void)
{
    memset(&gArmLeftRaw, 0, sizeof(gArmLeftRaw));
    memset(&gArmMidRaw, 0, sizeof(gArmMidRaw));
    memset(&gArmRightRaw, 0, sizeof(gArmRightRaw));

    gArmLeftZero = arm_make_zero_default();
    gArmMidZero = arm_make_zero_default();
    gArmRightZero = arm_make_zero_default();
}

/*
 * 模块总初始化。
 * 这里只准备基础输入、Body/World 输出、fix、样本池和坐标转换层状态。
 */
void Arm200A_Init(void)
{
    memset(&gArmLeftIn, 0, sizeof(gArmLeftIn));
    memset(&gArmMidIn, 0, sizeof(gArmMidIn));
    memset(&gArmRightIn, 0, sizeof(gArmRightIn));

    Arm200A_ZeroInit();

    gArmLeftIn.cd_mm = 3600.0f;
    gArmLeftIn.gg1_mm = 2480.0f;
    gArmMidIn.cd_mm = 3600.0f;
    gArmMidIn.gg1_mm = 2480.0f;
    gArmRightIn.cd_mm = 3600.0f;
    gArmRightIn.gg1_mm = 2480.0f;

    memset(&gArmLeftBodyOut, 0, sizeof(gArmLeftBodyOut));
    memset(&gArmMidBodyOut, 0, sizeof(gArmMidBodyOut));
    memset(&gArmRightBodyOut, 0, sizeof(gArmRightBodyOut));
    memset(&gArmLeftOut, 0, sizeof(gArmLeftOut));
    memset(&gArmMidOut, 0, sizeof(gArmMidOut));
    memset(&gArmRightOut, 0, sizeof(gArmRightOut));
    memset(&gArmLeftH1Info, 0, sizeof(gArmLeftH1Info));
    memset(&gArmMidH1Info, 0, sizeof(gArmMidH1Info));
    memset(&gArmRightH1Info, 0, sizeof(gArmRightH1Info));

    Arm200A_ClearFix();
    Arm200A_ClearAllSample();
    Arm200A_CoordInit();
}

/*
 * 零点基准层入口。
 * 这里只做：
 * 1. 零点采样
 * 2. raw -> 工程量换算
 */
void Arm200A_ZeroTask(void)
{
    unsigned char changed;
    unsigned int left_mask;
    unsigned int mid_mask;
    unsigned int right_mask;

    changed = 0;
    left_mask = gArmZeroMaskLeft;
    mid_mask = gArmZeroMaskMid;
    right_mask = gArmZeroMaskRight;
    gArmZeroMaskLeft = 0;
    gArmZeroMaskMid = 0;
    gArmZeroMaskRight = 0;

    if(left_mask)
    {
        arm_zero_capture_mask(&gArmLeftRaw, &gArmLeftZero, left_mask);
        changed = 1;
    }
    if(mid_mask)
    {
        arm_zero_capture_mask(&gArmMidRaw, &gArmMidZero, mid_mask);
        changed = 1;
    }
    if(right_mask)
    {
        arm_zero_capture_mask(&gArmRightRaw, &gArmRightZero, right_mask);
        changed = 1;
    }

    if(changed)
    {
        arm_mark_save();
    }
}

/*
 * 清空三条臂的加工误差修正量。
 */
void Arm200A_ClearFix(void)
{
    memset(&gArmLeftFix, 0, sizeof(gArmLeftFix));
    memset(&gArmMidFix, 0, sizeof(gArmMidFix));
    memset(&gArmRightFix, 0, sizeof(gArmRightFix));
}

/*
 * 清空左臂加工误差样本池。
 */
void Arm200A_ClearLeftSample(void)
{
    memset(gArmLeftSample, 0, sizeof(gArmLeftSample));
    arm_fit_clear(&gArmLeftFit);
}

/*
 * 清空中臂加工误差样本池。
 */
void Arm200A_ClearMidSample(void)
{
    memset(gArmMidSample, 0, sizeof(gArmMidSample));
    arm_fit_clear(&gArmMidFit);
}

/*
 * 清空右臂加工误差样本池。
 */
void Arm200A_ClearRightSample(void)
{
    memset(gArmRightSample, 0, sizeof(gArmRightSample));
    arm_fit_clear(&gArmRightFit);
}

/*
 * 清空三条臂全部加工误差样本池。
 */
void Arm200A_ClearAllSample(void)
{
    Arm200A_ClearLeftSample();
    Arm200A_ClearMidSample();
    Arm200A_ClearRightSample();
}

/*
 * 兼容保留函数。
 * 当前零位误差拟合层已退出主框架，这里只清内部兼容缓冲。
 */
void Arm200A_ClearAllZeroSample(void)
{
    memset(gArmLeftZeroSample, 0, sizeof(gArmLeftZeroSample));
    memset(gArmMidZeroSample, 0, sizeof(gArmMidZeroSample));
    memset(gArmRightZeroSample, 0, sizeof(gArmRightZeroSample));
    arm_zero_fit_clear(&gArmLeftZeroFit);
    arm_zero_fit_clear(&gArmMidZeroFit);
    arm_zero_fit_clear(&gArmRightZeroFit);
}

/*
 * 兼容保留函数。
 * 当前零位误差拟合层已不在主流程中，这里保留内部空壳，不作为正式交互入口使用。
 */
unsigned char Arm200A_ZeroAddSample(unsigned char arm_id)
{
    ARM200A_ZERO_SAMPLE *sample;
    ARM200A_ZERO_CFG *zero;
    ARM200A_RAW_INPUT *raw;
    ARM200A_ZERO_FIT_INFO *fit;
    int index;

    sample = arm_get_zero_sample_buf(arm_id);
    zero = arm_get_zero_cfg_buf(arm_id);
    raw = arm_get_raw_buf(arm_id);
    fit = arm_get_zero_fit_buf(arm_id);

    index = arm_find_empty_zero_sample_index(sample);
    if(index < 0)
    {
        index = arm_find_worst_zero_sample_index(arm_id, sample, zero);
    }

    sample[index].use = 1;
    sample[index].root_raw = raw->root_raw;
    sample[index].base_raw = raw->base_raw;
    sample[index].boom_raw = raw->boom_raw;
    sample[index].cd_raw = raw->cd_raw;
    sample[index].d1_raw = raw->d1_raw;
    sample[index].e1_raw = raw->e1_raw;
    sample[index].f_raw = raw->f_raw;
    sample[index].gg1_raw = raw->gg1_raw;
    sample[index].body_roll_raw = raw->body_roll_raw;
    sample[index].body_pitch_raw = raw->body_pitch_raw;

    arm_update_zero_sample_one(arm_id, sample, zero, fit);
    arm_mark_save();
    return (unsigned char)index;
}

/*
 * 左臂计算入口。
 *
 * 固定顺序：
 * 1. 先算左臂 Body 点位
 * 2. 再把 Body 点位转换成 World/大地坐标
 * 3. 最后基于大地系 h / h1 更新 H1/H 向量与夹角
 *
 * 这样得到的 gArmLeftH1Info 是最终可直接用于显示或记录的“大地坐标系结果”。
 */
void Arm200A_CalcLeft(void)
{
    arm_calc_one(&gArmLeftBase, &gArmLeftFix, &gArmLeftIn, &gArmLeftBodyOut);
    arm_body_to_world_result(&gArmLeftBodyOut, &gArmLeftIn, &gArmLeftOut);
    arm_update_h1_info(&gArmLeftOut, &gArmLeftH1Info);
}

/*
 * 中臂计算入口。
 * 流程与左臂一致：Body -> World -> H1/H 大地系派生量。
 */
void Arm200A_CalcMid(void)
{
    arm_calc_one(&gArmMidBase, &gArmMidFix, &gArmMidIn, &gArmMidBodyOut);
    arm_body_to_world_result(&gArmMidBodyOut, &gArmMidIn, &gArmMidOut);
    arm_update_h1_info(&gArmMidOut, &gArmMidH1Info);
}

/*
 * 右臂计算入口。
 * 流程与左臂一致：Body -> World -> H1/H 大地系派生量。
 */
void Arm200A_CalcRight(void)
{
    arm_calc_one(&gArmRightBase, &gArmRightFix, &gArmRightIn, &gArmRightBodyOut);
    arm_body_to_world_result(&gArmRightBodyOut, &gArmRightIn, &gArmRightOut);
    arm_update_h1_info(&gArmRightOut, &gArmRightH1Info);
}

/*
 * 刷新当前加工误差样本池的统计结果。
 * 这里不会新增样本，只回代当前样本池，更新 count / RMS / Max / ok。
 */
void Arm200A_UpdateSampleNow(void)
{
    arm_update_sample_one(ARM200A_LEFT, &gArmLeftFix, gArmLeftSample, &gArmLeftFit);
    arm_update_sample_one(ARM200A_MID, &gArmMidFix, gArmMidSample, &gArmMidFit);
    arm_update_sample_one(ARM200A_RIGHT, &gArmRightFix, gArmRightSample, &gArmRightFit);
}

unsigned char Arm200A_GetSampleCount(unsigned char arm_id)
{
    ARM200A_SAMPLE *sample;
    unsigned char count;
    int i;

    sample = arm_get_sample_buf(arm_id);
    count = 0;
    for(i = 0; i < ARM200A_SAMPLE_MAX; i++)
    {
        if(sample[i].use)
        {
            count++;
        }
    }
    return count;
}

/*
 * 采样入口。
 * HMI 先写 Hx/Hy/Hz，再把 gArmSampleReq* 置 1。
 * 控制器自动抓当前 gArm*In，形成一条加工误差样本。
 */
unsigned char Arm200A_AddSampleByCurrentInput(unsigned char arm_id, float hx, float hy, float hz)
{
    ARM200A_SAMPLE *sample;
    ARM200A_INPUT *in;
    ARM200A_FIX *fix;
    ARM200A_FIT_INFO *fit;
    int index;

    sample = arm_get_sample_buf(arm_id);
    in = arm_get_input_buf(arm_id);
    fix = arm_get_fix_buf(arm_id);
    fit = arm_get_fit_buf(arm_id);

    arm_update_sample_one(arm_id, fix, sample, fit);
    index = arm_find_empty_sample_index(sample);
    if(index < 0)
    {
        index = arm_find_worst_sample_index(arm_id, sample, fix);
    }

    sample[index].use = 1;
    sample[index].in = *in;
    sample[index].h_ref = arm_p(hx, hy, hz);

    arm_update_sample_one(arm_id, fix, sample, fit);
    arm_mark_save();
    return (unsigned char)index;
}

/*
 * HMI 拟合请求辅助入口。
 * 本质还是把对应 gArmFitReq* 置 1。
 */
void Arm200A_RequestFit(unsigned char arm_id)
{
    if(arm_id == ARM200A_LEFT)
    {
        gArmFitReqLeft = 1;
    }
    else if(arm_id == ARM200A_MID)
    {
        gArmFitReqMid = 1;
    }
    else if(arm_id == ARM200A_RIGHT)
    {
        gArmFitReqRight = 1;
    }
}

/*
 * 兼容保留函数。
 * 当前零位误差拟合层已退出主框架，不建议再从 HMI 使用。
 */
void Arm200A_ZeroFitLeft(void)
{
    arm_zero_fit_one(ARM200A_LEFT, gArmLeftZeroSample, &gArmLeftZero, &gArmLeftZeroFit);
    arm_refresh_all_now();
    arm_mark_save();
}

/*
 * 兼容保留函数。
 * 当前零位误差拟合层已退出主框架，不建议再从 HMI 使用。
 */
void Arm200A_ZeroFitMid(void)
{
    arm_zero_fit_one(ARM200A_MID, gArmMidZeroSample, &gArmMidZero, &gArmMidZeroFit);
    arm_refresh_all_now();
    arm_mark_save();
}

/*
 * 兼容保留函数。
 * 当前零位误差拟合层已退出主框架，不建议再从 HMI 使用。
 */
void Arm200A_ZeroFitRight(void)
{
    arm_zero_fit_one(ARM200A_RIGHT, gArmRightZeroSample, &gArmRightZero, &gArmRightZeroFit);
    arm_refresh_all_now();
    arm_mark_save();
}

/*
 * 左臂加工误差拟合入口。
 */
void Arm200A_FitLeft(void)
{
    arm_fit_one(ARM200A_LEFT, gArmLeftSample, &gArmLeftFix, &gArmLeftFit);
    arm_refresh_all_now();
    arm_mark_save();
}

/*
 * 中臂加工误差拟合入口。
 */
void Arm200A_FitMid(void)
{
    arm_fit_one(ARM200A_MID, gArmMidSample, &gArmMidFix, &gArmMidFit);
    arm_refresh_all_now();
    arm_mark_save();
}

/*
 * 右臂加工误差拟合入口。
 */
void Arm200A_FitRight(void)
{
    arm_fit_one(ARM200A_RIGHT, gArmRightSample, &gArmRightFix, &gArmRightFit);
    arm_refresh_all_now();
    arm_mark_save();
}

/*
 * 当前主流程。
 * 固定顺序：
 * 1. 零点基准处理
 * 2. 坐标转换层标定请求
 * 3. 正解得到 Body
 * 4. 坐标转换得到 World
 * 5. 加工样本采集
 * 6. 加工误差拟合
 */
void Arm200A_Task10ms(void)
{
    Arm200A_ZeroTask();
    arm_convert_all_input();

    if(gArmCoordReq)
    {
        Arm200A_CoordCalib();
    }

    Arm200A_CalcLeft();
    Arm200A_CalcMid();
    Arm200A_CalcRight();

    if(gArmSampleReqLeft)
    {
        Arm200A_AddSampleByCurrentInput(ARM200A_LEFT, gArmLeftSampleHx, gArmLeftSampleHy, gArmLeftSampleHz);
        gArmSampleReqLeft = 0;
    }
    if(gArmSampleReqMid)
    {
        Arm200A_AddSampleByCurrentInput(ARM200A_MID, gArmMidSampleHx, gArmMidSampleHy, gArmMidSampleHz);
        gArmSampleReqMid = 0;
    }
    if(gArmSampleReqRight)
    {
        Arm200A_AddSampleByCurrentInput(ARM200A_RIGHT, gArmRightSampleHx, gArmRightSampleHy, gArmRightSampleHz);
        gArmSampleReqRight = 0;
    }

    if(gArmFitReqLeft)
    {
        Arm200A_FitLeft();
         gArmFitReqLeft = 0; // hudh
    }
    if(gArmFitReqMid)
    {
        Arm200A_FitMid();
         gArmFitReqMid = 0;
    }
    if(gArmFitReqRight)
    {
        Arm200A_FitRight();
         gArmFitReqRight = 0;
    }
}
