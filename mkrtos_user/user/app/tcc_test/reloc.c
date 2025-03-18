/**
 * @brief
 *
 */
typedef struct app_info
{
    const char d[32];
    const char magic[8];
    union
    {
        struct exec_head_info
        {
            unsigned long ram_size;
            unsigned long heap_offset;
            unsigned long stack_offset;
            unsigned long heap_size;
            unsigned long stack_size;
            unsigned long data_offset;
            unsigned long bss_offset;
            unsigned long got_start;
            unsigned long got_end;
            unsigned long rel_start;
            unsigned long rel_end;
            unsigned long text_start;
            unsigned long data_start;
            unsigned long dyn_start;
        } i;
        // const char d1[256];
    };
    const char dot_text[];
} app_info_t;
#define R_TYPE(x) ((x) & 255)
#define R_SYM(x) ((x) >> 8)
#define R_INFO ELF32_R_INFO
#define R_ARM_RELATIVE		23
/**
 * gbase: 内存基地址
 * tbase: text的首地址
 */
void _reloc(unsigned int *gbase, unsigned int tbase)
{
    int i;
    unsigned int s;
    unsigned int offset;
    unsigned int gs, ge, ts;
    unsigned int *rs, *re;
    unsigned int *pointer;
    unsigned int ds;

    app_info_t *app = (void *)tbase;
    ds = (unsigned int)app->i.data_start;
    gs = (unsigned int)app->i.got_start;
    ge = (unsigned int)app->i.got_end;
    ts = (unsigned int)app->i.text_start;

    for (i = 0, s = gs; s < ge; s += 4, i++)
    {
        if (gbase[i] >= ds)
        {
            offset = gbase[i] - ds;
            gbase[i] = offset + (unsigned int)gbase;
        }
        else
        {
            offset = gbase[i] - ts;
            gbase[i] = offset + tbase;
        }
    }
    rs = (unsigned int *)(app->i.rel_start + tbase);
    re = (unsigned int *)(app->i.rel_end + tbase);
    // rel
    for (i = 0, s = (unsigned int)rs; s < (unsigned int)re; s += 8, i += 2)
    {
        offset = rs[i] - ds;
        pointer = (unsigned int *)((unsigned int)gbase + offset);
        switch (R_TYPE(rs[i + 1]))
        {
        case R_ARM_RELATIVE:
        {
            if (*pointer >= ds)
            {
                offset = *pointer - ds;
                *pointer = offset + (unsigned int)gbase;
            }
            else
            {
                offset = *pointer - ts;
                *pointer = offset + (unsigned int)tbase;
            }
        }
        break;
        }
    }
    return;
}