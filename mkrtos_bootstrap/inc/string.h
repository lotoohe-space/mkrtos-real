#pragma once

static inline void memset(void *data, int val, int size)
{
    unsigned int *_d = data;

    for (int i = 0; i < size; i++)
    {
        _d[0] = 0;
    }
}
