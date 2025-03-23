
 cd /mnt
 tcc -nostdinc -nostdlib -c /bin/reloc.c -o reloc.o
 tcc -nostdinc -nostdlib -c /bin/main.c -o main.o
 tcc -nostdinc -nostdlib -c /bin/cons.c -o cons.o
 as -mthumb -mcpu=cortex-m3 /bin/start.S -o start.o 
 as -mthumb -mcpu=cortex-m3 /bin/syscall.S -o syscall.o 
 ld start.o main.o -T/bin/link.lds -o a.out --section-start .text=0x8000000
 ld start.o main.o cons.o syscall.o -T/bin/link_pie.lds -o a.out --section-start .text=0x20143800
 ld start.o main.o -T/bin/link_pie.lds -o a.out --section-start .text=0x20143400 -L/bin -static -lsys
 ld reloc.o start.o main.o -T/bin/link.lds -o b.out -r --gc-sections
 objcopy -O binary -S a.out a.bin
    59: 201438ec     0 NOTYPE  GLOBAL DEFAULT    5 __stack_end__
 objdump -b binary -D -m armv7e-m -M force-thumb --adjust-vma=0x8000000 a.bin

127

 tcc -nostdinc -nostdlib -c main.c -o main.o
 as -mthumb -mcpu=cortex-m3 start.S -o start.o 
 ld start.o main.o -T/bin/link.lds -o a.out --section-start .text=0x8000000

 echo "" | xxd -r -p > a.bin
