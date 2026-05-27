
#include <pokeagb/pokeagb.h>
#include "include/agb_debug/debug.c"
#include "built_graphics/3_test.h"

#define CUSTOM_VAR 0x4000
#define REG_ID_BG3HOFS 0x1C
#define REG_ID_BG3VOFS 0x1E
#define CUSTOM_CHAR_BASE 3
struct fieldCameraStruct{
    s16 xPixelOffset;
    s16 yPixelOffset;
};

extern void InitBgFromTemplate(const struct BgConfig* config);
extern void gpu_bg_config_set_field(u8 r0, u8 r1, u8 r2);
u16 var_load(u16 variable);

struct fieldCameraStruct *opponentParty = (struct fieldCameraStruct *)0x0202402c;
extern s16 sHorizontalCameraPan;
extern s16 sVerticalCameraPan;

void main(u8 r4, u8 r5){
    u16 bg3_scale = var_load(CUSTOM_VAR);

    u8 bg3_hofs = r5;
    u8 bg3_vofs = r4;

    if (bg3_scale == 1 || bg3_scale == 2) {
        s16 r5New = opponentParty->xPixelOffset + sHorizontalCameraPan;
        s16 r4New = sVerticalCameraPan + opponentParty->yPixelOffset + 8;
        s8 halvedH = r5New / (bg3_scale + 1);
        s8 halvedV = r4New / (bg3_scale + 1);
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

struct BgConfig bg_config1 = {.padding=0, .b_padding=0, .priority=3, .palette=0, .size=0, .map_base=30, .character_base=CUSTOM_CHAR_BASE, .bgid=3, };
void **overworld_bg3_tilemap = (void **)0x0300501C;

void task_apply_pal(u8 task_id);


void loadImg() {
    u8 bgId = 3; 
    InitBgFromTemplate(&bg_config1);
    gpu_bg_config_set_field(3, 5, 1);
    //dprintf("%x", *((u16 *)0x05000062));
    void *buffer = malloc_and_clear(0x840); // tiles are 8x8, so 32 tiles cover 256 screen. 33 tiles are required for img 256x257. 33 * 32 * 2 = 2112 = 0x840
    task_add((TaskCallback)(task_apply_pal), 0); 
    LZ77UnCompWram((void *)(__testMap), (void *)buffer);
    lz77UnCompVram((void *)(__testTiles), (void *)(ADDR_VRAM + (CUSTOM_CHAR_BASE*0x4000) + 50*0x20));
    bgid_set_tilemap(bgId, buffer);
    free(*overworld_bg3_tilemap);
    *overworld_bg3_tilemap = buffer;
    bgid_mark_for_sync(bgId);
    gpu_sync_bg_show(bgId);
}

//delay to task, otherwise we would write to palettes and then GF code would overwrite our palette
void task_apply_pal(u8 task_id){
    if (!pal_fade_control.active) {
        gpu_pal_apply((void *)(__testPal), 3 * 16, 32);
        task_del(task_id);
    }
}


enum {
    METATILE_LAYER_TYPE_NORMAL,  // Metatile uses middle and top bg layers
    METATILE_LAYER_TYPE_COVERED, // Metatile uses bottom and middle bg layers
    METATILE_LAYER_TYPE_SPLIT,   // Metatile uses bottom and top bg layers
};

extern u16 *gBGTilemapBuffers1;
extern u16 *gBGTilemapBuffers2;
extern u16 *gBGTilemapBuffers3;

//replacement of original function at 0805A9B4, borrowed from pokefirered
void DrawMetatile(s32 metatileLayerType, const u16 *tiles, u16 offset) {
    u16 bg3_scale = var_load(CUSTOM_VAR);
    switch (metatileLayerType)
    {
    case METATILE_LAYER_TYPE_SPLIT:
        if (bg3_scale == 0){
            // Draw metatile's bottom layer to the bottom background layer.
            gBGTilemapBuffers3[offset] = tiles[0];
            gBGTilemapBuffers3[offset + 1] = tiles[1];
            gBGTilemapBuffers3[offset + 0x20] = tiles[2];
            gBGTilemapBuffers3[offset + 0x21] = tiles[3];
        }

        // Draw transparent tiles to the middle background layer.
        gBGTilemapBuffers1[offset] = 0;
        gBGTilemapBuffers1[offset + 1] = 0;
        gBGTilemapBuffers1[offset + 0x20] = 0;
        gBGTilemapBuffers1[offset + 0x21] = 0;

        // Draw metatile's top layer to the top background layer.
        gBGTilemapBuffers2[offset] = tiles[4];
        gBGTilemapBuffers2[offset + 1] = tiles[5];
        gBGTilemapBuffers2[offset + 0x20] = tiles[6];
        gBGTilemapBuffers2[offset + 0x21] = tiles[7];
        break;
    case METATILE_LAYER_TYPE_COVERED:
        if (bg3_scale == 0){
            // Draw metatile's bottom layer to the bottom background layer.
            gBGTilemapBuffers3[offset] = tiles[0];
            gBGTilemapBuffers3[offset + 1] = tiles[1];
            gBGTilemapBuffers3[offset + 0x20] = tiles[2];
            gBGTilemapBuffers3[offset + 0x21] = tiles[3];
        }

        // Draw metatile's top layer to the middle background layer.
        gBGTilemapBuffers1[offset] = tiles[4];
        gBGTilemapBuffers1[offset + 1] = tiles[5];
        gBGTilemapBuffers1[offset + 0x20] = tiles[6];
        gBGTilemapBuffers1[offset + 0x21] = tiles[7];

        // Draw transparent tiles to the top background layer.
        gBGTilemapBuffers2[offset] = 0;
        gBGTilemapBuffers2[offset + 1] = 0;
        gBGTilemapBuffers2[offset + 0x20] = 0;
        gBGTilemapBuffers2[offset + 0x21] = 0;
        break;
    case METATILE_LAYER_TYPE_NORMAL:
        if (bg3_scale == 0){
            // Draw garbage to the bottom background layer.
            gBGTilemapBuffers3[offset] = 0x3014;
            gBGTilemapBuffers3[offset + 1] = 0x3014;
            gBGTilemapBuffers3[offset + 0x20] = 0x3014;
            gBGTilemapBuffers3[offset + 0x21] = 0x3014;
        }

        // Draw metatile's bottom layer to the middle background layer.
        gBGTilemapBuffers1[offset] = tiles[0];
        gBGTilemapBuffers1[offset + 1] = tiles[1];
        gBGTilemapBuffers1[offset + 0x20] = tiles[2];
        gBGTilemapBuffers1[offset + 0x21] = tiles[3];

        // Draw metatile's top layer to the top background layer, which covers object event sprites.
        gBGTilemapBuffers2[offset] = tiles[4];
        gBGTilemapBuffers2[offset + 1] = tiles[5];
        gBGTilemapBuffers2[offset + 0x20] = tiles[6];
        gBGTilemapBuffers2[offset + 0x21] = tiles[7];
        break;
    }
    bgid_mark_for_sync(1);
    bgid_mark_for_sync(2);
    bgid_mark_for_sync(3);
}