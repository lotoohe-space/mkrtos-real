
#include <types.h>
typedef double double_t;


#ifndef fp_force_evalf
#define fp_force_evalf fp_force_evalf
static inline void fp_force_evalf(float x)
{
	volatile float y;
	y = x;
}
#endif

#ifndef fp_force_eval
#define fp_force_eval fp_force_eval
static inline void fp_force_eval(double x)
{
	volatile double y;
	y = x;
}
#endif

#ifndef fp_force_evall
#define fp_force_evall fp_force_evall
static inline void fp_force_evall(long double x)
{
	volatile long double y;
	y = x;
}
#endif
#define FORCE_EVAL(x) do {                        \
	if (sizeof(x) == sizeof(float)) {         \
		fp_force_evalf(x);                \
	} else if (sizeof(x) == sizeof(double)) { \
		fp_force_eval(x);                 \
	} else {                                  \
		fp_force_evall(x);                \
	}                                         \
} while(0)

#define DBL_TRUE_MIN 4.94065645841246544177e-324
#define DBL_MIN 2.22507385850720138309e-308
#define DBL_MAX 1.79769313486231570815e+308
#define DBL_EPSILON 2.22044604925031308085e-16

#if FLT_EVAL_METHOD == 0 || FLT_EVAL_METHOD == 1
#define EPS DBL_EPSILON
#elif FLT_EVAL_METHOD == 2
#define EPS LDBL_EPSILON
#endif
static const double_t toint = 1 / EPS;

double ceil(double x)
{
    union
    {
        double f;
        uint64_t i;
    } u = {x};
    int e = u.i >> 52 & 0x7ff;
    double_t y;

    if (e >= 0x3ff + 52 || x == 0)
        return x;
    /* y = int(x) - x, where int(x) is an integer neighbor of x */
    if (u.i >> 63)
        y = x - toint + toint - x;
    else
        y = x + toint - toint - x;
    /* special case because of non-nearest rounding modes */
    if (e <= 0x3ff - 1)
    {
        FORCE_EVAL(y);
        return u.i >> 63 ? -0.0 : 1;
    }
    if (y < 0)
        return x + y + 1;
    return x + y;
}
