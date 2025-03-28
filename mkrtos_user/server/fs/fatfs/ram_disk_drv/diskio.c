/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"		/* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include <stdio.h>
#include "blk_drv_cli.h"
#include "ns_cli.h"
#include "u_factory.h"
#include "u_vmam.h"
#include "u_hd_man.h"
#include "u_sleep.h"
#include <string.h>
#include "u_share_mem.h"
/* Definitions of physical drive number for each drive */
#define DEV_MK_BLOCK 0 /* Example: Map Ramdisk to physical drive 0 */

static obj_handler_t dev_hd;
static obj_handler_t shm_hd;
static addr_t dev_shm_mem;
static addr_t dev_shm_size;
static blk_drv_info_t blk_info;
static int ram_block_inited;
int disk_set_dev_path(int pdrv, const char *dev)
{
	int ret;
	switch (DEV_MK_BLOCK)
	{
	case DEV_MK_BLOCK:
	{
		int try_cn = 0;
	again:
		ret = ns_query_svr(dev, &dev_hd);
		if (ret < 0)
		{
			try_cn++;
			if (try_cn > 100)
			{
				return ret;
			}
			u_sleep_ms(5);
			goto again;
		}
		ret = blk_drv_cli_info(dev_hd, &blk_info);
		if (ret < 0)
		{
			return ret;
		}
	}
	break;
	default:
		return -1;
	}
	return 0;
}
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv)
	{
	case DEV_MK_BLOCK:
		result = 0;

		// translate the reslut code here
		stat = RES_OK;
		return stat;
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
	BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;

	switch (pdrv)
	{
	case DEV_MK_BLOCK:
	{
		if (ram_block_inited)
		{
			return RES_OK;
		}
		msg_tag_t tag;

		shm_hd = handler_alloc();
		if (shm_hd == HANDLER_INVALID)
		{
			printf("handler alloc failed.\n");
			return RES_ERROR;
		}
		tag = u_facotry_create_share_mem(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, shm_hd),
									   SHARE_MEM_CNT_BUDDY_CNT, blk_info.blk_size);
		if (msg_tag_get_val(tag) < 0)
		{
			handler_free(shm_hd);
			printf("share mem create failed.\n");
			return RES_ERROR;
		}
		tag  = u_share_mem_map(shm_hd, vma_addr_create(VPAGE_PROT_RW, VMA_ADDR_RESV, 0), &dev_shm_mem, &dev_shm_size);
		if (msg_tag_get_val(tag) < 0)
		{
			handler_del_umap(shm_hd);
			printf("share mem map failed.\n");
			return RES_ERROR;
		}
		stat = RES_OK;
		ram_block_inited = 1;
		// translate the reslut code here
		return stat;
	}
	}
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
	BYTE pdrv,	  /* Physical drive nmuber to identify the drive */
	BYTE *buff,	  /* Data buffer to store read data */
	LBA_t sector, /* Start sector in LBA */
	UINT count	  /* Number of sectors to read */
)
{

	switch (pdrv)
	{
	case DEV_MK_BLOCK:
		// translate the reslut code here
		for (umword_t i = sector; i < sector + count; i++)
		{
			if (blk_drv_cli_read(dev_hd, shm_hd, blk_info.blk_size, i) < 0)
			{
				return RES_ERROR;
			}
			memcpy(buff + (i - sector) * blk_info.blk_size, (void *)dev_shm_mem, blk_info.blk_size);
		}
		return 0;
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
	BYTE pdrv,		  /* Physical drive nmuber to identify the drive */
	const BYTE *buff, /* Data to be written */
	LBA_t sector,	  /* Start sector in LBA */
	UINT count		  /* Number of sectors to write */
)
{

	switch (pdrv)
	{
	case DEV_MK_BLOCK:
		// translate the arguments here
		for (umword_t i = sector; i < sector + count; i++)
		{
			memcpy((void *)dev_shm_mem, buff + (i - sector) * blk_info.blk_size, blk_info.blk_size);

			if (blk_drv_cli_write(dev_hd, shm_hd, blk_info.blk_size, i) < 0)
			{
				return RES_ERROR;
			}
		}
		return 0;
	}

	return RES_PARERR;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl(
	BYTE pdrv, /* Physical drive nmuber (0..) */
	BYTE cmd,  /* Control code */
	void *buff /* Buffer to send/receive control data */
)
{

	switch (pdrv)
	{
	case DEV_MK_BLOCK:
	{
		switch (cmd)
		{					   // fatfs内核使用cmd调用
		case GET_SECTOR_COUNT: // sector count, 传入sect_cnt
			*(DWORD *)buff = blk_info.blk_nr;
			return RES_OK;
		case GET_SECTOR_SIZE: // sector size, 传入block size(SD),单位bytes
			*(WORD *)buff = blk_info.blk_size;
			return RES_OK;
		case GET_BLOCK_SIZE:	// block size, 由上文可得，对于SD2.0卡最大8192，最小 1
			*(DWORD *)buff = 1; // 单位为 sector(FatFs)
			return RES_OK;
		case CTRL_SYNC: // 同步命令，貌似FatFs内核用来判断写操作是否完成
			return RES_OK;
		}
	}
	default:
		printf("No device %d.\n", pdrv);
		break;
	}
	return RES_PARERR; // 默认返回参数错误
}
