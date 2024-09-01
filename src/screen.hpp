#ifndef SCREEN_H
#define SCREEN_H
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>


#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 0
#define R2_PIN 14
#define G2_PIN 2
#define B2_PIN 13
#define A_PIN 23
#define B_PIN 19
#define C_PIN 18
#define D_PIN 12
#define E_PIN 21 // required for 1/32 scan panels, like 64x64px. Any available pin would do, i.e. IO32
#define LAT_PIN 4
#define OE_PIN 17
#define CLK_PIN 16


#define PANEL_RES_X 64    // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 64     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another

HUB75_I2S_CFG::i2s_pins _pins={R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};

MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);
uint16_t myTEAL = dma_display->color565(0, 100, 100);

#endif