.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO
.include "sys-tick_constants.s" // Register-adresser og konstanter for SysTick

.text
	.global Start
	
Start:

	ldr r0, =FREQUENCY / 10               // clock frquency 14 000 000 devided to count tenth of seconds
	ldr r1, =SYSTICK_BASE + SYSTICK_LOAD  // load loc
	str r0, [r1]                          // store frequency in load

	mov r0, #0                            // 0
	ldr r1, =SYSTICK_BASE + SYSTICK_VAL   // val loc
	str r0, [r1]                          // store zero in val

	ldr r0, =0b110                        // clcksource + tickint, not enabling before button press
	ldr r1, =SYSTICK_BASE + SYSTICK_CTRL  // ctrl loc
	str r0, [r1]                          // store

	ldr r0, =GPIO_BASE + GPIO_EXTIPSELH   // base loc
	ldr r1, [r0]                          // mem address
	and r1, 0b1111 << 4                   // left shift 4, and to loc
	orr r1, 0b0001 << 4                   // left shift 4, or with prior
	str r1, [r0]                          // store on base address

	ldr r0, =GPIO_BASE+GPIO_EXTIFALL      // load address
	ldr r1, [r0]                          // load base address on r1
	orr r1, 1 << BUTTON_PIN               // or 1 lsl by 9 with r1 on r1,
	str r1, [r0]                          // store on base address

	ldr r0, =GPIO_BASE + GPIO_IEN         // load address
	ldr r1, [r0]                          // load base address on r1
	orr r1, 1 << BUTTON_PIN               // or r1 with 1 lsl by 9
	str r1, [r0]                          // store on base address

	B Loop                                // branch to loop

.global GPIO_ODD_IRQHandler
.thumb_func
GPIO_ODD_IRQHandler:
	ldr r0, =SYSTICK_BASE + SYSTICK_CTRL  // load address
	ldr r1, [r0]                          // load base address on r1
	eor r1, 0b001                         // xor enable bit on ctrl address
	str r1, [r0]                          // store on base address

	ldr r0, =GPIO_BASE + GPIO_IFC         // load interupt reset loc
	mov r1, #1 << BUTTON_PIN              // left shift 1 by 9 to get correct pin
	str r1, [r0]                          // store on base address

	bx lr                                 // branch to link reg

.global SysTick_Handler
.thumb_func
SysTick_Handler:

	ldr r0, =tenths                       // load golbal tenths variable
	ldr r1, [r0]                          // load r1 to store on tenth's address
	add r1, #1                            // Add value 1 to tenths variable
	cmp r1, #10                           // Check if value has reached 10
	bne Tenths                            // Branch to Tenths if not 10
	ldr r1, =0                            // Reset 'tenths' to zero if r1 = 10

	ldr r6, =GPIO_BASE + (PORT_SIZE * LED_PORT) + GPIO_PORT_DOUTTGL
	mov r7, #1 << LED_PIN
	str r7, [r6]

	ldr r2, =seconds                      // load global seoncds var
	ldr r3, [r2]                          // load to store on seconds' address
	add r3, #1                            // add 1
	cmp r3, #60                           // compare to 60, 60 seconds
	bne Seconds                           // branch to Seconds if not equal
	ldr r3, =0                            // reset seconds to 0 per minute 

	ldr r4, =minutes                      // load global minutes var
	ldr r5, [r4]                          // load to store on address
	add r5, #1                            // add 1
	str r5, [r4]                          // No need to branch off or compare, not counting hours, I hope

Seconds:
	str r3, [r2]                          // store seconds value in seconds var

Tenths:
	str r1, [r0]                          // store tenth value in tenth var
	bx lr                                 // return to link reg

Loop:
	B Loop                                // infinite loop

NOP
