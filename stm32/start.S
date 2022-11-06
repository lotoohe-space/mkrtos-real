  .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

  .section .text._start
  .weak _start
  .type _start, %function
_start:

blx main

bx lr
