#include "appfs.h"
#include "hw_block_sim.h"
#include "libgen.h"
#include "test.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define APPFS_FILE_NR 128

static fs_info_t fs;
static char *pack_path;
static char *output_path;

// 比较函数，用于 qsort 排序
int compare(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}
static char **file_get_all_sort(const char *file_path, int *ret_size)
{
    DIR *dir;
    struct dirent *entry;
    char **filenames = NULL;
    int count = 0;

    assert(ret_size);
    // 打开目录
    dir = opendir(file_path);
    if (dir == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
        return NULL;
    }

    // 读取目录中的文件
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        { // 只处理普通文件
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", file_path, entry->d_name);
            filenames = realloc(filenames, (count + 1) * sizeof(char *));
            filenames[count] = strdup(full_path);
            count++;
        }
    }

    // 关闭目录
    closedir(dir);

    // 对文件名进行排序
    qsort(filenames, count, sizeof(char *), compare);

    *ret_size = count;
    return filenames;
}

static int read_file_all(const char *file_path, void **ret_ptr)
{
    FILE *file = fopen(file_path, "rb"); // 以二进制模式打开文件
    if (!file)
    {
        perror("fopen");
        return EXIT_FAILURE;
    }

    // 寻求文件末尾以确定文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0)
    {
        perror("ftell");
        fclose(file);
        return EXIT_FAILURE;
    }

    // 分配内存来存储文件内容
    char *content = (char *)malloc((size_t)file_size);
    if (!content)
    {
        perror("malloc");
        fclose(file);
        return EXIT_FAILURE;
    }

    // 读取文件内容到内存中
    size_t bytes_read = fread(content, 1, (size_t)file_size, file);
    if (bytes_read != (size_t)file_size)
    {
        perror("fread");
        free(content);
        fclose(file);
        return EXIT_FAILURE;
    }
    *ret_ptr = content;
    fclose(file);
    return file_size;
}

int read_files_and_write_appfs(const char *path)
{
    struct stat info;
    int ret;
    long total_size = 0;
    int files_count;
    char **files = file_get_all_sort(path, &files_count);

    if (files == NULL)
    {
        return -ENOENT;
    }
    for (int i = 0; i < files_count; i++)
    {
        // 获取文件信息
        if (stat(files[i], &info) != 0)
        {
            printf("pack fail:%s\n", files[i]);
            continue;
        }
        char *file_name = basename(files[i]);

        if (S_ISREG(info.st_mode))
        {
            printf("%s %fMB\n", files[i], (float)info.st_size / 1024.0f / 1024.0f);
            ret = appfs_create_file(&fs, file_name, info.st_size);
            if (ret < 0)
            {
                printf("create file error.\n");
                return -1;
            }
            void *file_ptr;
            ret = read_file_all(files[i], &file_ptr);
            if (ret < 0)
            {
                printf("read file error.\n");
                return ret;
            }
            ret = appfs_write_file(&fs, file_name, file_ptr, info.st_size, 0);
            if (ret < 0)
            {
                free(file_ptr);
                printf("write file error.\n");
                return ret;
            }
            free(file_ptr);
            total_size += info.st_size;
        }
    }
    printf("file total size:%fMB\n", ((float)total_size / 1024.0f / 1024.0f));
    return 0;
}
static char *debug_img_path;
static int gen_img_size = -1;
static int gen_img_blk_size = 4096;
static int slight_mode = 0; //! 瘦模式
static int appfs_gen_init(void)
{
    int ret;

    ret = hw_init_block_sim_test(&fs, gen_img_size, gen_img_blk_size);
    if (ret < 0)
    {
        return ret;
    }
    if (debug_img_path == NULL)
    {
        appfs_format(&fs, APPFS_FILE_NR);
    }
    ret = appfs_init(&fs);
    return ret;
}
int main(int argc, char *argv[])
{
    int opt;
    int ret = 0;
    // appfs_test_func();

    while ((opt = getopt(argc, argv, "jti:g:o:s:b:")) != -1)
    {
        switch (opt)
        {
        case 't':
            printf("test mode.\n");
            appfs_test_func();
            return 0;
        case 'j':
            slight_mode = 1;
            break;
        case 'i':
            printf("input img:%s\n", optarg);
            debug_img_path = optarg;
            break;
        case 'g':
            printf("package path is %s\n", optarg);
            pack_path = optarg;
            break;
        case 'o':
            printf("output file is %s\n", optarg);
            output_path = optarg;
            break;
        case 's':
            gen_img_size = atol(optarg);
            printf("img size is %d.\n", gen_img_size);
            break;
        case 'b':
            gen_img_blk_size = atol(optarg);
            printf("img blk size is %d.\n", gen_img_blk_size);
            break;
        default:
            printf("Unknown option\n");
            break;
        }
    }
    if (!pack_path)
    {
        printf("pack patch is not set.\n");
        return -1;
    }
    if (gen_img_size < 0)
    {
        printf("output img size don't set.\n");
        return -1;
    }
    if (output_path == NULL)
    {
        output_path = "a.img";
    }
    if (appfs_gen_init() < 0)
    {
        printf("init fail.\n");
        return -1;
    }

    ret = read_files_and_write_appfs(pack_path);
    if (ret < 0)
    {
        printf("appfs create failed\n");
        return ret;
    }
    ret = hw_dump_to_file(&fs, output_path, slight_mode);
    if (ret < 0)
    {
        printf("appfs dump failed\n");
        return ret;
    }
    return 0;
}
