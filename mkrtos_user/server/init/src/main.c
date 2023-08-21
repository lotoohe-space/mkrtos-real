
int syscall(int nr, ...);
#define uint32_t unsigned int
typedef union msg_tag
{
    uint32_t raw;
    union
    {
        uint32_t type : 4;
        uint32_t type2 : 6;
        uint32_t prot : (sizeof(uint32_t) * 8) - 12;
    };
} msg_tag_t;

#define msg_tag_init(r) \
    ((msg_tag_t){.raw = (r)})

#define msg_tag_init3(t, t2, p) \
    msg_tag_init(((uint32_t)(t)&0xf) | (((uint32_t)(t2)&0x3f) << 4) | (((uint32_t)(p)) << 10))

int main(int argc, char *args[])
{
    syscall(4, msg_tag_init3(0, 1, 4).raw, 'a', 2, 3, 4, 5);
    return 0;
}
