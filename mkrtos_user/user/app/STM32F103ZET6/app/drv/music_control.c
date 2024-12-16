
#include "relay.h"
#include "sysinfo.h"
#include "usart2.h"
#include "delay.h"
#include "music_control.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define AT_CA "AT+CA" // 指定音量后面有参数。详见4.2.1
#define AT_CB "AT+CB" // 播放暂停详见4.2.1
#define AT_CC "AT+CC" // 下一曲详见4.2.1
#define AT_CD "AT+CD" // 上一曲详见4.2.1
#define AT_CE "AT+CE" // 音量+ 详见4.2.1
#define AT_CF "AT+CF" // 音量- 详见4.2.1
#define AT_CT "AT+CT" // 设置波特率后面有参数，详见4.2.2
#define AT_CM "AT+CM" // 设置模式后面有参数，详见4.2.3
#define AT_CU "AT+CU" // 设置静音后面有参数，详见4.2.4
#define AT_CS "AT+CS" // 设置DAC 高阻后面有参数，详见4.2.4
#define AT_CZ "AT+CZ" // 芯片复位芯片软复位
#define AT_CW "AT+CW" // 芯片恢复出厂设置恢复出厂设置，清除所有之前记忆的参数
#define AT_CP "AT+CP" // 上电模式后面有参数，详见4.2.5
#define AT_CR "AT+CR" // 自动回传功能芯片的关键参数会自动回传，这里可以关闭，详见4.2.6
#define AT_CJ "AT+CJ" // 单曲触发播放详见4.2.7
#define AT_CN "AT+CN" // 设置提示音详见4.2.9
#define AT_CQ "AT+CQ" // 设置播放EQ 详见4.2.10
#define AT_CK "AT+CK" // 设置蓝牙切换后台详见4.2.11
#define AT_C1 "AT+C1" // 设置按键功能关闭详见4.2.13
#define AT_C2 "AT+C2" // 芯片主动返回数据详见4.2.14
#define AT_C3 "AT+C3" // 保留详见4.2.15
#define AT_C4 "AT+C4" // 蓝牙是否上电回连详见4.2.16

#define AT_AA "AT+AA" // 设置播放模式后面有参数。详见5.1.4
#define AT_AB "AT+AB" // 指定物理顺序播放详见5.1.5
#define AT_AC "AT+AC" // 指定播放模式详见5.1.6
#define AT_AD "AT+AD" // 指定播放的设备待定
#define AT_AE "AT+AE" // 指定播放的EQ 待定-- 不支持
#define AT_AS "AT+AS" // 指定播放的速度待定-- 不支持
#define AT_AF "AT+AF" // 指定文件夹循环播放详见5.1.6
#define AT_AJ "AT+AJ" // 指定文件夹文件名播放一次详见5.1.7
#define AT_AR "AT+AR" // 指定文件读取数据--理解为读txt 详见5.1.10
#define AT_AL "AT+AL" // 指定路径删除文件详见5.1.12

#define AT_BA "AT+BA" // 蓝牙的控制相关命令详见6.1.5
#define AT_BD "AT+BD" // 设置EDR 蓝牙名称这里EDR 指的是蓝牙音频和SPP 后面有参数 AT+BD1234\r\n 这里是设置EDR 的蓝牙名为“1234”
#define AT_BE "AT+BE" // 设置EDR 连接密码后面有参数，详见6.1.3
#define AT_BM "AT+BM" // 设置BLE 蓝牙名称这里的BLE 就是指“低功耗蓝牙”
#define AT_BN "AT+BN" // 设置BLE 连接密码
#define AT_BS "AT+BS" // 设置EDR 的MAC 地址详见6.1.6 。BLE 的地址不需要设置，在EDR 基础上自动生成
#define AT_B1 "AT+B1" // 简易密码设置后面只有00或者01,00代表是关闭，01代表的是打开
#define AT_B2 "AT+B2" // 通话设置同上
#define AT_B3 "AT+B3" // 蓝牙音频设置同上
#define AT_B4 "AT+B4" // 控制BLE 的打开关闭详见6.1.3
#define AT_B5 "AT+B5" // 控制EDR 的打开关闭
#define AT_BT "AT+BT" // 指定号码拨号详见6.1.3
#define AT_U0 "AT+U0" // 指定服务UUID 详见7.6
#define AT_U1 "AT+U1" // 指定特征码1
#define AT_U2 "AT+U2" // 指定特征码2
#define AT_U3 "AT+U3" // 指定特征码3

#define AT_QM "AT+QM" // 查询播放模式

#define AT_END "\r\n"
// 运行接口函数
#define RUN_INTFACE(inx, a, b)              \
    if (a[inx] != 0x00)                     \
    {                                       \
        a[inx]((u8 *)b, strlen((char *)b)); \
    }
#define RUN_INTFACE_RAW(inx, a, b, c) \
    if (a[inx] != 0x00)               \
    {                                 \
        a[inx]((u8 *)b, c);           \
    }

void music_recv_data_fun(u8 *data, u16 len);

music_com_send_fun music_com_send_cb[2] = {0};

// 字符串发送函数
void music_com_set_send_fun(int inx, music_com_send_fun music_comd_send)
{
    music_com_send_cb[inx] = music_comd_send;
}

void bluetooth_init_cfg(int inx, music_com_send_fun func)
{
    // usart2_set_recv_data_cb(music_recv_data_fun);
    music_com_set_send_fun(inx, func);

    bluetooth_rst(inx); // 进行软复位，等到信息
    delay_ms(200);
    bluetooth_query_mode(inx);
    delay_ms(20);

    //	bluetooth_set_CP("02");

    delay_ms(20);
}
void bluetooth_name_set(int inx, const char *blueName)
{
    RUN_INTFACE(inx, music_com_send_cb, AT_BD);
    RUN_INTFACE(inx, music_com_send_cb, blueName);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
void bluetooth_query_mode(int inx)
{
    RUN_INTFACE(inx, music_com_send_cb, AT_QM);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
void bluetooth_rst(int inx)
{
    RUN_INTFACE(inx, music_com_send_cb, AT_CZ);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
void bluetooth_set_EDR_name(int inx, const char *str)
{
    RUN_INTFACE(inx, music_com_send_cb, AT_BD);
    RUN_INTFACE(inx, music_com_send_cb, str);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
void bluetooth_set_EDR_psd(int inx, const char *psd)
{
    RUN_INTFACE(inx, music_com_send_cb, AT_BE);
    RUN_INTFACE(inx, music_com_send_cb, psd);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}

void bluetooth_send_data(int inx, u8 *data, u16 len)
{
    RUN_INTFACE_RAW(inx, music_com_send_cb, data, len);
}
// 设置上电进入什么模式
void bluetooth_set_CP(int inx, const char *str)
{
    RUN_INTFACE(inx, music_com_send_cb, AT_CP);
    RUN_INTFACE(inx, music_com_send_cb, str);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}

u8 bluetoothInitFlag = 0;
// 蓝牙信息
BluetoothInfo blthInfo = {1, 0};

// void dev_control(u8 *data,u16 len);

////蓝牙返回了数据
// void music_recv_data_fun(u8 *data,u16 len){
//	char *temp;
//	//可以在这里进行处理，也可以不处理蓝牙返回的数据
//	//蓝牙模式
//	if(strstr(data,"QM+01")!=0x00){
//		blthInfo.bluetooth_play_mode=1;
//	}else if(strstr(data,"QM+02")!=0x00){
//		blthInfo.bluetooth_play_mode=2;
//	}else if(strstr(data,"QM+03")!=0x00){
//		blthInfo.bluetooth_play_mode=3;
//	}else if((temp=strstr(data,"QA+"))!=0x00){
//		//音量
//		blthInfo.bluetooth_volume=atoi(data+3);
//		bluetoothInitFlag=1;
//	}	else {
//		dev_control(data,len);
//	}
// }

// void dev_control(u8 *data,u16 len){
////	if(len<3){
////		return;
////	}
////	switch(data[0]){

////		case 'm'://下一曲
////			if(data[1]=='d'){
////				music_down();
////			}else if(data[1]=='u'){
////				music_up();
////			}else 	if(data[1]=='s'){
////				music_volume_sub();
////			}
////			break;
////	}
//
//	//用户自定义数据
//	///包头 包长 类型  状态
//	u32 pack_head;
//	u32 pack_len;
//	u32 type;
//	u32 status;
//	pack_head=((u32*)data)[0];
//	if(pack_head!=0xffeeddcc){
//		return;
//	}
//	pack_len=((u32*)data)[1];
//	type=((u32*)data)[2];
//	status=((u32*)data)[3];
//	switch(type){
//		case 0://上一曲
//			music_up();
//			break;
//		case 1://下一曲
//			music_down();
//			break;
//		case 2://暂停
//			music_pause();
//			break;
//		case 3://加音量
//			music_volume_add();
//			break;
//		case 4://减音量
//			music_volume_sub();
//			break;
//		case 5://继电器0
//			relay_ctrl(type-5,status);
//			break;
//		case 6://继电器1
//			relay_ctrl(type-5,status);
//			break;
//		case 7://继电器3
//			relay_ctrl(type-5,status);
//			break;
//		case 8://继电器4
//			relay_ctrl(type-5,status);
//			break;
//		case 9://继电器5
//			relay_ctrl(type-5,status);
//			break;
//		case 15://模式切换
//			music_mode_next();
//			break;
//		case 11://字符串信息
//		{
//			u8 i=0;
//			u8 *str=data+16;
//			#include "save_info.h"
//
////			for(i=0;i<8;i++){
////				write_text(i,(char*)str);
////				str+=12;
////			}
//		}
//			break;
//	}
//}
/////协议说明
/////包长 噪声0 噪声1 温度0 温度1 温度2 温度3 温度4 温度5 保留0 保留1
// vu32 timer_tick_count_last_main=0;
// void bluetooth_loop(void){
//	if(timer_tick_count-timer_tick_count_last_main>1090){
//		u8 send_data[50]={0};
//		u32 temp;
//		s32 temp_s;
//		u8 i=0;
//		u16 temp_byte;
//
//		temp=44;//包长
//		memcpy(send_data+i,&temp,4);
//		i+=4;
//
//		temp=((u32)sys_info.noise[0]);
//		memcpy(send_data+i,&temp,4);
//		i+=4;
//
//		temp=((u32)sys_info.noise[1]);
//		memcpy(send_data+i,&temp,4);
//		i+=4;
//
//		temp_s=((s32)sys_info.temp[0]);
//		memcpy(send_data+i,&temp_s,4);
//		i+=4;
//		temp_s=((s32)sys_info.temp[1]);
//		memcpy(send_data+i,&temp_s,4);
//		i+=4;
//		temp_s=((s32)sys_info.temp[2]);
//		memcpy(send_data+i,&temp_s,4);
//		i+=4;
//		temp_s=((s32)sys_info.temp[3]);
//		memcpy(send_data+i,&temp_s,4);
//		i+=4;
//		temp_s=((s32)sys_info.temp[4]);
//		memcpy(send_data+i,&temp_s,4);
//		i+=4;
//		temp_s=((s32)sys_info.temp[5]);
//		memcpy(send_data+i,&temp_s,4);
//		i+=4;
//
//		temp_byte=((u32)sys_info.relay[0]);
//		memcpy(send_data+i,&temp_byte,2);
//		i+=2;
//
//		temp_byte=((u32)sys_info.relay[1]);
//		memcpy(send_data+i,&temp_byte,2);
//		i+=2;
//
//		temp_byte=((u32)sys_info.relay[2]);
//		memcpy(send_data+i,&temp_byte,2);
//		i+=2;
//
//		temp_byte=((u32)sys_info.relay[3]);
//		memcpy(send_data+i,&temp_byte,2);
//		i+=2;
//
//		temp_byte=((u32)sys_info.relay[4]);
//		memcpy(send_data+i,&temp_byte,2);
//		i+=2;
//
//		temp_byte=((u32)sys_info.relay[5]);
//		memcpy(send_data+i,&temp_byte,2);
//		i+=2;
//
//
//		bluetooth_send_data(send_data,sizeof(send_data));
//
//
//		timer_tick_count_last_main=timer_tick_count;
//	}
// }

// 蓝牙 U盘 tf卡三个模式循环
void music_mode_next(int inx)
{
    switch (blthInfo.bluetooth_play_mode)
    {
    case 1:
        music_mode(inx, UP_MODE);
        blthInfo.bluetooth_play_mode = 2;
        break;
    case 2:
        music_mode(inx, TF_MODE);
        blthInfo.bluetooth_play_mode = 3;
        break;
    case 3:
        music_mode(inx, BULETOOTH_MODE);
        blthInfo.bluetooth_play_mode = 4;
        break;
    case 4:
        music_mode(inx, ADUIO_DEV_MODE);
        blthInfo.bluetooth_play_mode = 1;
        break;
    }
}
void music_mode(int inx, u8 index)
{
    switch (index)
    {
    case NEXT_MODE:
        RUN_INTFACE(inx, music_com_send_cb, AT_CM);
        RUN_INTFACE(inx, music_com_send_cb, "00");
        RUN_INTFACE(inx, music_com_send_cb, AT_END);
        break;
    case BULETOOTH_MODE:
        RUN_INTFACE(inx, music_com_send_cb, AT_CM);
        RUN_INTFACE(inx, music_com_send_cb, "01");
        RUN_INTFACE(inx, music_com_send_cb, AT_END);
        break;
    case UP_MODE:
        RUN_INTFACE(inx, music_com_send_cb, AT_CM);
        RUN_INTFACE(inx, music_com_send_cb, "02");
        RUN_INTFACE(inx, music_com_send_cb, AT_END);
        break;
    case TF_MODE:
        RUN_INTFACE(inx, music_com_send_cb, AT_CM);
        RUN_INTFACE(inx, music_com_send_cb, "03");
        RUN_INTFACE(inx, music_com_send_cb, AT_END);
        break;
    case SIGNLE_MUSIC_MODE:
        RUN_INTFACE(inx, music_com_send_cb, AT_CM);
        RUN_INTFACE(inx, music_com_send_cb, "04");
        RUN_INTFACE(inx, music_com_send_cb, AT_END);
        break;
    case EX_AUX_MODE:
        RUN_INTFACE(inx, music_com_send_cb, AT_CM);
        RUN_INTFACE(inx, music_com_send_cb, "05");
        RUN_INTFACE(inx, music_com_send_cb, AT_END);
        break;
    case ADUIO_DEV_MODE:
        RUN_INTFACE(inx, music_com_send_cb, AT_CM);
        RUN_INTFACE(inx, music_com_send_cb, "07");
        RUN_INTFACE(inx, music_com_send_cb, AT_END);
        break;
    case IDLE_MODE:
        RUN_INTFACE(inx, music_com_send_cb, AT_CM);
        RUN_INTFACE(inx, music_com_send_cb, "08");
        RUN_INTFACE(inx, music_com_send_cb, AT_END);
        break;
    }
}

void music_up(int inx)
{

    RUN_INTFACE(inx, music_com_send_cb, AT_CC);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
void music_down(int inx)
{

    RUN_INTFACE(inx, music_com_send_cb, AT_CD);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
void music_volume_set(int inx, int volume)
{
    char data[5];
    if (volume > 23)
    {
        volume = 23;
    }
    if (volume < 0)
    {
        volume = 0;
    }
    RUN_INTFACE(inx, music_com_send_cb, AT_CA);
    sprintf(data, "%02d", volume);
    RUN_INTFACE(inx, music_com_send_cb, data);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
void music_volume_add(int inx)
{
    RUN_INTFACE(inx, music_com_send_cb, AT_CE);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
void music_volume_sub(int inx)
{
    RUN_INTFACE(inx, music_com_send_cb, AT_CF);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
void music_pause(int inx)
{
    RUN_INTFACE(inx, music_com_send_cb, AT_CB);
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
void music_mute(int inx)
{
    RUN_INTFACE(inx, music_com_send_cb, AT_CU);
    RUN_INTFACE(inx, music_com_send_cb, "02"); // 当前如果是静音，则解除。否则就静音
    RUN_INTFACE(inx, music_com_send_cb, AT_END);
}
