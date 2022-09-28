.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO

.text
	.global Start
	
Start:

    // Skriv din kode her...
    ldr r0, =GPIO_BASE + (PORT_SIZE * LED_PORT)  // 0x40006000 + ( 36 * 4 )
    ldr r1, =GPIO_PORT_DOUTCLR  // 20
    ldr r2, =GPIO_PORT_DOUTSET  // 16
    ldr r3, =GPIO_BASE + (PORT_SIZE * BUTTON_PORT) + GPIO_PORT_DIN  // Data Input

    add r1, r0, r1  // clear
    add r2, r0, r2 	// set

	mov r4, #1 << LED_PIN     //  LEDleft shift to 100
	mov r5, #1 << BUTTON_PIN  // left shift to 10000000

Loop:
	ldr r6, [r3]
    and r6, r5, r6
    cmp r6, r5		// compare to determain whether pushed or not
    beq On          // jump to On if cmp = True

    str r4, [r2]    // else turn off
    B Loop

On:
    str r4, [r1]
    B Loop

NOP // Behold denne på bunnen av fila