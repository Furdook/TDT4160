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

    // GPIO_PORT_DOUTTGL??? - 24
    add r1, r0, r1  // clear - 0x40006000 + ( 36 * 4 ) + 20
    add r2, r0, r2  // set   - 0x40006000 + ( 36 * 4 ) + 16
    add r3, r0, =GPIO_PORT_DIN  // 0x40006000 + ( 36 * 4 ) + 28 (could be on r0 as it's no longer in use) 
    /* // Kanskje dette i stedet?
    ldr r3, =GPIO_PORT_DIN
    add r3, r0, r3 
    */

    // << is the same as LSL 
    ldr r4, =1 << LED_PIN    // 2 
    ldr r5, =1 << BUTTON_PIN // 9

/*Loop:

    beq On
Off:

    B Loop
On:

    B Loop
*/
Loop:
    // turn off per loop
    // turn on if pressed
    B loop

NOP // Behold denne på bunnen av fila 

/* 
ldr - load
mov - move
str - store
add - addition
sub - substract
mul - multiply
lsl - left shift
orr - or
and - and
cmp - compare
beq - jump to
bgt - greater than
blt - lower than
*/