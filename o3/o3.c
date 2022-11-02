#include "o3.h"
#include "gpio.h"
#include "systick.h"

struct gpio_port_map_t {  // from compendium
    volatile word CTRL;
    volatile word MODEL;
    volatile word MODEH;
    volatile word DOUT;
    volatile word DOUTSET;
    volatile word DOUTCLR;
    volatile word DOUTTGL;
    volatile word DIN;
    volatile word PINLOCKN;
};

volatile struct gpio_map_t {  // from compendium
    struct gpio_port_map_t ports[6];
    volatile word unused_space[10];
    volatile word EXTIPSELL;
    volatile word EXTIPSELH;
    volatile word EXTIRISE;
    volatile word EXTIFALL;
    volatile word IEN;
    volatile word IF;
    volatile word IFS;
    volatile word IFC;
    volatile word ROUTE;
    volatile word INSENSE;
    volatile word LOCK;
    volatile word CTRL;
    volatile word CMD;
    volatile word EM4WUEN;
    volatile word EM4WUPOL;
    volatile word EM4WUCAUSE;
} *GPIO = (struct gpio_map_t*)GPIO_BASE;  // GPIO pointer

volatile struct systick_t {
    word CTRL;
    word LOAD;
    word VAL;
    word CALIB;
} *SYSTICK = (struct systick_t*)SYSTICK_BASE;  // SYSTICK pointer

static int state = 0;
static int h, m, s;
static char str[8] = "0000000\0";

void int_to_string(char *timestamp, unsigned int offset, int i) {  // premade function
    if (i > 99) {
        timestamp[offset]   = '9';
        timestamp[offset+1] = '9';
        return;
    }

    while (i > 0) {
        if (i >= 10) {
            i -= 10;
            timestamp[offset]++;
        }
        else {
            timestamp[offset+1] = '0' + i;
            i = 0;
        }
    }
}

void time_to_string(char *timestamp, int h, int m, int s) {  // premade function
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

void toggleLED(int b) {
    if (b) {
        GPIO->ports[GPIO_PORT_E].DOUTSET = 1 << 2;
        return;
    }
    GPIO->ports[GPIO_PORT_E].DOUTCLR = 1 << 2;

}

void GPIO_ODD_IRQHandler() {
    if (state == 0) {
        s++;
        if (s >= 60)
           s = 0;
    }
    if (state == 1) {
        m++;
        if (m >= 60)
          m = 0;
    }
    if (state == 2) {
        h++;
    }
    GPIO->IFC = 1 << 9;  // clear interupt flag
}

void GPIO_EVEN_IRQHandler() {
    state++;
    if (state == 3) {
        SYSTICK->VAL = SYSTICK->LOAD;
        SYSTICK->CTRL |= SysTick_CTRL_ENABLE_Msk;  // start clock
    }
    if (state == 4) {
        state = 0;
        toggleLED(0);
    }
    GPIO->IFC = 1 << 10;  // clear interupt flag on PB1: 10
}

void SysTick_Handler() {
    if (state == 3) {     // if set to state 3 starts counting down
        if (s <= 0) {
            if (m <= 0) {
                if (h <= 0) {
                    state = 4;    // alarm state
                    SYSTICK->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);  // bitwise not &= call on msk, to stop clock
                    toggleLED(1); // left shift by LED_PIN
                }
                h--;
                m = 60;
            }
            m--;
            s = 60;
        }
        s--;
    }
    m = 0;
    s = 0;
}

void setFlag(volatile word *w, int i, word flag) {
    *w &= ~(0b1111 << (i * 4));
    *w |= flag << (i * 4);
}

int main() {
    init();
    setFlag(&GPIO->ports[GPIO_PORT_E].MODEL, 2, GPIO_MODE_OUTPUT);  // LED_PIN: 2
    setFlag(&GPIO->ports[GPIO_PORT_B].MODEH, 1, GPIO_MODE_INPUT);   // PB0 - 8: 9-8
    setFlag(&GPIO->ports[GPIO_PORT_B].MODEH, 2, GPIO_MODE_INPUT);   // PB1 - 8: 10-8
    setFlag(&GPIO->EXTIPSELH, 1, 0b0001);   // PB0 - 8
    setFlag(&GPIO->EXTIPSELH, 2, 0b0001);   // PB1 - 8

    GPIO->EXTIFALL |= 1 << 9;   // left shift by PB0
    GPIO->EXTIFALL |= 1 << 10;  // left shift by PB1
    GPIO->IEN |= 1 << 9;        // left shift by PB0
    GPIO->IEN |= 1 << 10;       // left shift by PB1

    SYSTICK->LOAD = FREQUENCY;
    SYSTICK->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;

    while (1) {
        time_to_string(str, h, m, s);
        lcd_write(str);
    }
    return 0;
}