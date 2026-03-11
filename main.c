#include <stdio.h>
#include "pico/stdlib.h"

#include "tft_lcd_ili9341/gfx/gfx_ili9341.h"
#include "tft_lcd_ili9341/ili9341/ili9341.h"
#include "tft_lcd_ili9341/touch_resistive/touch_resistive.h"

#include "image_bitmap.h"           //Header com os bitmaps do LED ON e OFF

// Propriedades do LCD
#define SCREEN_ROTATION 1           // 0 = RETRATO, 1 = PAISAGEM
const int width = 320;             // Variável global definida em gfx_ili9341.c que armazena a largura da tela
const int height = 240;            // Variável global definida em gfx_ili9341.c que armazena a altura da tela

// Posição da imagem na tela
const int  ledImgPosX = (width - 47) / 2;
const int  ledImgPosY = (height - 82) / 2;

// flag btn (não precisa ser volatile pq não é callback de HW!)
int f_btn = 0;

// callback do botão
void ledButtonCallback(GFX_Button *btn) {
    f_btn = 1;
}

// Desenha o LED conforme estado
void drawLed(int ledState) {
    gfx_fillRect(ledImgPosX, ledImgPosY, 47, 82, 0x0000);                       //Limpa a área do LED 

    if (ledState)
        gfx_drawBitmap(ledImgPosX, ledImgPosY, image_LED_ON, 47, 82, 0xF800);   // Desenha o LED aceso (vermelho)
    else
        gfx_drawBitmap(ledImgPosX, ledImgPosY, image_LED_OFF, 47, 82, 0xFFFF);  // Desenha o LED apagado (branco)
}

int main() {
    stdio_init_all();

    //### LCD
    LCD_initDisplay();
    LCD_setRotation(SCREEN_ROTATION);   // Ajusta a rotação da tela conforme definido

    //### TOUCH
    configure_touch();                  // Configura o touch resistivo

    //### GFX
    gfx_init();                         // Inicializa o GFX
    gfx_clear();                        // Limpa a tela

    gfx_setTextSize(2);                                 // Define o tamanho do texto
    gfx_setTextColor(0x07E0);                           // Define a cor do texto (verde)

    gfx_drawText(
        width/6,                                       // Posição horizontal do texto
        10,                                             // Posição vertical do texto
        "PicoDock LED Toggle"                           // Texto a ser exibido
    );
 
    // Estado do LED (0 = OFF, 1 = ON)
    int ledState = 0;
    drawLed(ledState); // Desenha o LED conforme o estado inicial (apagado)

    // Criação do botão para o LED, invisivel, mas que cobre a área da imagem do LED para detectar os toques
    GFX_Button ledButton = {            
        .x = ledImgPosX,                // Posição horizontal do botão (mesma da imagem do LED)
        .y = ledImgPosY,                // Posição vertical do botão (mesma da imagem do LED)
        .w = 47,                        // Largura do botão (mesma da largura da imagem do LED)
        .h = 82,                        // Altura do botão (mesma da altura da imagem do LED)
        .callback = ledButtonCallback   // Função callback que será chamada quando o botão for pressionado
    };

    gfx_registerButton(&ledButton);     // Registra o botão para que seja detectado toques pelo GFX

    while (true) {
        int touchRawX, touchRawY;               // Variaveis para armazenar as coordenadas brutas do toque
        int screenTouchX, screenTouchY  = 0;    // Variaveis para armazenar as coordenadas do toque transformadas para a tela

        int touchDetected = readPoint(&touchRawX, &touchRawY);  // Lê as coordenadas do toque e armazena em touchRawX e touchRawY,
                                                                // a função retorna 1 se um toque for detectado ou 0 caso contrário

        if (touchDetected)  {                                                       
            gfx_touchTransform(SCREEN_ROTATION,                 // Se um toque for detectado, transforma as coordenadas brutas do toque 
                               touchRawX, touchRawY,            // para as coordenadas da tela considerando a rotação
                               &screenTouchX, &screenTouchY);

                                                                            // Atualiza o estado dos botões registrados no GFX, 
            gfx_updateButtons(screenTouchX, screenTouchY, touchDetected);   // verificando se o toque ocorreu dentro da área de 
                                                                            // algum botão e chamando a função callback correspondente                             
        }

        if (f_btn) {
            ledState = !ledState;   // Alterna o estado do LED
            drawLed(ledState);      // Redesenha o LED com o novo estado
            f_btn = 0;
        }

        sleep_ms(10);
    }

    return 0;
}
