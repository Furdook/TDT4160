.thumb
.syntax unified

.include "gpio_constants.s"

.text
	.global Start
	
Start:
    ldr r0, =GPIO_BASE + (PORT_SIZE * LED_PORT)  // 0x40006000 + ( 36 * 4 )
    ldr r4, =GPIO_BASE + PORT_SIZE               // 0x40006000 + 36

    ldr r1, =GPIO_PORT_DOUTCLR  // 20
    ldr r2, =GPIO_PORT_DOUTSET  // 16
    ldr r3, =GPIO_PORT_DIN      // 38

    add r1, r0, r1  // Clear
    add r2, r0, r2 	// Set
    add r3, r4, r3  // Data input

	mov r0, #1 << LED_PIN     // LED_PIN = 2,    so we get 0b100
	mov r4, #1 << BUTTON_PIN  // BUTTON_PIN = 9, so we get 0b100000000

Loop:
	ldr r5, [r3]    // load data input value on r6
    and r5, r4, r5  // and to get intended data input
    cmp r5, r4		// compare to determain whether pushed or not
    beq On          // jump to On if cmp true

    str r0, [r2]    // str set on LED, turns off
    B Loop          // restarts loops

On:
    str r0, [r1]    // str clear on LED, turns on
    B Loop          // restarts loop

NOP