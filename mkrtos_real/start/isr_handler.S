.syntax unified
.cpu cortex-m3
.thumb

.global isr_handler
@ .section .text.isr_handler
.type isr_handler, %function
isr_handler:
	MRS R0, PSP
	B extern_intr
