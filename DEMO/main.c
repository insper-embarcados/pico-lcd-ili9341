#include <stdio.h>
#include "pico/stdlib.h"

#include "tft_lcd_ili9341/ili9341/ili9341.h"
#include "tft_lcd_ili9341/gfx/gfx_ili9341.h"
#include "tft_lcd_ili9341/touch_resistive/touch_resistive.h"

#include "image_bitmap.h"

#define SCREEN_ROTATION  1   // 0 = RETRATO, 1 = PAISAGEM

int main(void)
{   
    stdio_init_all();

    LCD_initDisplay();
    LCD_setRotation(SCREEN_ROTATION);

    gfx_init();
    gfx_clear();

    extern uint16_t _width;
    extern uint16_t _height;

    /* ============================
       TÍTULO CENTRALIZADO
       ============================ */

    const char *msg1 = "Computacao";
    const char *msg2 = "Embarcada";
    const char *msg3 = "5s";

    gfx_setTextSize(1);             // Tamanho 1 (6x8 pixels por caractere)
    gfx_setTextColor(0xF800);       // Vermelho
    gfx_drawText(130, 10, msg1);    // Escreve a msg1 na posição (130, 10)

    gfx_setTextSize(2);             // Tamanho 2 (12x16 pixels por caractere)
    gfx_setTextColor(0x07E0);       // Verde
    gfx_drawText(106, 25, msg2);    // Escreve a msg2 na posição (106, 25)
    
    gfx_setTextSize(4);             // Tamanho 4 (24x32 pixels por caractere)
    gfx_setTextColor(0x001F);       // Azul
    gfx_drawText(136, 50, msg3);    // Escreve a msg3 na posição (136, 50)


    /* ============================
       FORMAS GEOMÉTRICAS
       ============================ */

    // Círculo à esquerda
    gfx_drawCircle(
        (_width / 6),               // X centralizado na primeira sexta parte da tela
        (_height / 2),              // Y centralizado verticalmente
        30,                         // Raio de 30 pixels
        0xF81F,                     // Cor magenta
        10);                        // Espessura de 10 pixels

    // Quadrado à direita
    gfx_drawRect(
        (_width / 4) * 3,           // X centralizado na terceira sexta parte da tela
        (_height / 2) - 30,         // Y centralizado verticalmente, ajustado para o tamanho do quadrado
        60,                         // Largura de 60 pixels
        60,                         // Altura de 60 pixels  
        0x07FF,                     // Cor ciano
        3);                         // Espessura de 3 pixels                       

    /* ============================
       IMAGEM BITMAP
       ============================ */

    gfx_drawBitmap(
        117,                        // Posição em X
        140,                        // Posição em Y
        image_Layer_1_bits,         // Vetor de bytes do bitmap
        92,                         // Largura do bitmap
        92,                         // Altura do bitmap
        0xFFFF                      // Cor branca para os pixels "ligados" do bitmap
    );       

    /* ============================
       TOUCH
       ============================ */

    configure_touch();

    int px, py;

     while (true)
{
    if (readPoint(&px, &py))
    {
        int drawX, drawY;

        gfx_touchTransform(SCREEN_ROTATION,
                           px, py,
                           &drawX, &drawY);

        // Limpa faixa inferior
        gfx_fillRect(
            4,                      // X inicial
            _height - 40,           // Y inicial
            78,                     // Largura da faixa (toda a largura da tela)
            30,                     // Altura da faixa
            0xF800                  // Cor vermelho para limpar a área
        );                

        gfx_setTextSize(1);         // Seta para Tamanho 1 (6x8 pixels por caractere)
        gfx_setTextColor(0xFFFF);   // Cor branca para o texto

        // Linha 1
        gfx_setCursor(6, _height - 35);     // Posição para a primeira linha de texto
        gfx_print("ULTIMO TOQUE:");         // Escreve o título "ULTIMO TOQUE:"

        // Linha 2
        char buffer[40];                                // Buffer para armazenar as coordenadas formatadas
        sprintf(buffer, "X:%d Y:%d", drawX, drawY);     // Formata as coordenadas em uma string   

        gfx_setCursor(12, _height - 20);                // Posição para a segunda linha de texto, um pouco abaixo da primeira
        gfx_print(buffer);                              // Escreve as coordenadas do toque formatadas

        sleep_ms(10);
    }
}
}