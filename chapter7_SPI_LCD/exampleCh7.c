
#include<stm32f30x_usart.h>
#include<stm32f30x_gpio.h>
#include<stm32f30x_rcc.h>
#include<stm32f30x_spi.h>
#include<stm32f30x.h>

#define MADCTLGRAPHICS 0x6
#define MADCTLBMP      0x2

#define ST7735_width 128
#define ST7735_height 160

#define LOW 0
#define HIGH 1

#define LCD_C LOW
#define LCD_D HIGH

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_MADCTL 0x36
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E
#define ST7735_COLMOD 0x3A

#define MADVAL(x) (((x)<<5)|8)
static uint8_t madctlcurrent = MADVAL(MADCTLGRAPHICS);

void ST7735_setAddrWindow (uint16_t x0, uint16_t y0, uint16_t x1,
uint16_t y1, uint8_t madctl) {
    madctl = MADVAL(madctl);
    if (madctl != madctlcurrent) {
        ST7735_writeCmd(ST7735_MADCTL);
        LcdWrite(LCD_D, &madctl, 1);
        madctlcurrent = madctl;
    }
    ST7735_writeCmd(ST7735_CASET);
    LcdWrite16(LCD_D, &x0, 1);
    LcdWrite16(LCD_D, &x1, 1);

    ST7735_writeCmd(ST7735_RASET);
    LcdWrite16(LCD_D, &x0, 1);
    LcdWrite16(LCD_D, &x1, 1);

    ST7735_writeCmd(ST7735_RAMWR);
}

void ST7735_pushColor(uint16_t *color , int cnt) {
    LcdWrite16(LCD_D, color, cnt);
}

void ST7735_init() {
    GPIO_InitTypeDef GPIO_InitStructure;
    const struct ST7735_cmdBuf *cmd;

    //set up pins 
    // set cs, reset low

    GPIO_WriteBit(LCD_PORT, GPIO_PIN_SCE, HIGH);
    GPIO_WriteBit(LCD_PORT, GPIO_PIN_RST, HIGH);
    Delay(10);
    GPIO_WriteBit(LCD_PORT, GPIO_PIN_SCE, LOW);
    Delay(10);
    GPIO_WriteBit(LCD_PORT, GPIO_PIN_RST, HIGH);
    Delay(10);

    // Send initalization commands to ST7735

    for (cmd = initializers; cmd->command; cmd++) {
        LcdWrite(LCD_C, &(cmd->command), 1);
        if (cmd->len) 
            LcdWrite(LCD_D, cmd->data, cmd->len);
        if (cmd->delay)
            Delay(cmd->delay);
    }
}

void ST7735_backLight(uint8_t on) {
    if (on) 
        GPIO_WriteBit(LCD_PORT_BKL,GPIO_PIN_BKL, LOW);
    else
        GPIO_WriteBit(LCD_PORT_BKL,GPIO_PIN_BKL, HIGH);
}

void fillScreen (uint16_t color){
    uint8_t x,y;
    ST7735_setAddrWindow(0,0,ST7735_width-1,ST7735_height-1,MADCTLGRAPHICS);
    for (x=0; x<ST7735_width;x++) {
        for (y=0; y<ST7735_height;y++) {
            ST7735_pushColor(&color, 1);
        }
    }
}

static void LcdWrite(char dc, const char *data, int nbytes) {
    GPIO_WriteBit(LCD_PORT, GPIO_PIN_DC, dc); // dc 1-data,0-control
    GPIO_ResetBits(LCD_PORT, GPIO_PIN_SCE);
    spiReadWrite(SPILCD,0,data,nbytes,LCDSPEED);
    GPIO_SetBits(LCD_PORT, GPIO_PIN_SCE);
}

static void LcdWrite16(char dc, const uint16_t *data, int cnt) {
    GPIO_WriteBit(LCD_PORT, GPIO_PIN_DC, dc); // dc 1-data,0-control
    GPIO_ResetBits(LCD_PORT, GPIO_PIN_SCE);
    spiReadWrite16(SPILCD,0,data,cnt,LCDSPEED);
    GPIO_SetBits(LCD_PORT, GPIO_PIN_SCE);
}

static void ST7735_writeCmd(uint8_t c) {
    LcdWrite(LCD_C, &c, 1)
}

struct ST7735_cmdBuf {
    uint8_t command; // ST7735 command byte
    uint8_t delay; // ms delay after
    uint8_t len; // length of the parameter data
    uint8_t data[16]; // parameter data
}

static const struct ST7735_cmdBuf initializers[] = {
    // SWRESET Software reset
    { 0x01, 150, 0, 0},
    // SLPOUT Leave Sleep mode
    { 0x11, 150, 0, 0},
    // FRMCTR1, FRMCTR2 Frame rate configuration -- Normal Mode, idle
    // frame rate = fosc / (1x2+40) * (LINE+2C+2D)
    { 0xB1, 0, 3, { 0x01, 0x2C, 0x2D}},
    { 0xB2, 0, 3, { 0x01, 0x2C, 0x2D}},
    // FRMCTR3 Frame Rate Configuration -- partial mode
    { 0xB3, 0, 6, { 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D}},
    // INVCTR Display Inversion (no inversion)
    { 0xB4, 0, 1, {0x07}}
};