#pragma once
#include <u_types.h>

typedef struct pca9555
{
    uint16_t idata;
    uint16_t odata;
    uint16_t imode;
    int i2c_fd;

    uint8_t i2c_addr;     /*i2c addr*/
    const char *i2c_name; /*使用的i2c名字*/
} pca9555_t;

int pca9555_init(pca9555_t *io9555);
int pca9555_set_mode(pca9555_t *io9555, enum pca9555_io_mode mode, uint16_t val);
int pca9555_input_read(pca9555_t *io9555);
int pca9555_output_write(pca9555_t *io9555, uint16_t val);