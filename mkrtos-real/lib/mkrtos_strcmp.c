/*
 * mkrtos_strcmp.c
 *
 *  Created on: 2022��8��6��
 *      Author: Administrator
 */

int mkrtos_strcmp (const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
        s1++, s2++;
    return (*s1 - *s2);
}
