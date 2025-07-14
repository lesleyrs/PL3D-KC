/*****************************************************************************/
/*
 * FW LE (Lite edition) - Fundamentals of the King's Crook graphics engine.
 * 
 *   by EMMIR 2018-2022
 *   
 *   YouTube: https://www.youtube.com/c/LMP88
 *   
 * This software is released into the public domain.
 */
/*****************************************************************************/

#include "fw.h"
#include "fw_priv.h"

#include <stdio.h>
#include <stdlib.h>

#include <js/glue.h>
#include <js/dom_pk_codes.h>

#ifdef FW_OS_TYPE_WASM
// compiler_rt stub
void __unordtf2() {}

static void def_keyboard_func(int key) { (void) key; }
static void (*keyboard_func)(int key)   = def_keyboard_func;
static void (*keyboardup_func)(int key) = def_keyboard_func;

static VIDINFO FW_curinfo;

static int clock_mode = FW_CLK_MODE_LORES;

void clk_init      (void) {}
void clk_mode(int mode)
{
    switch (mode) {
        case FW_CLK_MODE_LORES:
        case FW_CLK_MODE_HIRES:
            clock_mode = mode;
            break;
        default:
            FW_error("[xclk] invalid clock mode");
            break;
    }
}
utime clk_sample(void) {
    if (clock_mode == FW_CLK_MODE_LORES) {
        return JS_DateNow();
    } else {
        return JS_performanceNow();
    }
}
int  wnd_osm_handle(void) { return 1; }
// TODO maybe remove eventlisteners here
void wnd_term      (void) {}

static int kbd_pk2vk(int code) {
    switch (code) {
        case DOM_PK_ARROW_LEFT:
            return FW_KEY_ARROW_LEFT;
        case DOM_PK_ARROW_UP:
            return FW_KEY_ARROW_UP;
        case DOM_PK_ARROW_RIGHT:
            return FW_KEY_ARROW_RIGHT;
        case DOM_PK_ARROW_DOWN:
            return FW_KEY_ARROW_DOWN;
        case DOM_PK_ENTER:
            return FW_KEY_ENTER;
        case DOM_PK_TAB:
            return FW_KEY_TAB;
        case DOM_PK_ESCAPE:
            return FW_KEY_ESCAPE;
        case DOM_PK_SHIFT_LEFT:
        case DOM_PK_SHIFT_RIGHT:
            return FW_KEY_SHIFT;
        case DOM_PK_CONTROL_LEFT:
        case DOM_PK_CONTROL_RIGHT:
            return FW_KEY_CONTROL;
        case DOM_PK_BACKSPACE:
            return FW_KEY_BACKSPACE;
        default:
            break;
    }

    return 0;
}


static bool onkeydown(void *user_data, int key, int code, int modifiers) {
    (void)user_data,(void)modifiers;

    int vk = kbd_pk2vk(code);
    keyboard_func(vk ? vk : key);

    if (code == DOM_PK_F5 || code == DOM_PK_F11 || code == DOM_PK_F12) {
        return 0;
    }
    return 1;
}

static bool onkeyup(void *user_data, int key, int code, int modifiers) {
    (void)user_data,(void)modifiers;

    int vk = kbd_pk2vk(code);
    keyboardup_func(vk ? vk : key);

    if (code == DOM_PK_F5 || code == DOM_PK_F11 || code == DOM_PK_F12) {
        return 0;
    }
    return 1;
}

/* open video context with given title, resolution, and scale */
int  vid_open(char *title, int width, int height, int scale, int flags) {
    JS_createCanvas(width, height);
    JS_setTitle(title);
    JS_addKeyDownEventListener(NULL, onkeydown);
    JS_addKeyUpEventListener(NULL, onkeyup);

    FW_curinfo.flags = flags;
    FW_curinfo.bytespp = 4; /* 4 bytes per pixel */

    int w, h, rw, rh, bpp;

    bpp = FW_curinfo.bytespp;

    FW_curinfo.width  = FW_BYTE_ALIGN(width , bpp);
    FW_curinfo.height = FW_BYTE_ALIGN(height, bpp);
    rw = FW_curinfo.width;
    rh = FW_curinfo.height;

    FW_curinfo.pitch = FW_CALC_PITCH(rw, bpp);

    w = rw, h = rh;
    FW_info("[hvid] creating HTML5 video context [%dx%d]", w, h);

    if (FW_curinfo.video) {
        free(FW_curinfo.video);
    }
    FW_curinfo.video = calloc(rw * rh, 4);
    if (FW_curinfo.video == NULL) {
        return FW_VERR_NOMEM;
    }

    /* refresh newly created display */
    vid_blit();
    vid_sync();

    return FW_VERR_OK;
}

void vid_blit       (void) {
    for (int h = 0; h < FW_curinfo.height; h++) {
        for (int w = 0; w < FW_curinfo.width; w++) {
            uint32_t pixel = FW_curinfo.video[h * FW_curinfo.width + w];
            pixel = ((pixel & 0xff0000) >> 16) | (pixel & 0x00ff00) | ((pixel & 0x0000ff) << 16) | 0xff000000;
            FW_curinfo.video[h * FW_curinfo.width + w] = pixel;
        }
    }

    JS_setPixelsAlpha((uint32_t*)FW_curinfo.video);
}

void vid_sync       (void) {
    // JS_setTimeout(0);
    JS_requestAnimationFrame();
}

VIDINFO *vid_getinfo(void) {
    return &FW_curinfo;
}

void sys_keybfunc(void (*keyboard)(int key))
{
    keyboard_func = keyboard;
}

void sys_keybupfunc(void (*keyboard)(int key))
{
    keyboardup_func = keyboard;
}

#endif
