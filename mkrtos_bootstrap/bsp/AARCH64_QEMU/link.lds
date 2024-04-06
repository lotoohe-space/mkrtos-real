ENTRY(_start);
SECTIONS
{
 . = 0x40000000 + 0x1000;
 _text = .;
 .text :
 {
  *(.text)
 }
 _etext = .;
 _rodata = .;
 .rodata :
 {
  *(.rodata)
 }
 _erodata = .;
 _data = .;
 .data :
 {
  *(.data)
  . = ALIGN(0x8);
  _pre_cpu_data_start = .;
  KEEP (*(.data.per_cpu))
  _pre_cpu_data_end = .;
 }
 _edata = .;
 . = ALIGN(0x8);
 _bss = .;
 bss_begin = .;
 .bss :
 {
  *(.bss*)
 }
 bss_end = .;
 _ebss = .;
 . = ALIGN(0x8);
 .cpio : {
  cpio_start = .;
  *(.cpio*)
  cpio_end = .;
 }
}
