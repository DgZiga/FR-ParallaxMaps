
typedef unsigned char u8;
typedef unsigned short int u16;

#define REG_ID_BG3HOFS 0x1C
#define REG_ID_BG3VOFS 0x1E
extern void lcd_io_set(u8 reg_id, u16 value);
u16 var_load(u16 variable);

void main(u16 r4, u16 r5){
    u16 bg3_scale = var_load(0x4000);
    
    u16 bg3_hofs = r5;
    u16 bg3_vofs = r4;

    if (bg3_scale == 1 || bg3_scale == 2) {
        bg3_hofs = bg3_hofs >> bg3_scale;
        bg3_vofs = bg3_vofs >> bg3_scale;
    }
    else if (bg3_scale == 3) {
        bg3_hofs = 0;
        bg3_vofs = 0;
    }
    lcd_io_set(REG_ID_BG3HOFS, bg3_hofs);
    lcd_io_set(REG_ID_BG3VOFS, bg3_vofs);
}