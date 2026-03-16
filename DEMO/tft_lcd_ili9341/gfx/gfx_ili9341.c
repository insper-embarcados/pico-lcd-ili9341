#include "gfx_ili9341.h"
#include "tft_lcd_ili9341/ili9341/ili9341.h"
#include "font6x8.h"
#include <string.h>
#include <stdlib.h>

extern uint16_t _width;
extern uint16_t _height;

/* ============================
   Estado interno
   ============================ */

static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t text_size = 1;
static uint16_t text_color = 0xFFFF;

#define COL_ON  0xFFFFu
#define COL_OFF 0x0000u

/* ============================
   Inicialização
   ============================ */

void gfx_init(void)
{
    // nada necessário
}

/* ============================
   Touch Transform
   ============================ */

void gfx_touchTransform(uint8_t rotation,
                        int rawX, int rawY,
                        int *outX, int *outY)
{
    if (rotation == 1)   // PAISAGEM
    {
        *outX = _width - rawY;
        *outY = rawX;
    }
    else                 // RETRATO
    {
        *outX = _width - rawX;
        *outY = _height - rawY;
    }
}

/* ============================
   Pixel de baixo nível
   ============================ */

static inline void draw_pixel_lowlevel(int x, int y, uint16_t color)
{
    if (x < 0 || y < 0 || x >= (int)_width || y >= (int)_height)
        return;

    LCD_WritePixel(x, y, color);
}

/* ============================
   Primitivas
   ============================ */

void gfx_fillRect(int x, int y, int w, int h, uint16_t color)
{
    if (w <= 0 || h <= 0) return;

    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = (x + w) > _width ? _width : (x + w);
    int y1 = (y + h) > _height ? _height : (y + h);

    for (int yy = y0; yy < y1; yy++)
        for (int xx = x0; xx < x1; xx++)
            draw_pixel_lowlevel(xx, yy, color);
}

void gfx_drawRect(int x,
                  int y,
                  int w,
                  int h,
                  uint16_t color,
                  int thickness)
{
    if (w <= 0 || h <= 0) return;
    if (thickness < 1) thickness = 1;

    for (int t = 0; t < thickness; t++)
    {
        gfx_fillRect(x + t, y + t, w - (2 * t), 1, color);
        gfx_fillRect(x + t, y + h - 1 - t, w - (2 * t), 1, color);
        gfx_fillRect(x + t, y + t, 1, h - (2 * t), color);
        gfx_fillRect(x + w - 1 - t, y + t, 1, h - (2 * t), color);
    }
}

void gfx_clear(void)
{
    gfx_fillRect(0, 0, _width, _height, COL_OFF);
}

/* ============================
   Texto
   ============================ */

void gfx_setCursor(int x, int y)
{
    cursor_x = x;
    cursor_y = y;
}

void gfx_setTextSize(uint8_t s)
{
    if (s < 1) s = 1;
    text_size = s;
}

void gfx_setTextColor(uint16_t color)
{
    text_color = color;
}

static void draw_scaled_pixel(int x, int y, uint8_t on)
{
    if (on)
    {
        gfx_fillRect(x, y, text_size, text_size, text_color);
    }
}

static void draw_char_at(int x, int y, char c)
{
    if (c < 32 || c > 127) c = '?';

    const uint8_t *ch = font6x8[c - 32];

    for (int col = 0; col < 6; col++)
    {
        uint8_t column = ch[col];

        for (int row = 0; row < 8; row++)
        {
            uint8_t on = (column >> row) & 1;

            if (on)
            {
                gfx_fillRect(x + col * text_size,
                             y + row * text_size,
                             text_size,
                             text_size,
                             text_color);
            }
        }
    }
}

void gfx_drawText(int x, int y, const char *s)
{
    int cx = x;

    while (*s)
    {
        draw_char_at(cx, y, *s);
        cx += 6 * text_size;
        s++;
    }
}

void gfx_print(const char *s)
{
    gfx_drawText(cursor_x, cursor_y, s);
    cursor_x += gfx_getTextWidth(s);
}

int gfx_getTextWidth(const char *s)
{
    return (int)(strlen(s) * 6 * text_size);
}

/* ============================
   Bitmap
   ============================ */

void gfx_drawBitmap(int16_t x,
                    int16_t y,
                    const uint8_t *bitmap,
                    int16_t w,
                    int16_t h,
                    uint16_t color)
{
    if (!bitmap || w <= 0 || h <= 0) return;

    int bytes_per_row = (w + 7) / 8;

    for (int16_t j = 0; j < h; j++)
    {
        int16_t py = y + j;
        if (py < 0 || py >= _height) continue;

        for (int16_t i = 0; i < w; i++)
        {
            int16_t px = x + i;
            if (px < 0 || px >= _width) continue;

            int byteIndex = j * bytes_per_row + (i >> 3);
            uint8_t byte = bitmap[byteIndex];

            if (byte & (0x80 >> (i & 7)))
                draw_pixel_lowlevel(px, py, color);
        }
    }
}

/* ============================
   Círculo
   ============================ */

void gfx_drawCircle(int16_t x0,
                    int16_t y0,
                    int16_t r,
                    uint16_t color,
                    int thickness)
{
    if (thickness < 1) thickness = 1;

    for (int t = 0; t < thickness; t++)
    {
        int16_t radius = r - t;

        int16_t f = 1 - radius;
        int16_t ddF_x = 1;
        int16_t ddF_y = -2 * radius;
        int16_t x = 0;
        int16_t y = radius;

        draw_pixel_lowlevel(x0, y0 + radius, color);
        draw_pixel_lowlevel(x0, y0 - radius, color);
        draw_pixel_lowlevel(x0 + radius, y0, color);
        draw_pixel_lowlevel(x0 - radius, y0, color);

        while (x < y)
        {
            if (f >= 0)
            {
                y--;
                ddF_y += 2;
                f += ddF_y;
            }

            x++;
            ddF_x += 2;
            f += ddF_x;

            draw_pixel_lowlevel(x0 + x, y0 + y, color);
            draw_pixel_lowlevel(x0 - x, y0 + y, color);
            draw_pixel_lowlevel(x0 + x, y0 - y, color);
            draw_pixel_lowlevel(x0 - x, y0 - y, color);
            draw_pixel_lowlevel(x0 + y, y0 + x, color);
            draw_pixel_lowlevel(x0 - y, y0 + x, color);
            draw_pixel_lowlevel(x0 + y, y0 - x, color);
            draw_pixel_lowlevel(x0 - y, y0 - x, color);
        }
    }
}

/* ============================
   Botão Retangular
   ============================ */

void gfx_But_drawRect(GFX_Button *btn, uint16_t color)
{
    gfx_drawRect(btn->x, btn->y, btn->w, btn->h, color, 1);
}

int gfx_But_isPressed(GFX_Button *btn, int touchX, int touchY)
{
    if (touchX >= btn->x &&
        touchX <= btn->x + btn->w &&
        touchY >= btn->y &&
        touchY <= btn->y + btn->h)
    {
        return 1;
    }

    return 0;
}

/* ============================
   Botão Bitmap
   ============================ */

void gfx_But_drawBitmap(GFX_BitmapButton *btn,
                        uint16_t color,
                        uint16_t bg)
{
    if (!btn || !btn->image) return;

    gfx_drawBitmap(btn->x,
               btn->y,
               btn->image->data,
               btn->image->width,
               btn->image->height,
               color);
}

int gfx_But_isPressedBitmap(GFX_BitmapButton *btn,
                            int touchX,
                            int touchY)
{
    if (!btn || !btn->image) return 0;

    int w = btn->image->width;
    int h = btn->image->height;

    if (touchX >= btn->x &&
        touchX <= btn->x + w &&
        touchY >= btn->y &&
        touchY <= btn->y + h)
    {
        return 1;
    }

    return 0;
}