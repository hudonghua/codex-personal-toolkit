#include "Arm200A_Store.h"
#include "Arm200A_Kine.h"
#include "App_Bus.h"
#include "config.h"

#define ARM200A_STORE_ADDR     0x2000UL
#define ARM200A_STORE_MAGIC    0x32414B41UL
#define ARM200A_STORE_VERSION  0x00010006UL
#define ARM200A_STORE_VERSION_10005  0x00010005UL
#define ARM200A_STORE_CHUNK    32U

typedef struct
{
    uint32 magic;
    uint32 version;
    unsigned char use_zero_comp;
    unsigned char coord_valid;
    unsigned char reserve1;
    unsigned char reserve2;
    ARM200A_ZERO_CFG left_zero;
    ARM200A_ZERO_CFG mid_zero;
    ARM200A_ZERO_CFG right_zero;
    float q1_world[3];
    float q2_world[3];
    float q3_world[3];
    float q4_world[3];
    ARM200A_COORD_TRANS coord_trans;
    float left_fix[ARM200A_FIX_NUM];
    float mid_fix[ARM200A_FIX_NUM];
    float right_fix[ARM200A_FIX_NUM];
    ARM200A_SAMPLE left_sample[ARM200A_SAMPLE_MAX];
    ARM200A_SAMPLE mid_sample[ARM200A_SAMPLE_MAX];
    ARM200A_SAMPLE right_sample[ARM200A_SAMPLE_MAX];
    ARM200A_FIT_INFO left_fit;
    ARM200A_FIT_INFO mid_fit;
    ARM200A_FIT_INFO right_fit;
    uint32 checksum;
} ARM200A_STORE_BLOCK;

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
} ARM200A_ZERO_CFG_V10005;

typedef struct
{
    uint32 magic;
    uint32 version;
    unsigned char use_zero_comp;
    unsigned char coord_valid;
    unsigned char reserve1;
    unsigned char reserve2;
    ARM200A_ZERO_CFG_V10005 left_zero;
    ARM200A_ZERO_CFG_V10005 mid_zero;
    ARM200A_ZERO_CFG_V10005 right_zero;
    float q1_world[3];
    float q2_world[3];
    float q3_world[3];
    float q4_world[3];
    ARM200A_COORD_TRANS coord_trans;
    float left_fix[ARM200A_FIX_NUM];
    float mid_fix[ARM200A_FIX_NUM];
    float right_fix[ARM200A_FIX_NUM];
    ARM200A_SAMPLE left_sample[ARM200A_SAMPLE_MAX];
    ARM200A_SAMPLE mid_sample[ARM200A_SAMPLE_MAX];
    ARM200A_SAMPLE right_sample[ARM200A_SAMPLE_MAX];
    ARM200A_FIT_INFO left_fit;
    ARM200A_FIT_INFO mid_fit;
    ARM200A_FIT_INFO right_fit;
    uint32 checksum;
} ARM200A_STORE_BLOCK_V10005;

/*
 * Keep the large EEPROM image out of the call stack.
 * The original local block was several KB and could overflow the LPC17xx
 * runtime stack during save/load paths.
 */
static ARM200A_STORE_BLOCK gArmStoreBlock;

static uint32 arm_store_checksum(unsigned char *buf, unsigned int len_without_checksum)
{
    uint32 sum;
    unsigned int i;

    sum = 0;
    for(i = 0; i < len_without_checksum; i++)
    {
        sum += buf[i];
    }
    return sum;
}

static unsigned char arm_store_read_bytes(unsigned int addr, unsigned char *buf, unsigned int len)
{
    unsigned int offset;
    unsigned int once_len;
    unsigned char ok;

    offset = 0;
    while(offset < len)
    {
        once_len = len - offset;
        if(once_len > ARM200A_STORE_CHUNK)
        {
            once_len = ARM200A_STORE_CHUNK;
        }

        ok = AT24_Read_Str(addr + offset, buf + offset, (unsigned char)once_len);
        WDT_Feed();
        if(ok == 0)
        {
            return 0;
        }
        offset += once_len;
    }
    return 1;
}

static unsigned char arm_store_write_bytes(unsigned int addr, unsigned char *buf, unsigned int len)
{
    unsigned int offset;
    unsigned int once_len;
    unsigned char ok;

    offset = 0;
    while(offset < len)
    {
        once_len = len - offset;
        if(once_len > ARM200A_STORE_CHUNK)
        {
            once_len = ARM200A_STORE_CHUNK;
        }

        ok = AT24_Write_Page(addr + offset, buf + offset, (unsigned char)once_len);
        WDT_Feed();
        if(ok == 0)
        {
            return 0;
        }
        offset += once_len;
    }
    return 1;
}

static void arm_store_fill_block(ARM200A_STORE_BLOCK *blk)
{
    blk->magic = ARM200A_STORE_MAGIC;
    blk->version = ARM200A_STORE_VERSION;
    blk->use_zero_comp = gArmUseZeroComp;
    blk->coord_valid = gArmCoordTrans.valid;
    blk->reserve1 = 0;
    blk->reserve2 = 0;

    blk->left_zero = gArmLeftZero;
    blk->mid_zero = gArmMidZero;
    blk->right_zero = gArmRightZero;

    blk->q1_world[0] = gArmQ1WorldX; blk->q1_world[1] = gArmQ1WorldY; blk->q1_world[2] = gArmQ1WorldZ;
    blk->q2_world[0] = gArmQ2WorldX; blk->q2_world[1] = gArmQ2WorldY; blk->q2_world[2] = gArmQ2WorldZ;
    blk->q3_world[0] = gArmQ3WorldX; blk->q3_world[1] = gArmQ3WorldY; blk->q3_world[2] = gArmQ3WorldZ;
    blk->q4_world[0] = gArmQ4WorldX; blk->q4_world[1] = gArmQ4WorldY; blk->q4_world[2] = gArmQ4WorldZ;
    blk->coord_trans = gArmCoordTrans;

    Arm200A_FixToArray(&gArmLeftFix, blk->left_fix, 1);
    Arm200A_FixToArray(&gArmMidFix, blk->mid_fix, 0);
    Arm200A_FixToArray(&gArmRightFix, blk->right_fix, 1);

    memcpy(blk->left_sample, gArmLeftSample, sizeof(gArmLeftSample));
    memcpy(blk->mid_sample, gArmMidSample, sizeof(gArmMidSample));
    memcpy(blk->right_sample, gArmRightSample, sizeof(gArmRightSample));

    blk->left_fit = gArmLeftFit;
    blk->mid_fit = gArmMidFit;
    blk->right_fit = gArmRightFit;

    blk->checksum = 0;
    blk->checksum = arm_store_checksum((unsigned char *)blk, sizeof(*blk) - sizeof(blk->checksum));
}

static unsigned char arm_store_block_valid(ARM200A_STORE_BLOCK *blk)
{
    uint32 sum;

    if(blk->magic != ARM200A_STORE_MAGIC)
    {
        return 0;
    }
    if(blk->version != ARM200A_STORE_VERSION)
    {
        return 0;
    }

    sum = arm_store_checksum((unsigned char *)blk, sizeof(*blk) - sizeof(blk->checksum));
    if(sum != blk->checksum)
    {
        return 0;
    }

    return 1;
}


static unsigned char arm_store_block_v10005_valid(ARM200A_STORE_BLOCK_V10005 *blk)
{
    uint32 sum;

    if(blk->magic != ARM200A_STORE_MAGIC)
    {
        return 0;
    }
    if(blk->version != ARM200A_STORE_VERSION_10005)
    {
        return 0;
    }

    sum = arm_store_checksum((unsigned char *)blk, sizeof(*blk) - sizeof(blk->checksum));
    if(sum != blk->checksum)
    {
        return 0;
    }

    return 1;
}

static void arm_store_copy_zero_v10005(ARM200A_ZERO_CFG *dst, ARM200A_ZERO_CFG_V10005 *src)
{
    dst->root_zero_raw = src->root_zero_raw;
    dst->base_zero_raw = src->base_zero_raw;
    dst->boom_zero_raw = src->boom_zero_raw;
    dst->cd_zero_raw = src->cd_zero_raw;
    dst->d1_zero_raw = src->d1_zero_raw;
    dst->e1_zero_raw = src->e1_zero_raw;
    dst->f_zero_raw = src->f_zero_raw;
    dst->gg1_zero_raw = src->gg1_zero_raw;
    dst->body_roll_zero_raw = src->body_roll_zero_raw;
    dst->body_pitch_zero_raw = src->body_pitch_zero_raw;
    dst->hole_depth_zero_raw = 0.0f;

    dst->root_scale = src->root_scale;
    dst->base_scale = src->base_scale;
    dst->boom_scale = src->boom_scale;
    dst->cd_scale = src->cd_scale;
    dst->d1_scale = src->d1_scale;
    dst->e1_scale = src->e1_scale;
    dst->f_scale = src->f_scale;
    dst->gg1_scale = src->gg1_scale;
    dst->body_roll_scale = src->body_roll_scale;
    dst->body_pitch_scale = src->body_pitch_scale;

    dst->root_sign = src->root_sign;
    dst->base_sign = src->base_sign;
    dst->boom_sign = src->boom_sign;
    dst->cd_sign = src->cd_sign;
    dst->d1_sign = src->d1_sign;
    dst->e1_sign = src->e1_sign;
    dst->f_sign = src->f_sign;
    dst->gg1_sign = src->gg1_sign;
    dst->body_roll_sign = src->body_roll_sign;
    dst->body_pitch_sign = src->body_pitch_sign;

    dst->cd_zero_mm = src->cd_zero_mm;
    dst->gg1_zero_mm = src->gg1_zero_mm;
}

static void arm_store_apply_block(ARM200A_STORE_BLOCK *blk)
{
    gArmUseZeroComp = blk->use_zero_comp;

    gArmLeftZero = blk->left_zero;
    gArmMidZero = blk->mid_zero;
    gArmRightZero = blk->right_zero;

    gArmQ1WorldX = blk->q1_world[0]; gArmQ1WorldY = blk->q1_world[1]; gArmQ1WorldZ = blk->q1_world[2];
    gArmQ2WorldX = blk->q2_world[0]; gArmQ2WorldY = blk->q2_world[1]; gArmQ2WorldZ = blk->q2_world[2];
    gArmQ3WorldX = blk->q3_world[0]; gArmQ3WorldY = blk->q3_world[1]; gArmQ3WorldZ = blk->q3_world[2];
    gArmQ4WorldX = blk->q4_world[0]; gArmQ4WorldY = blk->q4_world[1]; gArmQ4WorldZ = blk->q4_world[2];
    gArmCoordTrans = blk->coord_trans;
    gArmCoordTrans.valid = blk->coord_valid;

    Arm200A_ArrayToFix(blk->left_fix, &gArmLeftFix, 1);
    Arm200A_ArrayToFix(blk->mid_fix, &gArmMidFix, 0);
    Arm200A_ArrayToFix(blk->right_fix, &gArmRightFix, 1);

    memcpy(gArmLeftSample, blk->left_sample, sizeof(gArmLeftSample));
    memcpy(gArmMidSample, blk->mid_sample, sizeof(gArmMidSample));
    memcpy(gArmRightSample, blk->right_sample, sizeof(gArmRightSample));

    gArmLeftFit = blk->left_fit;
    gArmMidFit = blk->mid_fit;
    gArmRightFit = blk->right_fit;
}


static void arm_store_apply_block_v10005(ARM200A_STORE_BLOCK_V10005 *blk)
{
    gArmUseZeroComp = blk->use_zero_comp;

    arm_store_copy_zero_v10005(&gArmLeftZero, &blk->left_zero);
    arm_store_copy_zero_v10005(&gArmMidZero, &blk->mid_zero);
    arm_store_copy_zero_v10005(&gArmRightZero, &blk->right_zero);

    gArmQ1WorldX = blk->q1_world[0]; gArmQ1WorldY = blk->q1_world[1]; gArmQ1WorldZ = blk->q1_world[2];
    gArmQ2WorldX = blk->q2_world[0]; gArmQ2WorldY = blk->q2_world[1]; gArmQ2WorldZ = blk->q2_world[2];
    gArmQ3WorldX = blk->q3_world[0]; gArmQ3WorldY = blk->q3_world[1]; gArmQ3WorldZ = blk->q3_world[2];
    gArmQ4WorldX = blk->q4_world[0]; gArmQ4WorldY = blk->q4_world[1]; gArmQ4WorldZ = blk->q4_world[2];
    gArmCoordTrans = blk->coord_trans;
    gArmCoordTrans.valid = blk->coord_valid;

    Arm200A_ArrayToFix(blk->left_fix, &gArmLeftFix, 1);
    Arm200A_ArrayToFix(blk->mid_fix, &gArmMidFix, 0);
    Arm200A_ArrayToFix(blk->right_fix, &gArmRightFix, 1);

    memcpy(gArmLeftSample, blk->left_sample, sizeof(gArmLeftSample));
    memcpy(gArmMidSample, blk->mid_sample, sizeof(gArmMidSample));
    memcpy(gArmRightSample, blk->right_sample, sizeof(gArmRightSample));

    gArmLeftFit = blk->left_fit;
    gArmMidFit = blk->mid_fit;
    gArmRightFit = blk->right_fit;
}

void Arm200A_StoreSave(void)
{
    memset(&gArmStoreBlock, 0, sizeof(gArmStoreBlock));
    arm_store_fill_block(&gArmStoreBlock);
    arm_store_write_bytes(ARM200A_STORE_ADDR, (unsigned char *)&gArmStoreBlock, sizeof(gArmStoreBlock));
}

void Arm200A_StoreLoad(void)
{
    unsigned char ok;
    ARM200A_STORE_BLOCK_V10005 *old_blk;

    memset(&gArmStoreBlock, 0, sizeof(gArmStoreBlock));
    ok = arm_store_read_bytes(ARM200A_STORE_ADDR, (unsigned char *)&gArmStoreBlock, sizeof(gArmStoreBlock));
    if(ok && arm_store_block_valid(&gArmStoreBlock))
    {
        arm_store_apply_block(&gArmStoreBlock);
        return;
    }

    memset(&gArmStoreBlock, 0, sizeof(gArmStoreBlock));
    old_blk = (ARM200A_STORE_BLOCK_V10005 *)&gArmStoreBlock;
    ok = arm_store_read_bytes(ARM200A_STORE_ADDR, (unsigned char *)old_blk, sizeof(ARM200A_STORE_BLOCK_V10005));
    if(ok && arm_store_block_v10005_valid(old_blk))
    {
        arm_store_apply_block_v10005(old_blk);
        Arm200A_StoreSave();
    }
}
