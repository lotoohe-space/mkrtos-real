#pragma once

void u_rel_path_to_abs(const char *cur_path, const char *path, char *new_path);
static inline bool_t u_is_root_path(const char *path)
{
    return path[0] == '/' && path[1] == '\0';
}