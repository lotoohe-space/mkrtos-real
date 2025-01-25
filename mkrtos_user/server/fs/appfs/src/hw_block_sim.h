#pragma once
#include "appfs.h"
int hw_dump_to_file(fs_info_t *fs, const char *name, int mode);
int hw_init_block_sim_test(fs_info_t *fs, int img_size, int block_size);
int hw_read_file_all(const char *file_path, void **ret_ptr);
int hw_offload_img_from_file(const char *name);
