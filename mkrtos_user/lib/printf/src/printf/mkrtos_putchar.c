
#include "printf.h"
#include "cons_cli.h"
void putchar_(char character)
{
    cons_write((const uint8_t *)&character, 1);
}
