
#include "u_types.h"
#include "u_queue.h"
#include <stdio.h>

void print_hex(char *buffer, int len)
{
    int i;
    for (i = 1; i <= len; i++)
    {
        printf("0x%02X ", buffer[i - 1]);
        // if (i % 16 == 0)
        // {
        //     printf("\n");
        // }
    }
    printf("\n");
}
void print_q_hex(queue_t *q)
{
    int i;
    for (i = 1; !q_empty(q); i++)
    {
        uint8_t d;

        q_dequeue(q, &d);
        printf("0x%02X ", d);
        // if (i % 16 == 0)
        // {
        //     printf("\n");
        // }
    }
    printf("\n");
}