/**************************************************************************/ /**
 * @file     version.h
 * @brief    版本/目标控制
 * @version  V3.1
 * @date     2022.08.31
 ******************************************************************************/
#ifndef __VERSION_H__
#define __VERSION_H__

/* (用户必须设置)请根据实际使用的官方 Demo 板 PCB 丝印选择对应型号 */
#define SWM34SRE_PIN64_A001				0  
#define SWM34SVE_PIN100_A001			1 
#define SWM34SVE_PIN100_A002			2 // SWM34SVE_A002 && A003 板子仅丝印不同, 其余一致不变, 可替换使用.
#define SWM34S_LCM_PCBV					SWM34SRE_PIN64_A001   //SWM34SVE_PIN100_A002

#ifndef SWM34S_LCM_PCBV

#error "Please define the SWM34S_LCM_PCBV"

#endif //No Define SWM34S_LCM_PCBV

/* 说明 : 本文件中 __RESOLUTION_TARGET__ 已按预期下拉 Target 设定, 应用本工程时 <请勿随意> 更改 __RESOLUTION_TARGET__ (__RESOLUTION_TARGET__ 在 Keil Option for Target => C/C++(AC6) => Preprocessor Symbols => Define 设置)
 * 其余 TFT-LCD 分辨率 / 色彩 / 屏幕类型 / Demo 板 PCB 丝印等设置则可自定义.
 * 
 * enum : 编译目标选择

//-----------------------------------------RGB565--------------------------------------------//
 * 0 => LVGL 官方Demo, 其UI演示的分辨率自适应, 例子选择在 lv_ex_conf.h 中宏配置, 分辨率则在 version.h 内设置
 * 1 => Lib \ SimplLCD_RGB 例程在 LVGL 下实现 - 不依赖于任何 UI 素材, 单纯显示颜色测试
 * 2 => SYNWIT 应用例程 - 480  *  272 (JLT4303A)——晶力泰 4.3 寸电容屏
 * 3 => SYNWIT 应用例程 - 480  *  480 (TL040WVS03)——冠显光电 4.0 寸方形屏 / (TL021WC02_Round)——冠显光电 2.1 寸圆形屏
 * 4 => SYNWIT 应用例程 - 800  *  480 (JLT4305A)——晶力泰 4.3 寸电容屏 / (ATK_MD0700R)——正点原子 7 寸电容屏
 * 5 => SYNWIT 应用例程 - 1024 *  600 (WKS70170)——慧勤光电 7 寸电容屏 / (H743)——玫瑰科技 7 寸电容屏

//-----------------------------------------RGB888--------------------------------------------//
 * 6 => LVGL 官方Demo, 其UI演示的分辨率自适应, 例子选择在 lv_ex_conf.h 中宏配置, 分辨率则在 version.h 内设置
 * 7 => Lib \ SimplLCD_RGB 例程在 LVGL 下实现 - 不依赖于任何 UI 素材, 单纯显示颜色测试
 * 8 => SYNWIT 应用例程 - 480  *  272 (JLT4303A)——晶力泰 4.3 寸电容屏
 * 9 => SYNWIT 应用例程 - 480  *  480 (TL040WVS03)——冠显光电 4.0 寸方形屏 / (TL021WC02_Round)——冠显光电 2.1 寸圆形屏
 * 10 => SYNWIT 应用例程 - 800  *  480 (JLT4305A)——晶力泰 4.3 寸电容屏 / (ATK_MD0700R)——正点原子 7 寸电容屏
 * 11 => SYNWIT 应用例程 - 1024 *  600 (WKS70170)——慧勤光电 7 寸电容屏 / (H743)——玫瑰科技 7 寸电容屏
 *
 * PS: 实测在 1024*600-RGB888 下应用 lvgl, 8 MB SDRAM 空间捉襟见肘, 效果不佳(待后续推出大容量 SDRAM ).
 * 
 * 
 * 0xFF => UserBoot 功能应用, 适用于上述所有目标的固件(请注意: <使用前必须先打开查看 userboot.c 文件内的说明, 不看说明直接下载是不可接受的> )
 */

//-----------------------------------------RGB565--------------------------------------------//
#if 		(__RESOLUTION_TARGET__ == 0)

//自定义
#define LCD_HDOT		(480)
#define LCD_VDOT		(800)
// 水平点数
// 垂直点数

#elif 		(__RESOLUTION_TARGET__ == 1)

//自定义
#define LCD_HDOT		(800)
#define LCD_VDOT		(480)

#elif 		(__RESOLUTION_TARGET__ == 2)

//不可改变
#define LCD_HDOT		(480)
#define LCD_VDOT		(272)

#elif 		(__RESOLUTION_TARGET__ == 3)

//不可改变
#define LCD_HDOT		(480)
#define LCD_VDOT		(480)

#elif 		(__RESOLUTION_TARGET__ == 4)

//不可改变
#define LCD_HDOT		(800)
#define LCD_VDOT		(480)

#elif 		(__RESOLUTION_TARGET__ == 5)

//不可改变
#define LCD_HDOT		(1024)
#define LCD_VDOT		(600)
 
//-----------------------------------------RGB888--------------------------------------------//
#elif 		(__RESOLUTION_TARGET__ == 6)

//自定义
#define LCD_HDOT		(800)
#define LCD_VDOT		(480)

#elif 		(__RESOLUTION_TARGET__ == 7)

//自定义
#define LCD_HDOT		(800)
#define LCD_VDOT		(480)

#elif 		(__RESOLUTION_TARGET__ == 8)

//不可改变
#define LCD_HDOT		(480)
#define LCD_VDOT		(272)

#elif 		(__RESOLUTION_TARGET__ == 9)

//不可改变
#define LCD_HDOT		(480)
#define LCD_VDOT		(480)

#elif 		(__RESOLUTION_TARGET__ == 10)

//不可改变
#define LCD_HDOT		(800)
#define LCD_VDOT		(480)

#elif 		(__RESOLUTION_TARGET__ == 11)

//不可改变
#define LCD_HDOT		(1024)
#define LCD_VDOT		(600)

#elif 		(__RESOLUTION_TARGET__ == 0xFF)

//仅为了绕过编译错误所需, 无实际作用
#define LCD_HDOT		(480)
#define LCD_VDOT		(272)

#else 

#error "Please define the __RESOLUTION_TARGET__ "

#endif //__RESOLUTION_TARGET__


#ifndef LCD_HDOT

#error "Please define the LCD HDOT"

#endif //No Define H / V

#ifndef LCD_VDOT

#error "Please define the LCD VDOT"

#endif //No Define H / V


/* LCD_FORMAT : LCD 显示格式
 * enum : 
 * 0 => LCD_RGB565
 * 1 => LCD_RGB888 (特别提醒 : SWM341系列实现的 RGB888 都是 Alpha RGB888)
 */
#define LCD_RGB565				0
#define LCD_RGB888				1

#if 		(0 <= __RESOLUTION_TARGET__ && __RESOLUTION_TARGET__ < 6)
#define LCD_FMT		            LCD_RGB565 

#elif 		(6 <= __RESOLUTION_TARGET__ && __RESOLUTION_TARGET__ < 12)
#define LCD_FMT		            LCD_RGB888 

#elif 		(__RESOLUTION_TARGET__ == 0xFF)
#define LCD_FMT		            LCD_RGB565 //仅为了绕过编译错误所需, 无实际作用

#else 

#error "Please define the LCD_FMT : LCD_RGBxxx"

#endif //LCD_FORMAT


#if (SWM34S_LCM_PCBV == SWM34SRE_PIN64_A001 && LCD_FMT == LCD_RGB888)

// 64 脚无法配置成 RGB888 , 引脚不足.
#error "The Target cannot select this option, No more Pins(IO)!"

#endif // SWM34SRE_PIN64 && LCD_RGB888 

//仅在 480*480 / 480*800/ 800*480 / 1024*600 三类分辨率规格上提供对 不同屏幕型号 的支持
#if 	( ( LCD_HDOT == 480 && LCD_VDOT == 480 ) || ( LCD_HDOT == 480 && LCD_VDOT == 800 ) || ( LCD_HDOT == 800 && LCD_VDOT == 480 ) || ( LCD_HDOT == 1024 && LCD_VDOT == 600 ) )
/* enum : TFT-LCD 型号
 * 480  *  480 
 * 0 => TL040WVS03(冠显光电——4.0寸方形屏)
 * 1 => TL021WVC02_Round(冠显光电——2.1寸圆形屏)
 
 * 800  *  480 
 * 0 => JLT4305A(晶力泰——4.3寸电容屏)
 * 1 => ATK_MD0700R(正点原子——7寸电容屏)
 
 * 1024 *  600 
 * 0 => WKS70170(慧勤光电——7寸电容屏)
 * 1 => H743(玫瑰科技——7寸电容屏)
 */
#if 	( LCD_HDOT == 480 && LCD_VDOT == 480 )
#define TL040WVS03                  0
#define TL021WVC02_Round            1

#define LCD_TYPE			TL040WVS03 

#elif   ( LCD_HDOT == 480 && LCD_VDOT == 800 )
#define RV_HX8363A                  0
#define RV_OTM8009A                 1

#define LCD_TYPE			RV_OTM8009A

#elif   ( LCD_HDOT == 800 && LCD_VDOT == 480 )
#define JLT4305A                    0
#define ATK_MD0700R                 1

#define LCD_TYPE			ATK_MD0700R

#elif   ( LCD_HDOT == 1024 && LCD_VDOT == 600 )
#define WKS70170                    0
#define H743                        1

#define LCD_TYPE			H743

#else 

#error "Please define the LCD_TYPE : you custom"

#endif //( LCD_HDOT == ? && LCD_VDOT == ? )

#endif //480*480 || 800*480 || 1024*600


#define SIZE_32MB 				(0x02000000)
#define SIZE_16MB 				(0x01000000)
#define SIZE_8MB 				(0x00800000)
#define SIZE_1MB 				(0x00100000)
#define SIZE_512KB 				(0x00080000)
#define SIZE_256KB 				(0x00040000)
#define SIZE_128KB 				(0x00020000)
#define SIZE_64KB 				(0x00010000)
#define SIZE_32KB 				(0x00008000)
#define SIZE_1KB 				(0x00000400)

/* SDRAMM_SIZE : 内置的 SDRAM 大小,提供客制化选项 */
#define SDRAMM_SIZE 			SIZE_8MB // SDRAM 大小 = 8 MB


/* #define GUI_VERSION			711 //GUI版本直接分开工程以示区别
 * enum : LVGL 版本控制
 * 612 => lvgl-V6.1.2
 * 711 => lvgl-V7.11.0
 * 820 => lvgl-V8.2.0
 */

#endif //__VERSION_H__
