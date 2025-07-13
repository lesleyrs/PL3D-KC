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
#include <js/key_codes.h>

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

// NOTE: browser keyCodes share their value for any keyboard location
// numpad arrows were overwriting normal arrow keys, need to make a code mapping
static struct {
   int keysym;
   int key;
} xl8tab[] = {
   {DOM_VK_A, 'a'},
   {DOM_VK_B, 'b'},
   {DOM_VK_C, 'c'},
   {DOM_VK_D, 'd'},
   {DOM_VK_E, 'e'},
   {DOM_VK_F, 'f'},
   {DOM_VK_G, 'g'},
   {DOM_VK_H, 'h'},
   {DOM_VK_I, 'i'},
   {DOM_VK_J, 'j'},
   {DOM_VK_K, 'k'},
   {DOM_VK_L, 'l'},
   {DOM_VK_M, 'm'},
   {DOM_VK_N, 'n'},
   {DOM_VK_O, 'o'},
   {DOM_VK_P, 'p'},
   {DOM_VK_Q, 'q'},
   {DOM_VK_R, 'r'},
   {DOM_VK_S, 's'},
   {DOM_VK_T, 't'},
   {DOM_VK_U, 'u'},
   {DOM_VK_V, 'v'},
   {DOM_VK_W, 'w'},
   {DOM_VK_X, 'x'},
   {DOM_VK_Y, 'y'},
   {DOM_VK_Z, 'z'},

   // {DOM_VK_A, 'A'},
   // {DOM_VK_B, 'B'},
   // {DOM_VK_C, 'C'},
   // {DOM_VK_D, 'D'},
   // {DOM_VK_E, 'E'},
   // {DOM_VK_F, 'F'},
   // {DOM_VK_G, 'G'},
   // {DOM_VK_H, 'H'},
   // {DOM_VK_I, 'I'},
   // {DOM_VK_J, 'J'},
   // {DOM_VK_K, 'K'},
   // {DOM_VK_L, 'L'},
   // {DOM_VK_M, 'M'},
   // {DOM_VK_N, 'N'},
   // {DOM_VK_O, 'O'},
   // {DOM_VK_P, 'P'},
   // {DOM_VK_Q, 'Q'},
   // {DOM_VK_R, 'R'},
   // {DOM_VK_S, 'S'},
   // {DOM_VK_T, 'T'},
   // {DOM_VK_U, 'U'},
   // {DOM_VK_V, 'V'},
   // {DOM_VK_W, 'W'},
   // {DOM_VK_X, 'X'},
   // {DOM_VK_Y, 'Y'},
   // {DOM_VK_Z, 'Z'},

   {DOM_VK_0, '0'},
   {DOM_VK_1, '1'},
   {DOM_VK_2, '2'},
   {DOM_VK_3, '3'},
   {DOM_VK_4, '4'},
   {DOM_VK_5, '5'},
   {DOM_VK_6, '6'},
   {DOM_VK_7, '7'},
   {DOM_VK_8, '8'},
   {DOM_VK_9, '9'},

   {DOM_VK_NUMPAD0, '0'},
   {DOM_VK_INSERT, '0'},
   {DOM_VK_NUMPAD1, '1'},
   {DOM_VK_END, '1'},
   {DOM_VK_NUMPAD2, '2'},
   // {DOM_VK_DOWN, '2'},
   {DOM_VK_NUMPAD3, '3'},
   {DOM_VK_PAGE_DOWN, '3'},
   {DOM_VK_NUMPAD4, '4'},
   // {DOM_VK_LEFT, '4'},
   {DOM_VK_NUMPAD5, '5'},
   // {DOM_VK_BEGIN, '5'},
   {DOM_VK_NUMPAD6, '6'},
   // {DOM_VK_RIGHT, '6'},
   {DOM_VK_NUMPAD7, '7'},
   {DOM_VK_HOME, '7'},
   {DOM_VK_NUMPAD8, '8'},
   // {DOM_VK_UP, '8'},
   {DOM_VK_NUMPAD9, '9'},
   {DOM_VK_PAGE_UP, '9'},
   {DOM_VK_DELETE, FW_KEY_BACKSPACE},
   {DOM_VK_DECIMAL, FW_KEY_BACKSPACE},

   {DOM_VK_ESCAPE, FW_KEY_ESCAPE},
   {DOM_VK_BACK_QUOTE, '~'},
   {DOM_VK_HYPHEN_MINUS, FW_KEY_MINUS},
   {DOM_VK_EQUALS, FW_KEY_EQUALS},
   {DOM_VK_SUBTRACT, FW_KEY_MINUS},
   {DOM_VK_ADD, FW_KEY_PLUS},
   {DOM_VK_ENTER, FW_KEY_ENTER},
   {DOM_VK_DIVIDE, '/'},
   {DOM_VK_MULTIPLY, '*'},
   {DOM_VK_BACK_SPACE, FW_KEY_BACKSPACE},
   {DOM_VK_TAB, FW_KEY_TAB},
   {DOM_VK_OPEN_BRACKET, '['},
   {DOM_VK_CLOSE_BRACKET, ']'},
   {DOM_VK_RETURN, FW_KEY_ENTER},
   {DOM_VK_SEMICOLON, ':'},
   {DOM_VK_QUOTE, '\''},
   {DOM_VK_BACK_SLASH, '\\'},
   {DOM_VK_LESS_THAN, '<'},
   {DOM_VK_COMMA, ','},
   {DOM_VK_PERIOD, '.'},
   {DOM_VK_SLASH, '/'},
   {DOM_VK_SPACE, FW_KEY_SPACE},
   {DOM_VK_DELETE, FW_KEY_BACKSPACE},
   {DOM_VK_LEFT, FW_KEY_ARROW_LEFT},
   {DOM_VK_RIGHT, FW_KEY_ARROW_RIGHT},
   {DOM_VK_UP, FW_KEY_ARROW_UP},
   {DOM_VK_DOWN, FW_KEY_ARROW_DOWN},

   {DOM_VK_SHIFT, FW_KEY_SHIFT},
   {DOM_VK_CONTROL, FW_KEY_CONTROL},
};

static bool onkeydown(void *user_data, int key_code, int modifiers) {
    (void)user_data,(void)modifiers;

    int n = (sizeof(xl8tab) / sizeof(*xl8tab));
     for (int j = 0; j < n; j++) {
        if (xl8tab[j].keysym == key_code) {
            keyboard_func(xl8tab[j].key);
            break;
        }
    }

    if (key_code == DOM_VK_F5 || key_code == DOM_VK_F11 || key_code == DOM_VK_F12) {
        return 0;
    }
    return 1;
}

static bool onkeyup(void *user_data, int key_code, int modifiers) {
    (void)user_data,(void)modifiers;

    int n = (sizeof(xl8tab) / sizeof(*xl8tab));
     for (int j = 0; j < n; j++) {
        if (xl8tab[j].keysym == key_code) {
            keyboardup_func(xl8tab[j].key);
            break;
        }
    }

    if (key_code == DOM_VK_F5 || key_code == DOM_VK_F11 || key_code == DOM_VK_F12) {
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
