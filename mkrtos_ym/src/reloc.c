/*
 * reloc.c
 *
 *  Created on: 2022��9��3��
 *      Author: Administrator
 */

extern int __text_start__;
extern int __got_start__;
extern int __got_end__;
extern int __rel_start__;
extern int __rel_end__;

/*
 * @brief gbase ram�Ŀ�ʼ��ַ,tbase text�Ŀ�ʼ��ַ
 */
void reloc(unsigned int *gbase, unsigned int tbase)
{
    int i;
    unsigned int s;
    unsigned int offset;
    unsigned int gs, ge, ts;
    unsigned int *rs, *re;
    unsigned int *pointer;

    gs = (unsigned int)&__got_start__; //got��Ŀ�ʼƫ��
    ge = (unsigned int)&__got_end__; //got��Ľ���ƫ��
    ts = (unsigned int)&__text_start__;//text�εĿ�ʼƫ��

    for (i = 0, s = gs; s < ge; s += 4, i++)
    {
        if (gbase[i] >= gs)
        {
            offset = gbase[i] - gs;
            gbase[i] = offset + (unsigned int)gbase;
        }
        else
        {
            offset = gbase[i] - ts;
            gbase[i] = offset + tbase;
        }
    }

    rs = (unsigned int*)&__rel_start__;
    re = (unsigned int*)&__rel_end__;

    for (i = 0, s = (unsigned int)rs; s < (unsigned int)re; s+=8, i+=2)
    {
        if (rs[i+1] == 0x00000017)
        {
            offset = rs[i] - gs;
            pointer = (unsigned int *)((unsigned int)gbase + offset);

            if (*pointer >= gs)
            {
                offset = *pointer -gs;
                *pointer = offset + (unsigned int)gbase;
            }
            else
            {
                offset = *pointer - ts;
                *pointer = offset + (unsigned int)tbase;
            }
        }
    }
    return;
}

