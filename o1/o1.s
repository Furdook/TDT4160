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
    ldr r3, =GPIO_PORT_DIN

    add r1, r0, r1  // clear      - r0 + 20
    add r2, r0, r2  // set        - r0 + 16
    add r3, r0, r3  // data input - r0 + 28

    // << is the same as LSL
    ldr r4, =1 << LED_PIN     // 2
    ldr r5, =1 << BUTTON_PIN  // 9

Loop:
    ldr r6, [r3]  // last data input på r6?
    and r3, r5    // kok
    cmp r3, r5
    beq On        // jump to On

    str r4, [r2]  // turn off?
    B Loop

On:
    str r4, [r1]
    B Loop

NOP // Behold denne på bunnen av fila