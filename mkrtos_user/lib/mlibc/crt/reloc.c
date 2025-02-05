/**
 * @brief
 *
 */
#include "elf.h"
#include "reloc.h"
#include "u_app.h"
// extern int __text_start__;
// extern int __data_start__;
// extern int __got_start__;
// extern int __got_end__;
// extern int __rel_start__;
// extern int __rel_end__;
#define R_TYPE(x) ((x) & 255)
#define R_SYM(x) ((x) >> 8)
#define R_INFO ELF32_R_INFO
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
        case REL_RELATIVE:
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
        // case REL_OFFSET:
        //     addend -= (size_t)reloc_addr;
        case REL_SYMBOLIC:
        case REL_GOT:
        case REL_PLT:
            // *reloc_addr = sym_val + addend;
            break;
        }
    }
    return;
}
static uint32_t lookup_symbol(app_info_t *app, const char *symbol_name)
{

    // 实现符号查找逻辑
    return 0x12345678; // 示例地址
}
typedef struct mk_ldso
{
    Elf32_Sym *syms;
    char *strings;
} mk_ldso_t;
static mk_ldso_t ldso;
static void so_parse(app_info_t *app)
{
    // 遍历动态段
    Elf32_Dyn *dyn = (char *)app + app->i.dyn_start;

    for (int i = 0; i < dyn[i].d_tag != DT_NULL; i++)
    {
        if (dyn[i].d_tag == DT_SYMTAB)
        {
            Elf32_Sym *symtab = (char *)app + dyn[i].d_un.d_val;
            ldso.syms = symtab;
        }
        else if (dyn[i].d_tag == DT_STRTAB)
        {
            ldso.strings = (char *)app + dyn[i].d_un.d_val;
        }
    }
}
static const char *get_symbol_name(app_info_t *app, uint32_t sym)
{

    // 实现根据符号索引获取符号名称的逻辑
    return "external_function"; // 示例符号名称
}

void _reloc_so(unsigned int *gbase, unsigned int tbase)
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
        case REL_RELATIVE:
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
        // case REL_OFFSET:
        //     addend -= (size_t)reloc_addr;
        case REL_SYMBOLIC:
        case REL_GOT:
        case REL_PLT:
        {
            // uint32_t sym = ELF32_R_SYM(rel[i + 1]);
            // // uint32_t *target = (uint32_t *)(base_address + rel->r_offset);
            // // *reloc_addr = sym_val + addend;
            // if (*pointer >= ds)
            // {
            //     offset = *pointer - ds;
            //     *pointer = offset + (unsigned int)gbase;
            // }
            // else
            // {
            //     offset = *pointer - ts;
            //     *pointer = offset + (unsigned int)tbase;
            // }
        }
        break;
        }
    }
    so_parse(app);
    return;
}
