#ifndef GFX_ILI9341_H
#define GFX_ILI9341_H

#include <stdint.h>

typedef struct GFX_Button GFX_Button;

typedef void (*GFX_ButtonCallback)(GFX_Button *btn);

struct GFX_Button {
    int x;
    int y;
    int w;
    int h;
    const uint8_t *bitmap;
    uint16_t color;
    GFX_ButtonCallback callback;
    int pressed;
};




typedef struct { 
    int16_t width; 
    int16_t height; 
    const uint8_t *data; 
} GFX_Bitmap;

typedef struct {
    int x;
    int y;
    const GFX_Bitmap *image;
} GFX_BitmapButton;

/* Inicialização */
void gfx_init(void);

/* Tela */
void gfx_clear(void);
void gfx_fillRect(int x, int y, int w, int h, uint16_t color);
void gfx_drawRect(int x,
                  int y,
                  int w,
                  int h,
                  uint16_t color,
                  int thickness);

/* Texto */
void gfx_setCursor(int x, int y);
void gfx_setTextSize(uint8_t s);
void gfx_setTextColor(uint16_t color);

void gfx_print(const char *s);
void gfx_drawText(int x, int y, const char *s);
int  gfx_getTextWidth(const char *s);

void gfx_drawBitmap(int16_t x,
                    int16_t y,
                    const uint8_t *bitmap,
                    int16_t w,
                    int16_t h,
                    uint16_t color);

void gfx_drawCircle(int16_t x,
                    int16_t y,
                    int16_t r,
                    uint16_t color,
                    int thickness);
void gfx_touchTransform(uint8_t rotation,
                        int rawX, int rawY,
                        int *outX, int *outY);

void gfx_But_drawRect(GFX_Button *btn, uint16_t color);
int gfx_But_isPressed(GFX_Button *btn, int touchX, int touchY);

void gfx_But_drawBitmap(GFX_BitmapButton *btn,
                        uint16_t color,
                        uint16_t bg);

int gfx_But_isPressedBitmap(GFX_BitmapButton *btn,
                            int touchX,
                            int touchY);


void gfx_registerButton(GFX_Button *btn);
void gfx_updateButtons(int touchX, int touchY, int touched);

#endif
