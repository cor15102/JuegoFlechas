// Autores:    240 X 320
// Alvaro Torres; carnet: 16003
// Rodrigo Corona; carnet: 15102
// Digital 2, Sección 12
// Proyecto 2, Video juego

//NOTA: Se utilizaron ejemplos vistos en clase como guía para la elaboración de este código
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <SPI.h>
#include <SD.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};
const int chipSelect = 12;

void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES PARA CONTROLES
uint8_t U1;             // BOTON ARRIBA JUGADOR 1
uint8_t D1;             // BOTON ABAJO JUGADOR 1
uint8_t L1;             // BOTON IZQUIERDA JUGADOR 1
uint8_t R1;             // BOTON DERECHA JUGADOR 1

uint8_t U2;             // BOTON ARRIBA JUGADOR 2
uint8_t D2;             // BOTON ABAJO JUGADOR 2
uint8_t L2;             // BOTON IZQUIERDA JUGADOR 2
uint8_t R2;             // BOTON DERECHA JUGADOR 2

// VARIABLES PARA CÓDIGO
uint8_t z = 1;          // FOTOD DE INICIO Y CARGAR MEMORIA
uint8_t velocidad = 0;  // VELOCIDAD ESCOGIDA
uint8_t cont1 = 0;      // PUNTOS DE JUGADOR 1
uint8_t cont2 = 0;      // PUNTOS DE JUGADOR 2

int buzzer = 40;

const char* keg[10] = {"0","1","2","3","4","5","6","7","8","9"};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFINIMOS BOTONES
#define U1 PA_6
#define D1 PA_7
#define L1 PE_3
#define R1 PE_2
 
#define U2 PD_7
#define D2 PF_4
#define L2 PA_3
#define R2 PA_2

#define nota_1 500
#define nota_2 800
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP
void setup(void)
{
  // DEFINIMOS BOTONES COMO ENTRADAS
  pinMode(U1, INPUT);
  pinMode(D1, INPUT);
  pinMode(L1, INPUT);
  pinMode(R1, INPUT);

  pinMode(U2, INPUT);
  pinMode(D2, INPUT);
  pinMode(L2, INPUT);
  pinMode(R2, INPUT);

  pinMode(buzzer,OUTPUT); // Pin conectado al Buzzer PF2

  // INICIAMOS COMUNICACIÓN SERIAL
  Serial.begin(9600);

  // INICIAMOS LA LCD
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  LCD_Init();
  LCD_Clear( 0xFFFF);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN CODE
void loop (void) 
{

inicio:

  //  -------- PANTALLA DE INICIO -------- //
  while (z == 1)             // MOSTRAR PANTALLA DE INICIO
  {
    uint8_t lectura = LOW;    // INICIALIZO VARIABLE
    uint8_t q = 0;            // INICIALIZO VARIABLE

    while (z == 1) 
    {                                   // MIENTRAS SE CUMPLA CONDICION DE MOSTRAR PANTALLA
      String text1 = "Press UP button";                 // PALABRA 1 A DESPLEGAR
      String text2 = "to start";                        // PALABRA 2 A DESPLEGAR
      LCD_Print(text1, 38, 100, 2,  0x0000, 0xFFFF);    // POSICIÓN DONDE DESPLEGAMOS PALABRA 1
      LCD_Print(text2, 90, 130, 2,  0x0000, 0xFFFF);    // POSICIÓN DONDE DESPLEGAMOS PALABRA 2

      lectura = digitalRead(U1);        // LECTURA DE BOTON UP DE JUGADOR 1
      if (lectura == HIGH && q == 0)    // SI PRECIONAMOS BOTON DEL JUGADOR 1
      {
        q = 1;                          // VARIABLE PARA ANTI REBOTE
      }

      if (lectura == LOW && q == 1)     // SI JUGADOR YA SOLTÓ EL BOTON ;
      {
        LCD_Clear( 0xFFFF);                             // PINTO PANTALLA DE BLANCO
        q = 0;                                          // REINICIO EL ANTI REBOTE
        z = 2;
        FillRect(0, 0, 160, 120, 0x0000);               // COLOCO CUADRO NEGRO EN CASILLA SUPERIOR IZQUIERDA
        FillRect(160, 120, 160, 120, 0x0000);           // COLOCO CUADRO NEGRO EN CASILLA INFERIOR DERECHA

      }
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  -------- MENU DE VELOCIDAD -------- //
  while (z == 2) 
  {
    uint8_t v1 = LOW;   // VARIABLE PARA VELOCIDAD EASY
    uint8_t v2 = LOW;   // VARIABLE PARA VELOCIDAD NORMAL
    uint8_t v3 = LOW;   // VARIABLE PARA VELOCIDAD HARD
    uint8_t v4 = LOW;   // VARIABLE PARA VELOCIDAD ULTRA HARD
    uint8_t q = 0;    // Antirebote

    while (z == 2) 
    {
      String text1 = "EASY MODE";                       // PALABRA 1 A DESPLEGAR
      String text2 = "UP BUTTOM";                       // PALABRA 2 A DESPLEGAR
      LCD_Print(text1, 45, 45, 1,  0xFFFF, 0x0000);     // POSICIÓN DONDE DESPLEGAMOS PALABRA 1
      LCD_Print(text2, 45, 55, 1,  0xFFFF, 0x0000);     // POSICIÓN DONDE DESPLEGAMOS PALABRA 2

      String text3 = "NORMAL MODE";                           // PALABRA 3 A DESPLEGAR
      String text4 = "RIGHT BUTTOM";                          // PALABRA 4 A DESPLEGAR
      LCD_Print(text3, 45 + 155, 45, 1,  0x0000, 0xFFFF);     // POSICIÓN DONDE DESPLEGAMOS PALABRA 3
      LCD_Print(text4, 45 + 151, 55, 1,  0x0000, 0xFFFF);     // POSICIÓN DONDE DESPLEGAMOS PALABRA 4

      String text5 = "HARD MODE";                             // PALABRA 5 A DESPLEGAR
      String text6 = "DOWN BUTTOM";                           // PALABRA 6 A DESPLEGAR
      LCD_Print(text5, 45, 45 + 120, 1,  0x0000, 0xFFFF);     // POSICIÓN DONDE DESPLEGAMOS PALABRA 5
      LCD_Print(text6, 38, 55 + 120, 1,  0x0000, 0xFFFF);     // POSICIÓN DONDE DESPLEGAMOS PALABRA 6

      String text7 = "EXTREME MODE";                              // PALABRA 7 A DESPLEGAR
      String text8 = "LEFT BUTTOM";                               // PALABRA 8 A DESPLEGAR
      LCD_Print(text7, 45 + 149, 45 + 120, 1,  0xFFFF, 0x0000);   // POSICIÓN DONDE DESPLEGAMOS PALABRA 7
      LCD_Print(text8, 45 + 153, 55 + 120, 1,  0xFFFF, 0x0000);   // POSICIÓN DONDE DESPLEGAMOS PALABRA 8

      v1 = digitalRead(U1);          // SELECCION DIFICULTAD EASY
      v2 = digitalRead(R1);          // SELECCION DIFICULTAD NORMAL
      v3 = digitalRead(D1);          // SELECCION DIFICULTAD HARD
      v4 = digitalRead(L1);          // SELECCIONÓDIFICULTAD EXTREME

      // -- SELECIÓN DE VELOCIDAD EASY --   //
      if (v1 == HIGH && q == 0)     // SELECCIONÓ MODO EASY
      {
        q = 1;                      // VARIABLE ANTI REBOTE
      }

      if (v1 == LOW && q == 1)              // ANTIREBOTE PARA BOTÓN
      {            
        velocidad = 1000;                   // VELOCIDAD PARA DESPLEGAR FIGURAS DURANTE EL JUEGO
        q = 0;                              // LIMPIO ANTI REBOTE
        z = 3;                              // VARIABLE PARA CONTEO REGRESIVO
        FillRect(0, 0, 160, 120, 0xFFE0);   // MARCO OPCIÓN SELECCIONADA
        delay(500);                         // MUESTRO OPCIÓN SELECCIONADA POR MEDIO SEGUNDO
      }

      // -- SELECIÓN DE VELOCIDAD NORMAL -- //
      if (v2 == HIGH && q == 0)   // SELECCIONÓ MODO NORMAL
      {
        q = 2;                    // VARIABLE ANTI REBOTE
      }

      if (v2 == LOW && q == 2) {              // ANTIREBOTE PARA BOTÓN
        velocidad = 600;                    // VELOCIDAD PARA DESPLEGAR FIGURAS DURANTE EL JUEGO
        q = 0;                              // LIMPIO ANTI REBOTE
        z = 3;                              // VARIABLE PARA CONTEO REGRESIVO
        FillRect(160, 0, 160, 120, 0xFFE0);   // MARCO OPCIÓN SELECCIONADA
        delay(500);                         // MUESTRO OPCIÓN SELECCIONADA POR MEDIO SEGUNDO
      }

      // -- SELECIÓN DE VELOCIDAD HARD --   //
      if (v3 == HIGH && q == 0) {           // SELECCIONÓ MODO HARD
        q = 3;                              // VARIABLE ANTI REBOTE
      }

      if (v3 == LOW && q == 3) {              // ANTIREBOTE PARA BOTON
        velocidad = 400;                    // VELOCIDAD PARA DESPLEGAR FIGURAS DURANTE EL JUEGO
        q = 0;                              // LIMPIO ANTI REBOTE
        z = 3;                              // VARIABLE PARA CONTEO REGRESIVO
        FillRect(0, 120, 160, 120, 0xFFE0);   // MARCO OPCIÓN SELECCIONAD
        delay(500);                         // MUESTRO OPCIÓN SELECCIONADA POR MEDIO SEGUNDO
      }

      // -- SELECIÓN DE VELOCIDAD ULTRA HARD -- //
      if (v4 == HIGH && q == 0) {                        // SELECCIONÓ EL MODO ULTRA HARD
        q = 4;                              // VARIABLE ANTI REBOTE
      }

      if (v4 == LOW && q == 4) {          // ANTIREBOTE PARA BOTON
        velocidad = 1;                      // VELOCIDAD PARA DESPLEGAR FIGURAS DURANTE EL JUEGO
        q = 0;                              // LIMPIO ANTI REBOTE
        z = 3;                              // VARIABLE PARA CONTEO REGRESIVO
        FillRect(160, 120, 160, 120, 0xFFE0);   // MARCO OPCIÓN SELECCIONAD
        delay(500);                         // MUESTRO OPCIÓN SELECCIONADA POR MEDIO SEGUNDO
      }
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  -------- CUENTA REGRESIVA  -------- //
  if (z == 3) {                           // MOSTRAR CUENTA REGRESIVA
    LCD_Clear(0xFFFF);                    // PINTO PANTALA DE BLANCO
    String text10 = "3";                  // MUESTRO NÚMERO 3
    LCD_Print(text10, 158, 116, 2,  0xF800, 0xFFFF);  //POSICIÓN DONDE MOSTRAR NÚMERO
    delay(300);                           // ESPERA ENTRE EL SIGUIENTE NÚMERO

    LCD_Clear(0xFFFF);                    // PINTO PANTALA DE BLANCO
    String text11 = "2";                  // MUESTRO NÚMERO 2
    LCD_Print(text11, 158, 116, 2,  0xF800, 0xFFFF);  //POSICIÓN DONDE MOSTRAR NÚMERO
    delay(300);                           // ESPERA ENTRE EL SIGUIENTE NÚMERO

    LCD_Clear(0xFFFF);                    // PINTO PANTALA DE BLANCO
    String text12 = "1";                  // MUESTRO NÚMERO 1
    LCD_Print(text12, 158, 116, 2,  0xF800, 0xFFFF);  //POSICIÓN DONDE MOSTRAR NÚMERO
    delay(300);                           // ESPERA ENTRE PARA SIGNO GO

    LCD_Clear(0xFFFF);                    // PINTO PANTALA DE BLANCO
    String text13 = "GO";                 // MUESTRO PALABRA GO
    LCD_Print(text13, 150, 113, 2,  0xF800, 0xFFFF);  //POSICIÓN DONDE MOSTRAR PALABRA
    delay(300);                           // ESPERA

    z = 4;                                // VARIABLE PARA ENTRAR AL JUEGO
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (z == 4) {                           // ENTRO AL JUEGO

    LCD_Clear(0x0000);                    // PINTO PANTALA DE un color

    LCD_Bitmap(290, 0, 30, 30, GI);       // MUESTRO FLECHA izquierda EN PANTALLA
    LCD_Bitmap(290, 30, 30, 30, GU);      // MUESTRO FLECHA arriba EN PANTALLA
    LCD_Bitmap(290, 60, 30, 30, GD);      // MUESTRO FLECHA abajo EN PANTALLA
    LCD_Bitmap(290, 90, 30, 30, GR);      // MUESTRO FLECHA derecha EN PANTALLA
    FillRect(0, 120, 290, 1, 0xFFFF);       // MUESTRO LINEA DIVISORA
    LCD_Bitmap(290, 121, 30, 30, GI);     // MUESTRO FLECHA EN PANTALLA
    LCD_Bitmap(290, 150, 30, 30, GU);     // MUESTRO FLECHA EN PANTALLA
    LCD_Bitmap(290, 180, 30, 30, GD);     // MUESTRO FLECHA EN PANTALLA
    LCD_Bitmap(290, 210, 30, 30, GR);     // MUESTRO FLECHA EN PANTALLA

    uint8_t y;              // VARIABLE PARA NÚMERO ALEATORIO ENTRE 1 Y 4
    uint8_t q1 = 0;         // VARIABLE DE INGRESO JUGADOR 1
    uint8_t q2 = 0;         // VARIABLE DE INGRESO JUGADOR 2
    uint8_t general = 0;    // CICLO DE CONTEO DE FLECHAS
    int x = 0;              // VARIABLE PARA CICLO DE DEZPLASAMIENTO
    uint8_t estado1, estado2;

    while (z == 4) 
    {
      for (general = 0; general < 5; general++) {     // FOR PARA CUMPLIR LAS 20 FLECHAS
        randomSeed(analogRead(0));
        y = random(1, 5);                             // GENERO NÚMERO ALEATORIO ENTRE 1 Y 4
        //y = 1;
        
        switch (y) {                                  // SWITCH PARA VER QUE FLECHA SALIÓ
          case 1:    // MUESTRO FLECHA IZQUIERDAAAAAA
            
            for (x = 11; x <= 290; x++) {             // MOVER LA FLECHA HASTA QUE LLEGUE A POSICIÓN INDICADA
              FillRect(x - 30, 0, 30, 30, 0x00);      // OCULTO FLECHA ANTES DESPLEGADA PARA JUGADOR 1
              FillRect(x - 30, 122, 30, 30, 0x00);    // OCULTO FLECHA ANTES DESPLEGADA PARA JUGADOR 2                 
              LCD_Bitmap(x, 0, 30, 30, GI);           // MUESTRO FLECHA EN POSICIÓN INDICADA PARA JUGADOR 1
              LCD_Bitmap(x, 122, 30, 30, GI);         // MUESTRO FLECHA EN POSICIÓN INDICADA PARA JUGADOR 2
              x = x + 30;                             // AVANZO LA FLECHA A SIGUIENTE POSICIÓN
              delay(velocidad);                       // ESPERAR PARA MOSTRAR LA SIGUIENTE FLECHA

              q1 = digitalRead(L1);
              q2 = digitalRead(L2);

              if (q1 == HIGH && x <= 260)
              {
                estado1 = 1;
              }

              if (q1 == LOW && x > 260)
              {
                estado1 = 0;
              }

              if (q2 == LOW && x <= 260)
              {
                estado2 = 1;
              }

              if (q2 == LOW && x > 260)
              {
                estado2 = 0;
              }
            } //  ==== Cierra el FOR =====
            
            q1 = digitalRead(L1);
            q2 = digitalRead(L2);

            if (q1 == HIGH && estado1 == 0)
            {
              cont1++;
              LCD_Bitmap(290, 0, 30, 30, DI);           // MUESTRO FLECHA PARA INDICADORA DE PRESIONAR
              beep(nota_1,500);
            }

            if (q2 == HIGH && estado2 == 0)
            {
              cont2++;
              LCD_Bitmap(290, 120, 30, 30, DI);         // MUESTRO FLECHA PARA INDICADORA DE PRESIONAR
              beep(nota_2,500);
            }

            q1 = LOW;  // Reiniciamos la lectura digital
            q2 = LOW;  // Reiniciamos la lectura digital
            break;

          case 2:   // MUESTRO FLECHA ARRIBAAA
          
            for (x = 11; x <= 290; x++) {             // MOVER LA FLECHA HASTA QUE LLEGUE A POSICIÓN INDICADA
              FillRect(x - 30, 30, 30, 30, 0x00);     // OCULTO FLECHA ANTES DESPLEGADA PARA JUGADOR 1
              FillRect(x - 30, 150, 30, 30, 0x00);    // OCULTO FLECHA ANTES DESPLEGADA PARA JUGADOR 2
              LCD_Bitmap(x, 30, 30, 30, GU);          // MUESTRO FLECHA EN POSICIÓN INDICADA PARA JUGADOR 1
              LCD_Bitmap(x, 150, 30, 30, GU);         // MUESTRO FLECHA EN POSICIÓN INDICADA PARA JUGADOR 2
              x = x + 30;                             // AVANZO LA FLECHA A SIGUIENTE POSICIÓN
              delay(velocidad);                       // ESPERAR PARA MOSTRAR LA SIGUIENTE FLECHA

              q1 = digitalRead(U1);
              q2 = digitalRead(U2);

              if (q1 == HIGH && x <= 260)
              {
                estado1 = 1;
              }

              if (q1 == LOW && x > 260)
              {
                estado1 = 0;
              }

              if (q2 == LOW && x <= 260)
              {
                estado2 = 1;
              }

              if (q2 == LOW && x > 260)
              {
                estado2 = 0;
              }
            } //  ==== Cierra el FOR =====
            
            q1 = digitalRead(U1);
            q2 = digitalRead(U2);

            if (q1 == HIGH && estado1 == 0)
            {
              cont1++;
              LCD_Bitmap(290, 30, 30, 30, DU);           // MUESTRO FLECHA PARA INDICADORA DE PRESIONAR
              beep(nota_1,500);
            }

            if (q2 == HIGH && estado2 == 0)
            {
              cont2++;
              LCD_Bitmap(290, 150, 30, 30, DU);         // MUESTRO FLECHA PARA INDICADORA DE PRESIONAR
              beep(nota_2,500);
            }

            q1 = LOW;  // Reiniciamos la lectura digital
            q2 = LOW;  // Reiniciamos la lectura digital
            break;

          case 3:   // MUESTRO FLECHA abajooooooo
          
            for (x = 11; x <= 290; x++) {             // abajo MOVER LA FLECHA HASTA QUE LLEGUE A POSICIÓN INDICADA
              FillRect(x - 30, 60, 30, 30, 0x00);     // OCULTO FLECHA ANTES DESPLEGADA PARA JUGADOR 1
              FillRect(x - 30, 180, 30, 30, 0x00);    // OCULTO FLECHA ANTES DESPLEGADA PARA JUGADOR 2
              LCD_Bitmap(x, 60, 30, 30, GD);          // MUESTRO FLECHA EN POSICIÓN INDICADA PARA JUGADOR 1
              LCD_Bitmap(x, 180, 30, 30, GD);         // MUESTRO FLECHA EN POSICIÓN INDICADA PARA JUGADOR 2
              x = x + 30;                             // AVANZO LA FLECHA A SIGUIENTE POSICIÓN
              delay(velocidad);                       // ESPERAR PARA MOSTRAR LA SIGUIENTE FLECHA

              q1 = digitalRead(D1);
              q2 = digitalRead(D2);

              if (q1 == HIGH && x <= 260)
              {
                estado1 = 1;
              }

              if (q1 == LOW && x > 260)
              {
                estado1 = 0;
              }

              if (q2 == LOW && x <= 260)
              {
                estado2 = 1;
              }

              if (q2 == LOW && x > 260)
              {
                estado2 = 0;
              }
            } //  ==== Cierra el FOR =====
            
            q1 = digitalRead(D1);
            q2 = digitalRead(D2);

            if (q1 == HIGH && estado1 == 0)
            {
              cont1++;
              LCD_Bitmap(290, 60, 30, 30, DD);           // MUESTRO FLECHA PARA INDICADORA DE PRESIONAR
              beep(nota_1,500);
            }

            if (q2 == HIGH && estado2 == 0)
            {
              cont2++;
              LCD_Bitmap(290, 180, 30, 30, DD);         // MUESTRO FLECHA PARA INDICADORA DE PRESIONAR
              beep(nota_2,500);
            }

            q1 = LOW;  // Reiniciamos la lectura digital
            q2 = LOW;  // Reiniciamos la lectura digital
            break;

          case 4:   // MUESTRO FLECHA derechaaaaa
          
            for (x = 11; x <= 290; x++) {             // derecha MOVER LA FLECHA HASTA QUE LLEGUE A POSICIÓN INDICADA
              FillRect(x - 30, 90, 30, 30, 0x00);     // OCULTO FLECHA ANTES DESPLEGADA PARA JUGADOR 1
              FillRect(x - 30, 210, 30, 30, 0x00);    // OCULTO FLECHA ANTES DESPLEGADA PARA JUGADOR 2
              LCD_Bitmap(x, 90, 30, 30, GR);          // MUESTRO FLECHA EN POSICIÓN INDICADA PARA JUGADOR 1
              LCD_Bitmap(x, 210, 30, 30, GR);         // MUESTRO FLECHA EN POSICIÓN INDICADA PARA JUGADOR 2
              x = x + 30;                             // AVANZO LA FLECHA A SIGUIENTE POSICIÓN
              delay(velocidad);                       // ESPERAR PARA MOSTRAR LA SIGUIENTE FLECHA

              q1 = digitalRead(R1);
              q2 = digitalRead(R2);

              if (q1 == HIGH && x <= 260)
              {
                estado1 = 1;
              }

              if (q1 == LOW && x > 260)
              {
                estado1 = 0;
              }

              if (q2 == LOW && x <= 260)
              {
                estado2 = 1;
              }

              if (q2 == LOW && x > 260)
              {
                estado2 = 0;
              }
            } //  ==== Cierra el FOR =====
            
            q1 = digitalRead(R1);
            q2 = digitalRead(R2);

            if (q1 == HIGH && estado1 == 0)
            {
              cont1++;
              LCD_Bitmap(290, 90, 30, 30, DR);           // MUESTRO FLECHA PARA INDICADORA DE PRESIONAR
              beep(nota_1,500);
            }

            if (q2 == HIGH && estado2 == 0)
            {
              cont2++;
              LCD_Bitmap(290, 210, 30, 30, DR);         // MUESTRO FLECHA PARA INDICADORA DE PRESIONAR
              beep(nota_2,500);
            }

            q1 = LOW;  // Reiniciamos la lectura digital
            q2 = LOW;  // Reiniciamos la lectura digital
            break;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      }
      z = 5;
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  delay(1000);
  if (z == 5) {                    // OPCION PARA MOSTRAR AL GANADOR

    while (z == 5) {
      LCD_Clear(0xFFFF);                                      // PINTO PANTALA DE BLANCO

      if (cont1 > cont2) {                                      // SI GANÓ JUGADOR 1
        String text14 = "JUGADOR 1";                          // PALABRA A IMPRIMIR
        String text15 = "GANA";                               // PALABRA A IMPRIMIR
        const char* marca1 = keg[cont1];
        const char* marca2 = keg[cont2];
        LCD_Print(text15, 130, 103, 2,  0xF800, 0xFFFF);      // IMPRIMO PALABRA
        LCD_Print(text14, 95, 133, 2,  0xF800, 0xFFFF);       // IMPRIMO PALABRA
        LCD_Print(marca1, 125, 163, 2,  0xF800, 0xFFFF);       // IMPRIMO PALABRA
        LCD_Print(marca2, 165, 163, 2,  0xF800, 0xFFFF);       // IMPRIMO PALABRA
        delay(1000);                                          // ESPERA DE 7 SEGUNDOS
      }

      if (cont2 > cont1) {                                      // SI GANÓ JUGADOR 2
        String text17 = "JUGADOR 2";                          // PALABRA A IMPRIMIR
        String text16 = "GANO";                               // PALABRA A IMPRIMIR
        const char* marca1 = keg[cont1];
        const char* marca2 = keg[cont2];
        LCD_Print(text16, 130, 103, 2,  0xF800, 0xFFFF);      // IMPRIMO PALABRA
        LCD_Print(text17, 95, 133, 2,  0xF800, 0xFFFF);       // IMPRIMO PALABRA
        LCD_Print(marca1, 125, 163, 2,  0xF800, 0xFFFF);       // IMPRIMO PALABRA
        LCD_Print(marca2, 165, 163, 2,  0xF800, 0xFFFF);       // IMPRIMO PALABRA
        delay(1000);                                          // ESPERA DE 7 SEGUNDOS
      }

      if (cont1 == cont2) {                                     // SI QUEDARON EMPATES
        
        String text18 = "EMPATE";                             // PALABRA A IMPRIMIR
        const char* marca1 = keg[cont1];
        const char* marca2 = keg[cont2];
        
        LCD_Print(text18, 110, 113, 2,  0xF800, 0xFFFF);      // IMPRIMO PALABRA
        LCD_Print(marca1, 125, 163, 2,  0xF800, 0xFFFF);       // IMPRIMO PALABRA
        LCD_Print(marca2, 165, 163, 2,  0xF800, 0xFFFF);       // IMPRIMO PALABRA
        
        delay(1000);                                          // ESPERA DE 7 SEGUNDOS
      }

      z = 6;                       // VARIABLE PARA EMPEZAR A JUGAR OTRA VEZ
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  String text18 = "PRESIONE UP PARA JUGAR OTRA VEZ";      // PALABRA A MOSTRAR
  LCD_Print(text18, 20, 218, 1,  0xF800, 0xFFFF);          // IMPRIMO PALABRA
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  while (z == 6)                  // FUNCION PARA REINICIAR JUEGO
  {
    uint8_t reinicio = LOW;       // VARIABLE PARA LECTURA DE BOTON
    uint8_t q = 0;
    
    while (z == 6) 
    {
      reinicio = digitalRead(U1);         // CHEQUEO SI EL JUGADOR UNO PRESIONA BOTON PARA REINICIAR
      
      if (reinicio == HIGH && q == 0)     // SI EL JUGADOR PRESIONÓ EL BOTON REGRESÓ TODAS LAS VARIABLES
      {
        velocidad = 0;                    // VELOCIDAD ESCOGIDA
        cont1 = 0;                        // PUNTOS DE JUGADOR 1
        cont2 = 0;                        // PUNTOS DE JUGADOR 2
        q = 1;
      }

      if (reinicio == LOW && q == 1)
      {
        q = 0;
        z = 1;                            // FOTOD DE INICIO Y CARGAR MEMORIA
      }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FillRect(0 , 0, 320, 240, 0xFFFF);    // PINTO PANTALLA DE BLANCO 
    delay(500);                           // ESPERO MEDIO SEGUNDO 
    goto inicio;                          // VUELVO AL INICIO DEL JUEGO
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y + i, w, c);
  }
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}

void beep(int note, int duration)
{
  tone(buzzer, note, duration/2);
  delay(duration/2);
  noTone(buzzer);
  delay(duration/2 + 20);  
}
