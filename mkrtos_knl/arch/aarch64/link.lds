ENTRY(_start);
SECTIONS
{
 . = 0x40000000 +0x1000000;
 kernel_start = .;
 _text_boot = .;
 .text.boot :
 {
  *(.text.boot)
 }
 . = ALIGN(4096);
 _etext_boot = .;
 _data_boot = .;
 .data.boot :
 {
  *(.data.boot)
 }
 . = ALIGN(4096);
 _edata_boot = .;
 . = _edata_boot;
 _text = .;
 .text :
 {
  *(.text)
  . = ALIGN(8);
  _mkrtos_init_start = .;
  KEEP (*(SORT(.mkrtos.init.*)))
  _mkrtos_init_end = .;
 }
 .preinit_array :
 {
  PROVIDE_HIDDEN (__preinit_array_start = .);
  KEEP (*(.preinit_array*))
  PROVIDE_HIDDEN (__preinit_array_end = .);
 }
 .init_array :
 {
  PROVIDE_HIDDEN (__init_array_start = .);
  KEEP (*(SORT(.init_array.*)))
  KEEP (*(.init_array*))
  PROVIDE_HIDDEN (__init_array_end = .);
 }
 .fini_array :
 {
  PROVIDE_HIDDEN (__fini_array_start = .);
  KEEP (*(SORT(.fini_array.*)))
  KEEP (*(.fini_array*))
  PROVIDE_HIDDEN (__fini_array_end = .);
 }
 . = ALIGN(4096);
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
 . = ALIGN(4096);
 _buddy_data_start = .;
}
