//
// Created by Administrator on 2022/2/11/011.
//

#ifndef UNTITLED1_DEV_H
#define UNTITLED1_DEV_H

//�ַ��豸
#define NULL_MAJOR 6
#define TTY_MAJOR 4
#define TTYMAUX_MAJOR 5
#define FB_DEV_MAJOR 7
//���豸
#define BK_FLASH 0
#define MMC_MAJOR 1

//�����豸�����ȡ
#define MAJOR(a) (a>>16)
#define MINOR(a) (a&0xffff)

//����һ���豸
#define MKDEV(a,b) ((a<<16)|(b&0xffff))


#endif //UNTITLED1_DEV_H
