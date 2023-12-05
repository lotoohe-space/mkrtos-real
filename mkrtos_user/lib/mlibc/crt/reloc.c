/**
 * @brief
 *
 */
extern int __text_start__;
extern int __got_start__;
extern int __got_end__;
extern int __rel_start__;
extern int __rel_end__;

void _reloc(unsigned int *gbase, unsigned int tbase)
{
    int i;
    unsigned int s;
    unsigned int offset;
    unsigned int gs, ge, ts;
    unsigned int *rs, *re;
    unsigned int *pointer;

    gs = (unsigned int)&__got_start__;
    ge = (unsigned int)&__got_end__;
    ts = (unsigned int)&__text_start__;

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

    rs = (unsigned int *)&__rel_start__;
    re = (unsigned int *)&__rel_end__;
    // rel
    for (i = 0, s = (unsigned int)rs; s < (unsigned int)re; s += 8, i += 2)
    {
        if (rs[i + 1] == 0x00000017)
        {
            offset = rs[i] - gs;
            pointer = (unsigned int *)((unsigned int)gbase + offset);

            if (*pointer >= gs)
            {
                offset = *pointer - gs;
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
