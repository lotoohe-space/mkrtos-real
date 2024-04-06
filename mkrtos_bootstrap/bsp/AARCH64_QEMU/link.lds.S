ENTRY(_start);

SECTIONS
{
	/*
	 * CONFIG_KNL_DATA_ADDR
	 *
	 * 这里“.”表示location counter，当前位置
	 */
	. = CONFIG_KNL_DATA_ADDR + 0x1000;
	/*
	 * text代码段
	 */
	_text = .;
	.text :
	{
		*(.text)
	}
	_etext = .;

	/*
	 * 只读数据段
	 */
	
	_rodata = .;
	.rodata :
	{
		*(.rodata)
	}
	_erodata = .;


	/*
	 * 数据段
	 */
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

		/*
	 * bss段
	 *
	 * ALIGN(8)表示8个字节对齐
	 * bss_begin的起始地址以8字节对齐
	 */
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
