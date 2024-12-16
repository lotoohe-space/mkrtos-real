.section .text
.balign 4
.global nes_rom
nes_rom:
.incbin "/Users/zhangzheng/mkrtos-real/mkrtos_user/user/app/nes_simulator/nes_rom/超级玛丽.nes"
.global nes_rom_end
nes_rom_end:
.long 0


