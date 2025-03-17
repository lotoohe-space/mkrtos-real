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
#include "ram_disk.h"
#include <stdio.h>
/* Definitions of physical drive number for each drive */
#define DEV_RAM 0 /* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC 1 /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB 2 /* Example: Map USB MSD to physical drive 2 */

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
	case DEV_RAM:
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
	int result;

	switch (pdrv)
	{
	case DEV_RAM:
		result = 0;
		stat = RES_OK;
		// translate the reslut code here
		return stat;
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
	DRESULT res;

	switch (pdrv)
	{
	case DEV_RAM:
		// translate the reslut code here
		if (ram_disk_read(buff, sector, count) < 0) {
			return RES_ERROR;
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
	int result;

	switch (pdrv)
	{
	case DEV_RAM:
		// translate the arguments here

		if (ram_disk_write((uint8_t *)buff, sector, count) < 0) {
			return RES_ERROR;
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
	DRESULT res;
	int result;

	switch (pdrv)
	{
	case DEV_RAM:
		switch (cmd)
		{					   // fatfs内核使用cmd调用
		case GET_SECTOR_COUNT: // sector count, 传入sect_cnt
			*(DWORD *)buff = ram_disk_sector_nr();
			return RES_OK;
		case GET_SECTOR_SIZE: // sector size, 传入block size(SD),单位bytes
			*(DWORD *)buff = 512;
			return RES_OK;
		case GET_BLOCK_SIZE:	// block size, 由上文可得，对于SD2.0卡最大8192，最小 1
			*(DWORD *)buff = 1; // 单位为 sector(FatFs)
			return RES_OK;
		case CTRL_SYNC: // 同步命令，貌似FatFs内核用来判断写操作是否完成
			return RES_OK;
		}
	default:
		printf("No device %d.\n", pdrv);
		break;
	}
	return RES_PARERR; // 默认返回参数错误
}
