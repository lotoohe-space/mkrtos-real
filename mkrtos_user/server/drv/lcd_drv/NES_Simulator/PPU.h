/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief  
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */  

#ifndef _PPU_H_
#define _PPU_H_

/* Includes ------------------------------------------------------------------*/ 
#include "nes_datatype.h"

/********************************************************************************
 **sprite 存储器
 */
typedef struct{
	uint8_t spr_ram[256];			/* 精灵存储器，存储卡通数据*/
	uint8_t spr_addrcnt;			/* sprite 地址计数器*/
}Spr_MemType;

typedef struct{
 	uint8_t	y;		//垂直Y轴坐标(垂直位置-1)【Vertical Position-1 (FFh,00h..EEh=Scanline 0..239, EFh..FEh=Not displayed)】
	uint8_t	t_num;	//title 号，(R1.5 )0：(8*8: [7:0]title号码) , 1:(8*16: [7:1]title号码(0~127) ,[0] pattern 0 或1)
	uint8_t	attr;	//显示属性 [7]垂直翻转，[6]水平饭庄，[5]显示在背景上(下),[4:2]未使用，[1:0]sprite palette(0~3颜色索引表)
	uint8_t	x;		//水平x轴坐标
}SpriteType;

/********************************************************************************
 **PPU 存储器 map
 */
typedef struct{    
	uint16_t PPU_addrcnt;	/* PPU地址计数器高八位，第一次写入， PPU地址计数器低八位，第一次写入*/
	uint8_t  PPU_readtemp; /*读取操作缓冲*/

	/********************************************************************************
	 **PPU 内存映像 64KB寻址 16KB($0000 ~ &3FFF)物理内存,后面的为镜像
	 */
	uint8_t *patterntable0;			/*$0000 ~ $0FFF 图案表0				*/ 
	uint8_t *patterntable1; 			/*$1000 ~ $1FFF 图案表1				*/
	uint8_t *name_table[4]; 			/*$2000 ~ $23BF 命名表0（32x30块）	*/
									/*$23C0 ~ $23FF 属性表0） 			*/
	 								/*$2400 ~ $27BF 命名表1（32x30块）	*/
									/*$27C0 ~ $27FF 属性表1 			*/
	 								/*$2800 ~ $2BBF 命名表2（32x30块）	*/
									/*$2BC0 ~ $2BFF 属性表2（） 		*/
									/*$2C00 ~ $2FBF 命名表3（32x30块））*/
									/*$2FC0 ~ $2FFF 属性表3 			*/
	uint8_t image_palette[16];		/*$3F00 ~ $3F0F 背景调色板#1，颜色索引,使用颜色值（RG565）*/
	uint8_t sprite_palette[16];		/*$3F00 ~ $3F0F 精灵调色板#1，颜色索引,使用颜色值（RG565）*/
}PPU_MemType;


/********************************************************************************
 **PPU 寄存器
 */
typedef struct{
	uint8_t R0;	 //$2000
	uint8_t R1;	 //$2001
	uint8_t R2;	 //$2002
//	uint8_t R3;	 //$2003
//	uint8_t R4;	 //$2004
	uint8_t R5;	 //$2005
//	uint8_t R6;	 //$2006
//	uint8_t R7;	 //$2007
}PPU_RegType;

/********************************************************************************
 **PPU  标志
 */

/*sprite 属性标志*/
#define SPR_VFLIP		0x80
#define SPR_HFLIP		0x40
#define SPR_BG_PRIO		0x20	//(0=Sprite In front of BG, 1=Sprite Behind BG)

/*R0*/
#define R0_VB_NMI_EN	0x80
#define R0_SPR_SIZE		0x20	//(0=8x8, 1=8x16)
#define BG_PATTERN_ADDR	0x10
#define SPR_PATTERN_ADDR 0x08
#define PPU_ADDRINCR	0x04  	//bit2 （0: +1） （1: +32）
#define R0_NAME_TABLE	0x03

/*R1*/
#define R1_SPR_VISIBLE	0x10
#define R1_BG_VISIBLE	0x08
#define R1_SPR_LEFT8	0x04
#define R1_BG_LEFT8		0x02
#define R1_DISPMODE		0x01

/*R2*/
#define R2_VBlank_Flag	0x80
#define R2_SPR0_HIT		0x40
#define R2_LOST_SPR		0x20


/* 与显示行有关 ------------------------------------------------------------------*/

#define NES_DISP_WIDTH					256	  //每一行扫描像素宽度
#define CLOCKS_PER_SCANLINE				113   //每一行扫描线，CPU时钟113.66

/*扫描行号，共扫描262行(0~261)*/
#define SCAN_LINE_START_NUM				0
#define SCAN_LINE_DISPALY_START_NUM		21
#define	SCAN_LINE_DISPALY_END_NUM		261
#define	SCAN_LINE_END_NUM				262

/*背景显示 name table 表*/
#define NAME_TABLE_H_MASK	1
#define NAME_TABLE_V_MASK	2

/********************************************************************************
 * 外部引用 scanline显示变量
 */
extern int PPU_scanline;
extern uint8_t SpriteHitFlag;

/********************************************************************************
 **	外部引用 存储器数据声明
 */
extern Spr_MemType Spr_Mem;
extern PPU_RegType PPU_Reg;
extern PPU_MemType PPU_Mem;

extern SpriteType  * const sprite;
/********************************************************************************
 **	函数声明
 */


void  PPU_Init(uint8_t* vromaddr, uint8_t  ScreenMirrorType);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  PPU 存储器与寄存器函数组 
 */
void  PPU_MemWrite(uint8_t value);
uint8_t PPU_MemRead(void);

void  PPU_RegWrite(uint16_t addr, uint8_t byte);
uint8_t PPU_RegRead(uint16_t addr);

uint8_t PPU_NameTablesRead(void);
void  PPU_NameTablesWrite(uint8_t value);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  PPU 显示函数组 
 */
void NES_GetSpr0HitFlag(int y_axes);
void NES_RenderSprite88(SpriteType *sprptr, int dy_axes);
void NES_RenderSprite16(SpriteType *sprptr, int dy_axes);
void NES_RenderLine(int y_axes);
void NES_LCD_DisplayLine(int y_axes, uint16_t *Disaplyline_buffer);
void NES_LCD_BG_DisplayLine(uint16_t color);
  /**
  * @}
  */

#endif 	/* _PPU_H_*/
/*******************************END OF FILE***********************************/
