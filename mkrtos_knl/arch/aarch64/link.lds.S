ENTRY(_start);

SECTIONS
{
	/*
	 *
	 *
	 * 这里“.”表示location counter，当前位置
	 */
	. = CONFIG_KNL_DATA_ADDR+0x1000;
	kernel_start = .;
	/*
	 * 这里是第一个段text.boot，起始地址就是0x80000
	 * 这个段存放了benos的第一条指令
	 */
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
	/*
	 * text代码段
	 */
	_text = .;
	.text :
	{
		*(.text)
		. = ALIGN(8);
		_mkrtos_init_start = .;
		KEEP (*(SORT(.mkrtos.init.*)))
		_mkrtos_init_end = .;
	}
	.preinit_array     :
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

	. = ALIGN(4096);
	_buddy_data_start = .;
	.buddy :
	{
		*(.buddy*) 
		. += 0x1000000;
	}
	_buddy_data_end = .;


}
