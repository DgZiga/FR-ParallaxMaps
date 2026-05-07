
#include <pokeagb/pokeagb.h>
#include "include/agb_debug/debug.c"
#include "built_graphics/3_test.h"
#define REG_ID_BG3HOFS 0x1C
#define REG_ID_BG3VOFS 0x1E
struct fieldCameraStruct{
    s16 xPixelOffset;
    s16 yPixelOffset;
};

extern void gpu_bg_config_set_field(u8 r0, u8 r1, u8 r2);
extern u16 lcd_io_get(u8 reg_id);
u16 var_load(u16 variable);

struct fieldCameraStruct *opponentParty = (struct fieldCameraStruct *)0x0202402c;
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
        s16 r5New = opponentParty->xPixelOffset + sHorizontalCameraPan;
        s16 r4New = sVerticalCameraPan + opponentParty->yPixelOffset + 8;
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

/*
//char_base is multiplied by 0x4000 and added to 0x06000000. 
//map_base is multiplied by 0x800 and added to 0x06000000.
//size: 0 is 256x256, 1 is 512x256
//priority: 0-3, 0 is higher
//palette: 0 (16) or 1(256)

F8 01 00 00 D1 11 00 00  C2 21 00 00 E3 31 00 00


	u16 bgid : 2;
	u16 character_base : 2;
	u16 map_base : 5;
	u16 size : 2;
	u16 palette : 1;
	u16 priority : 2;
	u16 b_padding : 2; // bit field padding
	u16 padding;


01F8
00 01 11111 000
0000000111111000

00 00 0 00 11111 10 00
{.padding=0, .b_padding=0, .priority=0, .palette=0, .size=0, .map_base=31, .character_base=2, .bgid=0, };


11D1
0001000111010001

00 01 0 00 11101 00 01

{.padding=0, .b_padding=0, .priority=1, .palette=0, .size=0, .map_base=29, .character_base=0, .bgid=1, };

QUINDI: prendi l'u16, converti in bit, e da lì hai la struct in ordine inverso (quindi primo campo è b_padding e ultimo è bgid)
C2 21 = 21C2 = 0010000111000010
00 10 0 00 11100 00 10
{.padding=0, .b_padding=0, .priority=2, .palette=0, .size=0, .map_base=28, .character_base=0, .bgid=2, };

*/

struct BgConfig bg_config1 = {.padding=0, .b_padding=0, .priority=3, .palette=0, .size=0, .map_base=30/*30 = 0x18000*/, .character_base=3/*0x10000*/, .bgid=3, };
struct BgConfig bg_configs[4] = {
    {.padding=0, .b_padding=0, .priority=0, .palette=0, .size=0, .map_base=31, .character_base=2, .bgid=0, },   
    {.padding=0, .b_padding=0, .priority=1, .palette=0, .size=0, .map_base=29, .character_base=0, .bgid=1, },   
    {.padding=0, .b_padding=0, .priority=2, .palette=0, .size=0, .map_base=28, .character_base=0, .bgid=2, },   
    {.padding=0, .b_padding=0, .priority=3, .palette=0, .size=0, .map_base=30, .character_base=3, .bgid=3, }}
;

void loadImg() {
    u8 bgId = 3; 
    bg_vram_setup(0, bg_configs, 4);
    gpu_bg_config_set_field(1, 5, 1);
    gpu_bg_config_set_field(2, 5, 1);
    gpu_bg_config_set_field(3, 5, 1);
    void *buffer = malloc(0x1000);
    gpu_pal_apply_compressed((void *)(__testPal), bgId * 16, 32);
    LZ77UnCompWram((void *)(__testMap), (void *)buffer);
    //lz77UnCompVram((void *)(__testTiles), (void *)(ADDR_VRAM));
    lz77UnCompVram((void *)(__testTiles), (void *)(ADDR_VRAM + (bgId*0x4000)));
    bgid_set_tilemap(bgId, buffer);
    bgid_mark_for_sync(bgId);
    gpu_sync_bg_show(bgId);
}