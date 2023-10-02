#include "stm32_sys.h"

typedef void (*music_com_send_fun)(u8 *string, int len);

// 工作模式
#define NEXT_MODE 0      // 下一个模式
#define BULETOOTH_MODE 1 // 蓝牙模式
#define UP_MODE 2        // 优盘模式
#define TF_MODE 3        // tf卡模式
#define SIGNLE_MUSIC_MODE 4
#define EX_AUX_MODE 5 // REV
#define ADUIO_DEV_MODE 6
#define IDLE_MODE 7

typedef struct _BluetoothInfo
{
    u8 bluetooth_play_mode;
    u8 bluetooth_volume;
} BluetoothInfo;
extern u8 bluetoothInitFlag;
extern BluetoothInfo blthInfo;

// 字符串发送函数
void music_com_set_send_fun(int inx, music_com_send_fun music_comd_send);

void bluetooth_init_cfg(int inx, music_com_send_fun func);

// 设置上电进入什么模式
void bluetooth_set_CP(int inx, const char *str);
void buletooth_set_EDR_name(int inx, const char *str);
void bluetooth_set_EDR_psd(int inx, const char *psd);
void bluetooth_query_mode(int inx);
void bluetooth_rst(int inx);
void bluetooth_name_set(int inx, const char *blueName);

void music_mode_next(int inx);
void music_mode(int inx, u8 index);
void music_up(int inx);
void music_down(int inx);
void music_volume_set(int inx, int volume);
void music_volume_add(int inx);
void music_volume_sub(int inx);
void music_pause(int inx);
void music_mute(int inx);

void bluetooth_send_data(int inx, u8 *data, u16 len);

void bluetooth_loop(void);
