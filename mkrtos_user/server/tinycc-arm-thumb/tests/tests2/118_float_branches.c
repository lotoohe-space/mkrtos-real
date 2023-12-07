/*
 * The other tests does not seem to trigger all branches in arm-thumb-gen that
 * write instructions. This one will (for float code).
 */

#include <stdarg.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// Printer
///////////////////////////////////////////////////////////////////////////////
void print_double (double d)
{
	// Print a double without printf supporting it
	int i = d;
	int f = d * 1000.0;
	f = f % 1000;
	f = f < 0 ? -f : f;
	printf ("%i.%.3i\n", i, f);
}

///////////////////////////////////////////////////////////////////////////////
// Test arguments
///////////////////////////////////////////////////////////////////////////////
float test_floatarg (float a) {return a;}
double test_doublearg (double a) {return a;}
float test_floatarg2 (float a, float b) {return a;}
double test_doublearg2 (double a, double b) {return a;}
float test_floatarg3 (float a, float b, float c) {return a;}
double test_doublearg3 (double a, double b, double c) {return a;}
float test_floatarg4 (float a, float b, float c, float d) {return a;}
double test_doublearg4 (double a, double b, double c, double d) {return a;}
float test_floatarg5 (float a, float b, float c, float d, float e) {return a;}
double test_doublearg5 (double a, double b, double c, double d, double e) {return a;}

double test_doubleargn (
	double a, double b, double c, double d,
	double e, double f, double g, double h,
	double i, double j, double k, double l,
	double m, double n, double o, double p) {return p;}

void run_args (void)
{
	printf ("Test args\n");
	print_double (test_floatarg (0.1f));
	print_double (test_doublearg (0.1));
	print_double (test_floatarg2 (0.2f, 1.2f));
	print_double (test_doublearg2 (0.2, 1.2));
	print_double (test_floatarg3 (0.3f, 2.3f, 3.3f));
	print_double (test_doublearg3 (0.3, 1.3, 2.3));
	print_double (test_floatarg4 (0.4f, 2.4f, 3.4f, 4.4f));
	print_double (test_doublearg4 (0.4, 2.4, 3.4, 4.4));
	print_double (test_floatarg5 (0.5f, 2.5f, 3.5f, 4.5f, 5.5f));
	print_double (test_doublearg5 (0.5, 2.5, 3.5, 4.5, 5.5));
}

///////////////////////////////////////////////////////////////////////////////
// Test misc
///////////////////////////////////////////////////////////////////////////////
float test_store (float a)
{
	float b = a;
	return b;
}

float ret_const (void) {return 42.3f;}
float call (float f) {return test_store (f);}

void run_misc (void)
{
	printf ("Test misc\n");
	print_double (test_store (13.37f));
	print_double (ret_const ());
	print_double (call (32.0f));
}

///////////////////////////////////////////////////////////////////////////////
// Test conversion
///////////////////////////////////////////////////////////////////////////////
float ftof_d (double d) {return d;}
double ftof_f (float d) {return d;}

int ftoi_d (double d) {return d;}
int ftoi_f (float d) {return d;}
unsigned int ftoi_ud (double d) {return d;}
unsigned int ftoi_uf (float d) {return d;}
unsigned short int ftoi_sud (double d) {return d;}
unsigned short int ftoi_suf (float d) {return d;}

float itof_i (int i) {return i;}
float itof_ui (unsigned int i) {return i;}
double itof_di (int i) {return i;}
double itof_dui (unsigned int i) {return i;}

void run_conversion (void)
{
	printf ("Test conversion\n");
	print_double (ftof_d (1.1));
	print_double (ftof_f (2.2f));

	printf ("%i\n", ftoi_d (3.3));
	printf ("%i\n", ftoi_f (4.4f));
	printf ("%i\n", ftoi_ud (5.5));
	printf ("%i\n", ftoi_uf (6.6f));
	printf ("%i\n", ftoi_sud (7.7));
	printf ("%i\n", ftoi_suf (8.8f));

	print_double (itof_i (-100));
	print_double (itof_ui (200));
	print_double (itof_di (-300));
	print_double (itof_dui (400));
}

///////////////////////////////////////////////////////////////////////////////
// Test operations
///////////////////////////////////////////////////////////////////////////////
float add (float a, float b) {return a + b;}
float sub (float a, float b) {return a - b;}
float mul (float a, float b) {return a * b;}
float div (float a, float b) {return a / b;}

double add_d (double a, double b) {return a + b;}
double sub_d (double a, double b) {return a - b;}
double mul_d (double a, double b) {return a * b;}
double div_d (double a, double b) {return a / b;}

float fneg (float a, float b) {return -a;}
double fneg_d (double a, double b) {return -a;}

float cmp_lt (float a, float b) {if (a < b) return a; else return b;}
float cmp_le (float a, float b) {if (a <= b) return a; else return b;}
float cmp_gt (float a, float b) {if (a > b) return a; else return b;}
float cmp_ge (float a, float b) {if (a >= b) return a; else return b;}

void run_oper (void)
{
	printf ("Test oper\n");
	print_double (add (0.5f, 0.5f));
	print_double (sub (0.5f, 0.5f));
	print_double (mul (0.5f, 0.5f));
	print_double (div (0.5f, 0.5f));

	print_double (add_d (0.5, 0.5));
	print_double (sub_d (0.5, 0.5));
	print_double (mul_d (0.5, 0.5));
	print_double (div_d (0.5, 0.5));

	print_double (fneg (50.5f, 50.6f));
	print_double (fneg_d (50.5, 50.6));

	print_double (cmp_lt (0.5f, 0.8f));
	print_double (cmp_le (0.5f, 0.8f));
	print_double (cmp_gt (0.5f, 0.8f));
	print_double (cmp_ge (0.5f, 0.8f));
}

///////////////////////////////////////////////////////////////////////////////
// Test calls
///////////////////////////////////////////////////////////////////////////////
struct st {
	float a;
};

double printf2 (const char *fmt, ...)
{
	va_list list;
	va_start (list, fmt);

	double a = va_arg (list, double);
	print_double (a);
	va_end (list);

	return a;
}

void eatstruct (struct st a)
{
	print_double (a.a);
}

void call_a (float a)
{
	printf2 ("%f\n", a);
}

void call_b (struct st a)
{
	eatstruct (a);
}

void call_c (float a)
{
	printf2 ("%f\n", a);
}

void call_d (float a, int b, double c, long long l)
{
	double aa = 42.5;
	float ab = 13.37;
	printf2 ("", a, b, c, l, aa, ab);
}

struct stt {
	float f;
	float a;
	float b;
	float c;
};

void eatstruct2 (struct stt s, float f, char c, double d, float a, float e)
{
	print_double (s.f);
	print_double (s.a);
	print_double (s.b);
	print_double (s.c);
	print_double (f);
	print_double (c);
	print_double (d);
	print_double (a);
	print_double (e);
}

double call_e (float c, struct stt a)
{
	struct stt b;
	b.f = 1.1f;
	b.a = 2.2f;
	b.b = 3.3f;
	b.c = 4.4f;
	eatstruct2 (b, c, 'a', 8.0, 9, 0);
	return 0;
}

void run_calls (void)
{
	printf ("Test calls\n");
	call_a (55.5f);
	struct st a;
	a.a = 123.4f;
	call_b (a);
	call_c (66.6f);
	call_d (0.1f, 2, 2.2, 123456789);
	struct stt b;
	b.f = 5.5f;
	b.a = 6.6f;
	b.b = 7.7f;
	b.c = 8.8f;
	call_e (9.9f, b);
}

///////////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
	run_args ();
	run_misc ();
	run_conversion ();
	run_oper ();
	run_calls ();
	return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=80 :*/
