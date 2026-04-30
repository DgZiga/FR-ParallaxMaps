
#include "agb_debug/debug.c"
#define REG_ID_BG3HOFS 0x1C
#define REG_ID_BG3VOFS 0x1E
struct fieldCameraStruct{
    s16 xPixelOffset;
    s16 yPixelOffset;
};

extern void lcd_io_set(u8 reg_id, s16 value);
extern u16 lcd_io_get(u8 reg_id);
u16 var_load(u16 variable);

extern struct fieldCameraStruct party_opponent;
extern s16 sHorizontalCameraPan;
extern s16 sVerticalCameraPan;

void doPrint(s32 a, s32 b, s32 c){
    dprintf("%x, %x, %x\n", a, b, c);
}

void main(u8 r4, u8 r5){
    u16 bg3_scale = var_load(0x4000);

    u8 bg3_hofs = r5;
    u8 bg3_vofs = r4;

    s32 expandedH = (s32)bg3_hofs;
    s32 expandedV = (s32)bg3_vofs;

    if (bg3_scale == 1 || bg3_scale == 2) {
        s16 r5New = party_opponent.xPixelOffset + sHorizontalCameraPan;
        s16 r4New = sVerticalCameraPan + party_opponent.yPixelOffset + 8;
        s8 halvedH = r5New / 2;
        s8 halvedV = r4New / 2;
        bg3_hofs = halvedH;
        bg3_vofs = halvedV;
    }
    else if (bg3_scale == 3) {
        bg3_hofs = 0;
        bg3_vofs = 0;
    }
    lcd_io_set(REG_ID_BG3HOFS, bg3_hofs);
    lcd_io_set(REG_ID_BG3VOFS, bg3_vofs);
}