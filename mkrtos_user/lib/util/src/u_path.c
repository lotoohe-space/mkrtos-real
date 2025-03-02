
#include <string.h>

static int path_name_cp(char *dst, const char *src, int src_inx, int *cp_len, int *jmp_len)
{
    int j = 0;
    int jmp_cn = 0;
    int type = -1;
    int last_is_slash = 0;

    while (*src)
    {
        if (src[0] == '.' && (src_inx == 0 || src[-1] == '/'))
        {
            if (src[1] == 0)
            {
                type = 0;
            }
            else if (src[1] == '.')
            {
                if (src[2] == '/')
                {
                    type = 1;
                }
                else if (src[2] == 0)
                {
                    type = 2;
                }
            }
            else if (src[1] == '/')
            {
                type = 3;
            }
        }
        if (type != -1)
        {
            break;
        }
        if (*src != '/' || last_is_slash == 0)
        {
            *dst = *src;
            dst++;
            j++;
        }
        if (*src == '/')
        {
            last_is_slash = 1;
        }
        else
        {
            last_is_slash = 0;
        }
        src++;
        jmp_cn++;
    }
    dst[j] = 0;
    *cp_len = j;
    *jmp_len = jmp_cn;
    return type;
}
/**
 * 相对路径转绝对路径
 * TODO: new_path可能溢出，增加长度限制
 * @param cur_path 基路径
 * @param path 相对路径
 * @param new_path 输出的路径
 * @return 输出的路径
 */
char *u_rel_path_to_abs(const char *cur_path, const char *path, char *new_path)
{
    int t_len = 0;
    if (path[0] != '/' || (path[0] == '.' && path[1] == '/'))
    {
        strcpy(new_path, cur_path);
        if (new_path[strlen(new_path) - 1] != '/')
        {
            strcat(new_path, "/");
            t_len += 1;
        }
        t_len += strlen(cur_path);
    }
    for (int i = 0; path[i];)
    {
        int cp_len;
        int jmp_len;
        int type = path_name_cp(new_path + t_len, path + i, i, &cp_len, &jmp_len);

        t_len += cp_len;
        i += jmp_len;
        switch (type)
        {
        case 0:
            i += 1;
            break;
        case 1:
            if (t_len == 0)
            {
                i += 3;
            }
            else
            {
                new_path[--t_len] = 0;
                while (t_len > 0)
                {
                    if (new_path[--t_len] == '/')
                    {
                        break;
                    }
                }
                new_path[t_len] = 0;
                i += 3;
            }
            break;
        case 2:
            if (t_len == 0)
            {
                i += 2;
            }
            else
            {
                new_path[--t_len] = 0;
                while (t_len > 0)
                {
                    if (new_path[--t_len] == '/')
                    {
                        break;
                    }
                }
                new_path[t_len] = 0;
                i += 2;
            }
            break;
        case 3:
            i += 2;
            break;
        default:
            goto end;
        }
        if (t_len == 0)
        {
            new_path[0] = '/';
            t_len = 1;
        }
    }
end:
    new_path[t_len] = 0;
    return new_path;
}
