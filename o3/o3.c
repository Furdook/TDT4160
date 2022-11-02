#include "o3.h"

struct gpio_port_map_t {
    word CTRL;
    word MODEL;
    word MODEH;
    word DOUT;
    word DOUTSET;
    word DOUTCLR;
    word DOUTTGL;
    word DIN;
    word PINLOCKN;
};

struct gpio_map_t {
    struct gpio_port_map_t ports[6];
    word unused_space[10];
    word EXTIPSELL;
    word EXTIPSELH;
    word EXTIRISE;
    word EXTIFALL;
    word IEN;
    word IF;
    word IFS;
    word IFC;
    word ROUTE;
    word INSENSE;
    word LOCK;
    word CTRL;
    word CMD;
    word EM4WUEN;
    word EM4WUPOL;
    word EM4WUCAUSE;
} *GPIO = (struct gpio_map_t*)0x40006000;

struct systick_t {
    word CTRL;
    word LOAD;
    word VAL;
    word CALIB;
} *SYSTICK = (struct systick_t*)0xE000E010;

static int state = 0;
static int time = 0;
static char str[8] = "0000000\0";

void int_to_string(char *timestamp, unsigned int offset, int i) {
    if (i > 99) {
        timestamp[offset]   = '9';
        timestamp[offset + 1] = '9';
        return;
    }

    while (i > 0) {
        if (i >= 10) {
            i -= 10;
            timestamp[offset]++;
        }
        else {
            timestamp[offset + 1] = '0' + i;
            i = 0;
        }
    }
}

void time_to_string(char *timestamp, int h, int m, int s) {
    timestamp[0] = '0';
    timestamp[1] = '0';
    timestamp[2] = '0';
    timestamp[3] = '0';
    timestamp[4] = '0';
    timestamp[5] = '0';
    timestamp[6] = '\0';

    int_to_string(timestamp, 0, h);
    int_to_string(timestamp, 2, m);
    int_to_string(timestamp, 4, s);
}

void GPIO_ODD_IRQHandler() {
    if (state == 0)
        time++;
    if (state == 1)
        time += 60;
    if (state == 2)
        time += 3600;

    GPIO->IFC = 1 << 9;                         // clear interupt flag
}

void GPIO_EVEN_IRQHandler() {
    state++;
    if (state == 3) {
        SYSTICK->VAL = SYSTICK->LOAD;
        SYSTICK->CTRL |= 0b001;                 // start clock
    }
    if (state == 4) {
        state = 0;
        GPIO->ports[4].DOUTCLR = 1 << 2;
    }
    GPIO->IFC = 1 << 10;                        // clear interupt flag on PB1: 10
}

void SysTick_Handler() {
    if (state == 3) {                           // if set to state 3 starts counting down
        if (time <= 0) {
            state = 4;                          // alarm state
            SYSTICK->CTRL &= ~(0b001);          // bitwise not &= call on msk, to stop clock
            GPIO->ports[4].DOUTSET = 1 << 2;    // left shift by LED_PIN
        }
        time--;
    }
}

void setFlag(word *w, int i, word flag) {
    *w &= ~(0b1111 << (i * 4));
    *w |= flag << (i * 4);
}

int main() {
    init();
    setFlag(&GPIO->ports[4].MODEL, 2, 0b0100);  // LED_PIN: 2, port 4 (E)
    setFlag(&GPIO->ports[1].MODEH, 1, 0b0001);  // PB0 - 8: 9-8, port 4 (B)
    setFlag(&GPIO->ports[1].MODEH, 2, 0b0001);  // PB1 - 8: 10-8
    setFlag(&GPIO->EXTIPSELH, 1, 0b0001);       // PB0 - 8
    setFlag(&GPIO->EXTIPSELH, 2, 0b0001);       // PB1 - 8

    GPIO->EXTIFALL |= 1 << 9;                   // left shift by PB0
    GPIO->EXTIFALL |= 1 << 10;                  // left shift by PB1
    GPIO->IEN |= 1 << 9;                        // left shift by PB0
    GPIO->IEN |= 1 << 10;                       // left shift by PB1

    SYSTICK->LOAD = 14000000;
    SYSTICK->CTRL = 0b100 | 0b010;

    while (1) {
        time_to_string(str, time / 3600, (time / 60) % 60, time % 60);
        lcd_write(str);
    }
    return 0;
}
/*
SYSTICK_BASE 0xE000E010
GPIO_BASE 0x40006000
FREQUENCY 14000000

LED_PIN 2
PB0_PIN 9
PB1_PIN 10

SysTick_CTRL_CLKSOURCE_Msk  0b100
SysTick_CTRL_TICKINT_Msk    0b010
SysTick_CTRL_ENABLE_Msk     0b001

GPIO_MODE_INPUT  0b0001
GPIO_MODE_OUTPUT 0b0100

GPIO_PORT_A 0
GPIO_PORT_B 1
GPIO_PORT_C 2
GPIO_PORT_D 3
GPIO_PORT_E 4
GPIO_PORT_F 5
*/