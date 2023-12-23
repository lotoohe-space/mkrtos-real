/**
  ******************************************************************************
  * @file    LTDC/LTDC_AnimatedPictureFromSDCard/fatfs_drv.c
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   diskio interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 0 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "diskio.h"
#include "stm324x9i_eval_sdio_sd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/                
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
   * @brief  Initialize Disk Drive  
   * @param   drv : driver index
   * @retval DSTATUS : operation status
  */

DSTATUS disk_initialize ( BYTE drv )
{
  
  NVIC_InitTypeDef NVIC_InitStructure;  
  Stat = STA_NOINIT;
  
  if (drv == 0)
  {
    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    if( SD_Init() == 0)
    {
      Stat &= ~STA_NOINIT;
    }     
  }
  
  return Stat;
}

/**
   * @brief  Get Disk Status  
   * @param   drv : driver index
   * @retval DSTATUS : operation status
*/
DSTATUS disk_status ( BYTE drv	)
{  
  Stat = STA_NOINIT;
  
  if ((drv == 0) && (SD_GetStatus() == 0))
  {
    Stat &= ~STA_NOINIT;
  }
  
  return Stat;  
}

/**
   * @brief  Read Sector(s) 
   * @param   drv : driver index
   * @retval DSTATUS : operation status
  */
DRESULT disk_read (
                   BYTE drv,			  /* Physical drive number (0) */
                   BYTE *buff,			/* Pointer to the data buffer to store read data */
                   DWORD sector,		/* Start sector number (LBA) */
                   BYTE count			  /* Sector count (1..255) */
                     )
{
  
  SD_Error sdstatus = SD_OK;
 
  if (drv == 0)
  {
    SD_ReadMultiBlocks(buff, sector << 9, 512, count);
    
    /* Check if the Transfer is finished */
    sdstatus =  SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
    
    if (sdstatus == SD_OK)
    {
      return RES_OK;
    }
  }
  return RES_ERROR;
}
/**
   * @brief  write Sector(s) 
   * @param   drv : driver index
   * @retval DSTATUS : operation status
  */

#if _READONLY == 0
DRESULT disk_write (
                    BYTE drv,			    /* Physical drive number (0) */
                    const BYTE *buff,	/* Pointer to the data to be written */
                    DWORD sector,		  /* Start sector number (LBA) */
                    BYTE count			  /* Sector count (1..255) */
                      )
{
  
  SD_Error sdstatus = SD_OK;
  
  if (drv == 0)
  {
    SD_WriteMultiBlocks((BYTE *)buff, sector << 9, 512, count);
    /* Check if the Transfer is finished */
    sdstatus = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);     
    
    if (sdstatus == SD_OK)
    {
      return RES_OK;
    }
  }
  return RES_ERROR;
}
#endif /* _READONLY == 0 */


/**
   * @brief  I/O control operation
   * @param   drv : driver index
   * @retval DSTATUS : operation status
  */


#if _USE_IOCTL != 0
DRESULT disk_ioctl (
                    BYTE drv,		  /* Physical drive number (0) */
                    BYTE ctrl,		/* Control code */
                    void *buff		/* Buffer to send/receive control data */
                      )
{
  DRESULT res;
  SD_CardInfo SDCardInfo;    
  
  if (drv) return RES_PARERR;
  
  res = RES_ERROR;
  
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (ctrl) {
  case CTRL_SYNC :		/* Make sure that no pending write process */
    
    res = RES_OK;
    break;
    
  case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
    if(drv == 0)
    {
      SD_GetCardInfo(&SDCardInfo);  
      *(DWORD*)buff = SDCardInfo.CardCapacity / 512; 
    }
    
    res = RES_OK;
    break;
    
  case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
    *(WORD*)buff = 512;
    res = RES_OK;
    break;
    
  case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
    if(drv == 0)
      *(DWORD*)buff = 512;
    else
      *(DWORD*)buff = 32;
    
    
    break;
    
    
  default:
    res = RES_PARERR;
  }
  
  
  
  return res;
}
#endif /* _USE_IOCTL != 0 */
/**
   * @brief  Get Time from RTC
   * @param   None
   * @retval Time in DWORD
  */

DWORD get_fattime (void)
{
  return 0;
}

