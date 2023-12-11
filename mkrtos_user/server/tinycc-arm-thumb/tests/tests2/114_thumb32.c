// This is a modified version of the arm64 test. Except for a few tweaks, it
// mainly drops the floating point tests. Original description:
//
// This program is designed to test some arm64-specific things, such as the
// calling convention, but should give the same results on any architecture.

#include <stdarg.h>
#include <stdio.h>

// Bypass some header trouble
#ifdef __arm__
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef int int32_t;
typedef long long int64_t;
#else
#include <stdint.h>
#endif

struct s1 { char x[1]; } s1 = { "0" };
struct s2 { char x[2]; } s2 = { "12" };
struct s3 { char x[3]; } s3 = { "345" };
struct s4 { char x[4]; } s4 = { "6789" };
struct s5 { char x[5]; } s5 = { "abcde" };
struct s6 { char x[6]; } s6 = { "fghijk" };
struct s7 { char x[7]; } s7 = { "lmnopqr" };
struct s8 { char x[8]; } s8 = { "stuvwxyz" };
struct s9 { char x[9]; } s9 = { "ABCDEFGHI" };
struct s10 { char x[10]; } s10 = { "JKLMNOPQRS" };
struct s11 { char x[11]; } s11 = { "TUVWXYZ0123" };
struct s12 { char x[12]; } s12 = { "456789abcdef" };
struct s13 { char x[13]; } s13 = { "ghijklmnopqrs" };
struct s14 { char x[14]; } s14 = { "tuvwxyzABCDEFG" };
struct s15 { char x[15]; } s15 = { "HIJKLMNOPQRSTUV" };
struct s16 { char x[16]; } s16 = { "WXYZ0123456789ab" };
struct s17 { char x[17]; } s17 = { "cdefghijklmnopqrs" };

void fa_s1(struct s1 a) { printf("%.1s\n", a.x); }
void fa_s2(struct s2 a) { printf("%.2s\n", a.x); }
void fa_s3(struct s3 a) { printf("%.3s\n", a.x); }
void fa_s4(struct s4 a) { printf("%.4s\n", a.x); }
void fa_s5(struct s5 a) { printf("%.5s\n", a.x); }
void fa_s6(struct s6 a) { printf("%.6s\n", a.x); }
void fa_s7(struct s7 a) { printf("%.7s\n", a.x); }
void fa_s8(struct s8 a) { printf("%.8s\n", a.x); }
void fa_s9(struct s9 a) { printf("%.9s\n", a.x); }
void fa_s10(struct s10 a) { printf("%.10s\n", a.x); }
void fa_s11(struct s11 a) { printf("%.11s\n", a.x); }
void fa_s12(struct s12 a) { printf("%.12s\n", a.x); }
void fa_s13(struct s13 a) { printf("%.13s\n", a.x); }
void fa_s14(struct s14 a) { printf("%.14s\n", a.x); }
void fa_s15(struct s15 a) { printf("%.15s\n", a.x); }
void fa_s16(struct s16 a) { printf("%.16s\n", a.x); }
void fa_s17(struct s17 a) { printf("%.17s\n", a.x); }

void fa1(struct s8 a, struct s9 b, struct s10 c, struct s11 d,
         struct s12 e, struct s13 f)
{
    printf("%.3s %.3s %.3s %.3s %.3s %.3s\n", a.x, b.x, c.x, d.x, e.x, f.x);
}

void fa2(struct s9 a, struct s10 b, struct s11 c, struct s12 d,
         struct s13 e, struct s14 f)
{
    printf("%.3s %.3s %.3s %.3s %.3s %.3s\n", a.x, b.x, c.x, d.x, e.x, f.x);
}

void arg(void)
{
    printf("Arguments:\n");
    fa_s1(s1);
    fa_s2(s2);
    fa_s3(s3);
    fa_s4(s4);
    fa_s5(s5);
    fa_s6(s6);
    fa_s7(s7);
    fa_s8(s8);
    fa_s9(s9);
    fa_s10(s10);
    fa_s11(s11);
    fa_s12(s12);
    fa_s13(s13);
    fa_s14(s14);
    fa_s15(s15);
    fa_s16(s16);
    fa_s17(s17);
    fa1(s8, s9, s10, s11, s12, s13);
    fa2(s9, s10, s11, s12, s13, s14);
}

struct s1 fr_s1(void) { return s1; }
struct s2 fr_s2(void) { return s2; }
struct s3 fr_s3(void) { return s3; }
struct s4 fr_s4(void) { return s4; }
struct s5 fr_s5(void) { return s5; }
struct s6 fr_s6(void) { return s6; }
struct s7 fr_s7(void) { return s7; }
struct s8 fr_s8(void) { return s8; }
struct s9 fr_s9(void) { return s9; }
struct s10 fr_s10(void) { return s10; }
struct s11 fr_s11(void) { return s11; }
struct s12 fr_s12(void) { return s12; }
struct s13 fr_s13(void) { return s13; }
struct s14 fr_s14(void) { return s14; }
struct s15 fr_s15(void) { return s15; }
struct s16 fr_s16(void) { return s16; }
struct s17 fr_s17(void) { return s17; }


void ret(void)
{
    struct s1 t1 = fr_s1();
    struct s2 t2 = fr_s2();
    struct s3 t3 = fr_s3();
    struct s4 t4 = fr_s4();
    struct s5 t5 = fr_s5();
    struct s6 t6 = fr_s6();
    struct s7 t7 = fr_s7();
    struct s8 t8 = fr_s8();
    struct s9 t9 = fr_s9();
    struct s10 t10 = fr_s10();
    struct s11 t11 = fr_s11();
    struct s12 t12 = fr_s12();
    struct s13 t13 = fr_s13();
    struct s14 t14 = fr_s14();
    struct s15 t15 = fr_s15();
    struct s16 t16 = fr_s16();
    struct s17 t17 = fr_s17();
    printf("Return values:\n");
    printf("%.1s\n", t1.x);
    printf("%.2s\n", t2.x);
    printf("%.3s\n", t3.x);
    printf("%.4s\n", t4.x);
    printf("%.5s\n", t5.x);
    printf("%.6s\n", t6.x);
    printf("%.7s\n", t7.x);
    printf("%.8s\n", t8.x);
    printf("%.9s\n", t9.x);
    printf("%.10s\n", t10.x);
    printf("%.11s\n", t11.x);
    printf("%.12s\n", t12.x);
    printf("%.13s\n", t13.x);
    printf("%.14s\n", t14.x);
    printf("%.15s\n", t15.x);
    printf("%.16s\n", t16.x);
    printf("%.17s\n", t17.x);
}

int match(const char **s, const char *f)
{
    const char *p = *s;
    for (p = *s; *f && *f == *p; f++, p++)
        ;
    if (!*f) {
        *s = p - 1;
        return 1;
    }
    return 0;
}

void myprintf(const char *format, ...)
{
    const char *s;
    va_list ap;
    va_start(ap, format);
    for (s = format; *s; s++) {
        if (match(&s, "%7s")) {
            struct s7 t7 = va_arg(ap, struct s7);
            printf("%.7s", t7.x);
        }
        else if (match(&s, "%9s")) {
            struct s9 t9 = va_arg(ap, struct s9);
            printf("%.9s", t9.x);
        }
        else
            putchar(*s);
    }
    putchar('\n');
}

void stdarg(void)
{
    printf("stdarg:\n");
    myprintf("%9s %9s %9s %9s %9s %9s", s9, s9, s9, s9, s9, s9);
    myprintf("%7s %9s %9s %9s %9s %9s", s7, s9, s9, s9, s9, s9);
}

void pll(unsigned long long x)
{
    printf("%llx\n", x);
}

void movi(void)
{
    printf("MOVI:\n");
    pll(0);
    pll(0xabcd);
    pll(0xabcd0000);
    pll(0xabcd00000000);
    pll(0xabcd000000000000);
    pll(0xffffabcd);
    pll(0xabcdffff);
    pll(0xffffffffffffabcd);
    pll(0xffffffffabcdffff);
    pll(0xffffabcdffffffff);
    pll(0xabcdffffffffffff);
    pll(0xaaaaaaaa);
    pll(0x5555555555555555);
    pll(0x77777777);
    pll(0x3333333333333333);
    pll(0xf8f8f8f8);
    pll(0x1e1e1e1e1e1e1e1e);
    pll(0x3f803f80);
    pll(0x01ff01ff01ff01ff);
    pll(0x007fffc0);
    pll(0x03fff80003fff800);
    pll(0x0007fffffffffe00);

    pll(0xabcd1234);
    pll(0xabcd00001234);
    pll(0xabcd000000001234);
    pll(0xabcd12340000);
    pll(0xabcd000012340000);
    pll(0xabcd123400000000);
    pll(0xffffffffabcd1234);
    pll(0xffffabcdffff1234);
    pll(0xabcdffffffff1234);
    pll(0xffffabcd1234ffff);
    pll(0xabcdffff1234ffff);
    pll(0xabcd1234ffffffff);

    pll(0xffffef0123456789);
    pll(0xabcdef012345ffff);

    pll(0xabcdef0123456789);
}

static uint32_t addip0(uint32_t x) { return x + 0; }
static uint64_t sublp0(uint64_t x) { return x - 0; }
static uint32_t addip123(uint32_t x) { return x + 123; }
static uint64_t addlm123(uint64_t x) { return x + -123; }
static uint64_t sublp4095(uint64_t x) { return x - 4095; }
static uint32_t subim503808(uint32_t x) { return x - -503808; }
static uint64_t addp12345(uint64_t x) { return x + 12345; }
static uint32_t subp12345(uint32_t x) { return x - 12345; }

static uint32_t mvni(uint32_t x) { return 0xffffffff - x; }
static uint64_t negl(uint64_t x) { return 0 - x; }
static uint32_t rsbi123(uint32_t x) { return 123 - x; }
static uint64_t rsbl123(uint64_t x) { return 123 - x; }

static uint32_t andi0(uint32_t x) { return x & 0; }
static uint64_t andlm1(uint64_t x) { return x & -1; }
static uint64_t orrl0(uint64_t x) { return x | 0; }
static uint32_t orrim1(uint32_t x) { return x | -1; }
static uint32_t eori0(uint32_t x) { return x ^ 0; }
static uint64_t eorlm1(uint64_t x) { return x ^ -1; }
static uint32_t and0xf0(uint32_t x) { return x & 0xf0; }
static uint64_t orr0xf0(uint64_t x) { return x | 0xf0; }
static uint64_t eor0xf0(uint64_t x) { return x ^ 0xf0; }

static uint32_t lsli0(uint32_t x) { return x << 0; }
static uint32_t lsri0(uint32_t x) { return x >> 0; }
static int64_t asrl0(int64_t x) { return x >> 0; }
static uint32_t lsli1(uint32_t x) { return x << 1; }
static uint32_t lsli31(uint32_t x) { return x << 31; }
static uint64_t lsll1(uint64_t x) { return x << 1; }
static uint64_t lsll63(uint64_t x) { return x << 63; }
static uint32_t lsri1(uint32_t x) { return x >> 1; }
static uint32_t lsri31(uint32_t x) { return x >> 31; }
static uint64_t lsrl1(uint64_t x) { return x >> 1; }
static uint64_t lsrl63(uint64_t x) { return x >> 63; }
static int32_t asri1(int32_t x) { return x >> 1; }
static int32_t asri31(int32_t x) { return x >> 31; }
static int64_t asrl1(int64_t x) { return x >> 1; }
static int64_t asrl63(int64_t x) { return x >> 63; }

void opi(void)
{
    int x = 1000;
    pll(addip0(x));
    pll(sublp0(x));
    pll(addip123(x));
    pll(addlm123(x));
    pll(sublp4095(x));
    pll(subim503808(x));
    pll(addp12345(x));
    pll(subp12345(x));
    pll(mvni(x));
    pll(negl(x));
    pll(rsbi123(x));
    pll(rsbl123(x));
    pll(andi0(x));
    pll(andlm1(x));
    pll(orrl0(x));
    pll(orrim1(x));
    pll(eori0(x));
    pll(eorlm1(x));
    pll(and0xf0(x));
    pll(orr0xf0(x));
    pll(eor0xf0(x));
    pll(lsli0(x));
    pll(lsri0(x));
    pll(asrl0(x));
    pll(lsli1(x));
    pll(lsli31(x));
    pll(lsll1(x));
    pll(lsll63(x));
    pll(lsri1(x));
    pll(lsri31(x));
    pll(lsrl1(x));
    pll(lsrl63(x));
    pll(asri1(x));
    pll(asri31(x));
    pll(asrl1(x));
    pll(asrl63(x));
}

void pcs(void)
{
    arg();
    ret();
    stdarg();
    movi();
    opi();
}

int main()
{
    pcs();
    return 0;
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=80 :*/
