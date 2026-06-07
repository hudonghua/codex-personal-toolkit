#ifndef __ARM200A_KINE_H__
#define __ARM200A_KINE_H__

/*
 * Arm200A 当前框架
 *
 * 共用基础模块:
 * 1. 固定几何定义
 * 2. 外设换算
 * 3. 正解
 *
 * 误差修正层:
 * 1. 坐标转换层
 * 2. 零位基准层
 * 3. 加工误差层
 * 4. 视觉修正层
 *
 * 当前原则:
 * - 外设换算只做 raw -> 工程量
 * - 正解先得到 Body 坐标
 * - 坐标转换层再把 Body 坐标转成 World 坐标
 * - 坐标转换层当前按 Q1 Q2 Q3 求解，Q4 校验
 * - 零位误差拟合层已取消，只保留最基本零点标定动作
 * - 为压缩 RAM / EEPROM，统一使用 float
 */

#define ARM200A_LEFT   0
#define ARM200A_MID    1
#define ARM200A_RIGHT  2

#define ARM200A_SAMPLE_MAX      30
#define ARM200A_SAMPLE_MIN_FIT  20
#define ARM200A_FIX_NUM         22

typedef struct
{
    float x;
    float y;
    float z;
} ARM200A_POINT3;

typedef struct
{
    float root_deg;
    float base_deg;
    float boom_deg;
    float cd_mm;
    float d1_deg;
    float e1_deg;
    float f_deg;
    float gg1_mm;
    float body_roll_deg;
    float body_pitch_deg;
} ARM200A_INPUT;

typedef struct
{
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
    float hole_depth_raw;
} ARM200A_RAW_INPUT;

typedef struct
{
    float root_zero_raw;
    float base_zero_raw;
    float boom_zero_raw;
    float cd_zero_raw;
    float d1_zero_raw;
    float e1_zero_raw;
    float f_zero_raw;
    float gg1_zero_raw;
    float body_roll_zero_raw;
    float body_pitch_zero_raw;
    float hole_depth_zero_raw;

    float root_scale;
    float base_scale;
    float boom_scale;
    float cd_scale;
    float d1_scale;
    float e1_scale;
    float f_scale;
    float gg1_scale;
    float body_roll_scale;
    float body_pitch_scale;

    float root_sign;
    float base_sign;
    float boom_sign;
    float cd_sign;
    float d1_sign;
    float e1_sign;
    float f_sign;
    float gg1_sign;
    float body_roll_sign;
    float body_pitch_sign;

    float cd_zero_mm;
    float gg1_zero_mm;
} ARM200A_ZERO_CFG;

/*
 * 一条臂在某一坐标系下的完整点位结果。
 *
 * 这里的点位顺序对应当前正解链：
 * O0 -> O1 -> O -> B -> A -> C -> D -> D1 -> E -> E1 -> F -> G -> G1 -> H
 *
 * 补充说明：
 * 1. 这份结构既用于 Body 坐标，也用于 World/大地坐标。
 * 2. h1 是本轮新增的刚性附加点。
 * 3. h1 的几何含义不是独立铰接点，而是：
 *    - 以 H 点局部坐标系为基准
 *    - 局部刚性向量 H1->H = (300, 0, 0)
 *    - 随当前末端姿态一起运动
 * 4. 在 Body 结果里，h1 先按局部姿态算出；
 *    在 World 结果里，h1 再和其它点一起经过双轴补偿 + Body->World 变换。
 */
typedef struct
{
    ARM200A_POINT3 o0;
    ARM200A_POINT3 o1;
    ARM200A_POINT3 o;
    ARM200A_POINT3 b;
    ARM200A_POINT3 a;
    ARM200A_POINT3 c;
    ARM200A_POINT3 d;
    ARM200A_POINT3 d1;
    ARM200A_POINT3 e;
    ARM200A_POINT3 e1;
    ARM200A_POINT3 f;
    ARM200A_POINT3 g;
    ARM200A_POINT3 g1;
    ARM200A_POINT3 h1;
    ARM200A_POINT3 h;
    unsigned char has_o0;
    unsigned char ok;
} ARM200A_RESULT;

/*
 * H1/H 刚性向量在大地坐标系下的派生结果。
 *
 * 当前项目对 H1/H 的理解是：
 * 1. H1 与 H 为刚性连接。
 * 2. H1->H 在局部坐标系中固定为 (300,0,0)。
 * 3. 先随末端姿态旋转，再叠加车体双轴倾斜，再进入大地坐标系。
 *
 * 因此这里保存的是“最终世界系/大地系结果”，不是局部结果。
 *
 * 字段说明：
 * - h1：
 *   H1 点在大地坐标系下的位置。
 * - h1_to_h_world：
 *   大地坐标系下的 H1->H 向量。
 * - angle_xy_plane_deg / angle_xz_plane_deg / angle_yz_plane_deg：
 *   H1->H 直线与大地固定 XY/XZ/YZ 三个平面的夹角。
 *   这里是严格线面角定义，范围 0~90 度。
 * - angle_x_axis_deg / angle_y_axis_deg / angle_z_axis_deg：
 *   H1->H 向量与大地固定 X/Y/Z 三轴的夹角。
 * - ok：
 *   结果有效标志。只有当本轮 H / H1 都已正常算出时才置 1。
 */
typedef struct
{
    ARM200A_POINT3 h1;
    ARM200A_POINT3 h1_to_h_world;
    float angle_xy_plane_deg;
    float angle_xz_plane_deg;
    float angle_yz_plane_deg;
    float angle_x_axis_deg;
    float angle_y_axis_deg;
    float angle_z_axis_deg;
    unsigned char ok;
} ARM200A_H1_INFO;

typedef struct
{
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
} ARM200A_FIX;

typedef struct
{
    unsigned char use;
    ARM200A_INPUT in;
    ARM200A_POINT3 h_ref;
} ARM200A_SAMPLE;

typedef struct
{
    unsigned char ok;
    unsigned char count;
    float rms_mm;
    float max_mm;
    float last_step_mm;
} ARM200A_FIT_INFO;

typedef struct
{
    float r[3][3];
    ARM200A_POINT3 t;
    float rms_mm;
    float max_mm;
    float q4_err_mm;
    unsigned char valid;
} ARM200A_COORD_TRANS;

extern ARM200A_RAW_INPUT gArmLeftRaw;
extern ARM200A_RAW_INPUT gArmMidRaw;
extern ARM200A_RAW_INPUT gArmRightRaw;

extern ARM200A_ZERO_CFG gArmLeftZero;
extern ARM200A_ZERO_CFG gArmMidZero;
extern ARM200A_ZERO_CFG gArmRightZero;

extern ARM200A_INPUT gArmLeftIn;
extern ARM200A_INPUT gArmMidIn;
extern ARM200A_INPUT gArmRightIn;

extern ARM200A_RESULT gArmLeftBodyOut;
extern ARM200A_RESULT gArmMidBodyOut;
extern ARM200A_RESULT gArmRightBodyOut;

extern ARM200A_RESULT gArmLeftOut;
extern ARM200A_RESULT gArmMidOut;
extern ARM200A_RESULT gArmRightOut;

/*
 * 三条臂对应的 H1/H 大地坐标系结果。
 *
 * 这些量的口径已经包含：
 * 1. 机械正解
 * 2. H1 局部刚性向量
 * 3. 双轴倾斜补偿
 * 4. Body -> World/大地坐标系转换
 *
 * 后续若 HMI 需要直接显示：
 * - H1 点坐标
 * - H1->H 向量
 * - 与三平面夹角
 * - 与三轴夹角
 * 可以直接从这里取值。
 */
extern ARM200A_H1_INFO gArmLeftH1Info;
extern ARM200A_H1_INFO gArmMidH1Info;
extern ARM200A_H1_INFO gArmRightH1Info;

extern ARM200A_FIX gArmLeftFix;
extern ARM200A_FIX gArmMidFix;
extern ARM200A_FIX gArmRightFix;

extern ARM200A_SAMPLE gArmLeftSample[ARM200A_SAMPLE_MAX];
extern ARM200A_SAMPLE gArmMidSample[ARM200A_SAMPLE_MAX];
extern ARM200A_SAMPLE gArmRightSample[ARM200A_SAMPLE_MAX];

extern ARM200A_FIT_INFO gArmLeftFit;
extern ARM200A_FIT_INFO gArmMidFit;
extern ARM200A_FIT_INFO gArmRightFit;

extern ARM200A_COORD_TRANS gArmCoordTrans;

extern float gArmQ1WorldX;
extern float gArmQ1WorldY;
extern float gArmQ1WorldZ;
extern float gArmQ2WorldX;
extern float gArmQ2WorldY;
extern float gArmQ2WorldZ;
extern float gArmQ3WorldX;
extern float gArmQ3WorldY;
extern float gArmQ3WorldZ;
extern float gArmQ4WorldX;
extern float gArmQ4WorldY;
extern float gArmQ4WorldZ;

#define ARM200A_ZERO_MASK_ROOT        0x0001U
#define ARM200A_ZERO_MASK_BASE        0x0002U
#define ARM200A_ZERO_MASK_BOOM        0x0004U
#define ARM200A_ZERO_MASK_D1          0x0008U
#define ARM200A_ZERO_MASK_E1          0x0010U
#define ARM200A_ZERO_MASK_F           0x0020U
#define ARM200A_ZERO_MASK_CD          0x0040U
#define ARM200A_ZERO_MASK_GG1         0x0080U
#define ARM200A_ZERO_MASK_BODY_ROLL   0x0100U
#define ARM200A_ZERO_MASK_BODY_PITCH  0x0200U
#define ARM200A_ZERO_MASK_HOLE_DEPTH  0x0400U

extern unsigned char gArmUseZeroComp;
extern unsigned int gArmZeroMaskLeft;
extern unsigned int gArmZeroMaskMid;
extern unsigned int gArmZeroMaskRight;
extern unsigned char gArmCoordReq;
extern unsigned char gArmSampleReqLeft;
extern unsigned char gArmSampleReqMid;
extern unsigned char gArmSampleReqRight;
extern unsigned char gArmFitReqLeft;
extern unsigned char gArmFitReqMid;
extern unsigned char gArmFitReqRight;

extern float gArmLeftSampleHx;
extern float gArmLeftSampleHy;
extern float gArmLeftSampleHz;
extern float gArmMidSampleHx;
extern float gArmMidSampleHy;
extern float gArmMidSampleHz;
extern float gArmRightSampleHx;
extern float gArmRightSampleHy;
extern float gArmRightSampleHz;

void Arm200A_Init(void);
void Arm200A_ZeroInit(void);
void Arm200A_ZeroTask(void);
void Arm200A_CoordInit(void);
void Arm200A_CoordCalib(void);

void Arm200A_ClearFix(void);
void Arm200A_ClearLeftSample(void);
void Arm200A_ClearMidSample(void);
void Arm200A_ClearRightSample(void);
void Arm200A_ClearAllSample(void);

void Arm200A_CalcLeft(void);
void Arm200A_CalcMid(void);
void Arm200A_CalcRight(void);
void Arm200A_Task10ms(void);
void Arm200A_UpdateSampleNow(void);

void Arm200A_FitLeft(void);
void Arm200A_FitMid(void);
void Arm200A_FitRight(void);

void Arm200A_FixToArray(ARM200A_FIX *fix, float out_scalar[ARM200A_FIX_NUM], unsigned char has_o0);
void Arm200A_ArrayToFix(float in_scalar[ARM200A_FIX_NUM], ARM200A_FIX *fix, unsigned char has_o0);

unsigned char Arm200A_GetSampleCount(unsigned char arm_id);
unsigned char Arm200A_AddSampleByCurrentInput(unsigned char arm_id, float hx, float hy, float hz);
void Arm200A_RequestFit(unsigned char arm_id);

#endif
