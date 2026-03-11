#include "gfx_ili9341.h"
#include "tft_lcd_ili9341/ili9341/ili9341.h"
#include "font6x8.h"
#include <string.h>

extern uint16_t _width;
extern uint16_t _height;

/* ============================
   Estado interno
   ============================ */

static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t text_size = 1;
static uint16_t text_color = 0xFFFF;

#define COL_OFF 0x0000

#define MAX_BUTTONS 10
static GFX_Button *buttons[MAX_BUTTONS];
static int buttonCount = 0;

/* ============================
   Inicialização
   ============================ */

void gfx_init(void)
{
    // Nada necessário por enquanto
}

/* ============================
   Touch Transform
   ============================ */

void gfx_touchTransform(uint8_t rotation,
                        int rawX,
                        int rawY,
                        int *outX,
                        int *outY)
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

static void draw_char_at(int x, int y, char c)
{
    if (c < 32 || c > 127) c = '?';

    const uint8_t *ch = font6x8[c - 32];

    for (int col = 0; col < 6; col++)
    {
        uint8_t column = ch[col];

        for (int row = 0; row < 8; row++)
        {
            if ((column >> row) & 1)
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
    cursor_x += strlen(s) * 6 * text_size;
}

int gfx_getTextWidth(const char *s)
{
    return strlen(s) * 6 * text_size;
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
   Sistema de Botões
   ============================ */

void gfx_registerButton(GFX_Button *btn)
{
    if (buttonCount < MAX_BUTTONS)
    {
        buttons[buttonCount++] = btn;   // guarda ponteiro (NÃO copia)
    }
}

void gfx_updateButtons(int touchX, int touchY, int touched)
{
    for (int i = 0; i < buttonCount; i++)
    {
        GFX_Button *btn = buttons[i];

        int inside =
            touchX >= btn->x &&
            touchX <= btn->x + btn->w &&
            touchY >= btn->y &&
            touchY <= btn->y + btn->h;

        if (touched && inside)
        {
            if (!btn->pressed)
            {
                btn->pressed = 1;

                if (btn->callback)
                    btn->callback(btn);
            }
        }
        else
        {
            btn->pressed = 0;
        }
    }
}
