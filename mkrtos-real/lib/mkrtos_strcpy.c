/*
 * mkrtos_strcpy.c
 *
 *  Created on: 2022��8��6��
 *      Author: Administrator
 */

char * mkrtos_strcpy (register char *s1, register const char *s2)
{
    register char *res = s1;
    while ((*s1++ = *s2++));
    return (res);
}
