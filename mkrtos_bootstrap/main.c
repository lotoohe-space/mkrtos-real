

#include <type.h>
#include <stm32f2xx.h>
char * mkrtos_strcpy (register char *s1, register const char *s2);
int mkrtos_strcmp (const char *s1, const char *s2);

//! 内核镜像的开始地址
#define KERNEL_IMG_START_ADDR   (0X8000000 + 0x4000)
//! 内核的名字
#define KERNEL_NAME "kernel.bin"
//! CPIO_magic
#define CPIO_MAGIC 0x71c7
//! cpio文件结构体的，大小26bytes
#pragma pack (1)
typedef struct cpio_fs {
    uint16_t magic;
    uint16_t dev;
    uint16_t ino; //i_node
    uint16_t mode;
    uint16_t uid;
    uint16_t gid;
    uint16_t nlink;
    uint16_t rdev;
    uint32_t mtime;
    uint16_t namesize;
    uint16_t filesize_h;
    uint16_t filesize_l;
    uint8_t name[0];
} cpio_fs_t; 

void *cpio_find_file(uint32_t st_addr, uint32_t end_addr, const char *file_name)
{
    for(int i = st_addr;i < end_addr;) {
        cpio_fs_t *file = (cpio_fs_t *)i;
        if (file->magic != CPIO_MAGIC) {
            return NULL;
        }
        if (mkrtos_strcmp("TRAILER!!", file->name) == 0) {
            return NULL;
        }
        if (mkrtos_strcmp(file_name, file->name) == 0) {
            return (void *)(i + sizeof(cpio_fs_t) + file->namesize);
        }
        i += sizeof(cpio_fs_t) + file->namesize +
            (file->filesize_l | (file->filesize_h << 16));
    }
    return NULL;
}
#define KERNEL_ENTRY_ADDR       (0x8000000 + 0x00004000)
#define KERNEL_VECTORS_ADDR     (0x20000000)
#define KERNEL_RAM_ADDR         (0x20000000)
// #define KERNEL_RAM_SIZE         (96 * 1024)
// #define DRIVER_ENTRY_ADDR       (0x8000000 + 0x000A0000)
void jump2kernel(void)
{
    uint32_t jump_addr;
    void (*_main)(void);

    if(((*(__IO uint32_t*)KERNEL_IMG_START_ADDR) & 0x2FFE0000 ) == 0x20000000)//检查栈顶地址是否合法,即检查此段Flash中是否已有APP程序
    { 
        __set_PRIMASK(1);

        /* disable UART */
        // USART_Reset(USART1);
        /* disable GPIO */
        // GPIO_Reset(GPIOA);
        /* disable RCC */
        // RCC_Reset();
        /* disable EXTI */
        // EXTI_Reset();

        /* disable and clean up all interrupts. */
        {
            int i;

            for(i = 0; i < 8; i++)
            {
                /* disable interrupts. */
                NVIC->ICER[i] = 0xFFFFFFFF;

                /* clean up interrupts flags. */
                NVIC->ICPR[i] = 0xFFFFFFFF;
            }
        }

        /* reset register values */
        __set_BASEPRI(0);
        __set_FAULTMASK(0);

        /* initialize main stack pointer */
         __set_MSP(*(__IO uint32_t*) KERNEL_IMG_START_ADDR);
        __set_CONTROL(0);

        __ISB();

        __disable_irq();
        
        /* Set new vector table pointer */
        // SCB->VTOR = KERNEL_ENTRY_ADDR;
        jump_addr = *(__IO uint32_t*) (KERNEL_IMG_START_ADDR  + 4);
        _main = (void *) jump_addr;
                          
        _main();
    }
}
int main(void)
{
    // void *knl_st_addr = cpio_find_file(KERNEL_IMG_START_ADDR, KERNEL_IMG_START_ADDR+1024*1024, KERNEL_NAME);
    
    // if (!knl_st_addr) {
    //     while (1);
    // }
    jump2kernel();
    while(1);
}

void _start(void){

	main();
}
