#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include "appfs.h"
#include "hw_block_test.h"
#include "open.h"
static fs_info_t fs;
#define APPFS_FILE_NR 128

uint8_t test_data[1234];
uint8_t test_data2[1234];

int appfs_test(void)
{
    int ret;
    ret = appfs_create_file(&fs, "test.txt", 1234);
    if (ret < 0)
    {
        printf("create file failed!\n");
        return ret;
    }
    for (int i = 0; i < 1234; i++)
    {
        test_data[i] = i;
    }
    ret = appfs_write_file(&fs, "test.txt", test_data, sizeof(test_data), 0);
    if (ret < 0)
    {
        printf("write file failed!\n");
        return ret;
    }
    memset(test_data2, 0, sizeof(test_data2));
    ret = appfs_read_file(&fs, "test.txt", test_data2, sizeof(test_data2), 0);
    if (memcmp(test_data, test_data2, sizeof(test_data)) != 0)
    {
        printf("read file failed!\n");
        return ret;
    }
    ret = appfs_delete_file(&fs, "test.txt");
    assert(ret == 0);
    return ret;
}
/**
 * @brief 测试多文件
 */
int appfs_test2(void)
{
    int ret;
    char name[APPFS_FILE_NR][APPFS_FILE_NAME_MAX];
    for (int i = 0; i < 1234; i++)
    {
        test_data[i] = i;
    }
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        snprintf(name[i], APPFS_FILE_NAME_MAX, "test%d.txt", i);
        ret = appfs_create_file(&fs, name[i], 1234);
        assert(ret == 0);
        appfs_write_file(&fs, name[i], test_data, sizeof(test_data), 0);
    }
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        ret = appfs_read_file(&fs, name[rand() % APPFS_FILE_NR], test_data2, sizeof(test_data2), 0);
        assert(ret == sizeof(test_data2));
        if (memcmp(test_data, test_data2, sizeof(test_data)) != 0)
        {
            printf("read file failed!\n");
            return ret;
        }
    }

    // 删除文件
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        ret = appfs_delete_file(&fs, name[i]);
        assert(ret == 0);
    }

    return ret;
}
int appfs_test_create_mult_file_and_delete_mid_file(void)
{
    int ret;
    char name[APPFS_FILE_NR][APPFS_FILE_NAME_MAX];
    for (int i = 0; i < 1234; i++)
    {
        test_data[i] = i;
    }
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        snprintf(name[i], APPFS_FILE_NAME_MAX, "test%d.txt", i);
        ret = appfs_create_file(&fs, name[i], 1234);
        assert(ret == 0);
        appfs_write_file(&fs, name[i], test_data, sizeof(test_data), 0);
    }
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        ret = appfs_read_file(&fs, name[rand() % APPFS_FILE_NR], test_data2, sizeof(test_data2), 0);
        assert(ret == sizeof(test_data2));
        if (memcmp(test_data, test_data2, sizeof(test_data)) != 0)
        {
            printf("read file failed!\n");
            return ret;
        }
    }
    // 删除部分文件
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        if (i % 2 == 0)
        {
            ret = appfs_delete_file(&fs, name[i]);
            assert(ret == 0);
        }
    }
    // 再次读取文件
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        if (i % 2 == 1)
        {
            ret = appfs_read_file(&fs, name[i], test_data2, sizeof(test_data2), 0);
            assert(ret == sizeof(test_data2));
            if (memcmp(test_data, test_data2, sizeof(test_data)) != 0)
            {
                printf("read file failed!\n");
                return ret;
            }
        }
    }
    // 删除剩余文件
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        if (i % 2 == 1)
        {
            ret = appfs_delete_file(&fs, name[i]);
            assert(ret == 0);
        }
    }
    return 0;
}
/**
 * @brief 创建多个文件并排列文件
 *
 */
int appfs_create_mult_file_and_arrange_file(void)
{
    int ret;
    char name[APPFS_FILE_NR][APPFS_FILE_NAME_MAX];
    for (int i = 0; i < 1234; i++)
    {
        test_data[i] = i;
    }
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        snprintf(name[i], APPFS_FILE_NAME_MAX, "test%d.txt", i);
        ret = appfs_create_file(&fs, name[i], 1234);
        assert(ret == 0);
        appfs_write_file(&fs, name[i], test_data, sizeof(test_data), 0);
    }
    // 删除部分文件
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        if (i % 2 == 0)
        {
            ret = appfs_delete_file(&fs, name[i]);
            assert(ret == 0);
        }
    }
    // 整理文件
    ret = appfs_arrange_files(&fs);
    assert(ret == 0);
    // 读取文件并测试
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        if (i % 2 == 1)
        {
            ret = appfs_read_file(&fs, name[i], test_data2, sizeof(test_data2), 0);
            assert(ret == sizeof(test_data2));
            if (memcmp(test_data, test_data2, sizeof(test_data)) != 0)
            {
                printf("read file failed!\n");
                return ret;
            }
        }
    }
    // 删除剩余文件
    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        if (i % 2 == 1)
        {
            ret = appfs_delete_file(&fs, name[i]);
            assert(ret == 0);
        }
    }
    return 0;
}
void appfs_test_offset_write(void)
{
    int ret;
    char name[APPFS_FILE_NAME_MAX];

    memset(test_data, 0xff, sizeof(test_data));
    strcpy(name, "test0.txt");
    ret = appfs_create_file(&fs, name, 1234);
    assert(ret == 0);
    ret = appfs_write_file(&fs, name, test_data, sizeof(test_data), 0);
    assert(ret == sizeof(test_data));
    memset(test_data, 0x00, 512 + 1);
    ret = appfs_write_file(&fs, name, test_data, sizeof(test_data), 513);
    assert(ret == (ROUND_UP(sizeof(test_data), fs.save.block_size) * fs.save.block_size - (512 + 1)));

    memset(test_data, 0xff, sizeof(test_data));
    ret = appfs_write_file(&fs, name, test_data, sizeof(test_data), 0);
    assert(ret == sizeof(test_data));

    memset(test_data + 513, 0x00, 512);
    ret = appfs_write_file(&fs, name, test_data + 513, 512, 513);
    assert(ret == (512));

    ret = appfs_read_file(&fs, name, test_data2, sizeof(test_data), 0);
    assert(ret >= 0);
    if (memcmp(test_data, test_data2, sizeof(test_data)) != 0)
    {
        assert(0);
    }
    ret = appfs_read_file(&fs, name, test_data2, 512, 513);
    assert(ret >= 0);
    for (int i = 0; i < 512; i++)
    {
        if (test_data2[i] != 0x00)
        {
            assert(0);
        }
    }

    for (int i = 0; i < 32; i++)
    {
        int write_inx;

        write_inx = rand() % sizeof(test_data);
        ret = appfs_write_file(&fs, name, "1", 1, write_inx);
        assert(ret == 1);
        char read_char;
        ret = appfs_read_file(&fs, name, &read_char, 1, write_inx);
        assert(ret == 1);
        assert(read_char == '1');
    }

    appfs_delete_file(&fs, name);
}

void appfs_posix_test(void)
{
    int fd;
    int ret;
    char file_name[32];

    ret = appfs_open_init(&fs);
    assert(ret >= 0);

    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        snprintf(file_name, sizeof(file_name), "%d.bin", i);

        // printf("%d\n", i);
        fd = appfs_open(file_name, O_CREAT, 0777);
        assert(fd >= 0);
        ret = appfs_truncate(fd, sizeof(test_data));
        assert(ret >= 0);
        memset(test_data, 0x11, sizeof(test_data));
        ret = appfs_write(fd, test_data, sizeof(test_data));
        assert(ret == sizeof(test_data));
        ret = appfs_lseek(fd, 0, SEEK_SET);
        assert(ret >= 0);
        ret = appfs_read(fd, test_data2, sizeof(test_data2));
        assert(ret == sizeof(test_data2));
        assert(memcmp(test_data2, test_data, sizeof(test_data)) == 0);
        appfs_close(fd);
    }

    fd = appfs_open("32.bin", O_CREAT, 0777);
    assert(fd <= 0);

    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        snprintf(file_name, sizeof(file_name), "%d.bin", i);

        appfs_remove(file_name);
    }
}
void appfs_posix_readdir_test(void)
{
    int fd;
    int ret;
    char file_name[32];

    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        snprintf(file_name, sizeof(file_name), "%d.bin", i);

        // printf("%d\n", i);
        fd = appfs_open(file_name, O_CREAT, 0777);
        assert(fd >= 0);
        ret = appfs_truncate(fd, sizeof(test_data));
        assert(ret >= 0);
        memset(test_data, 0x11, sizeof(test_data));
        ret = appfs_write(fd, test_data, sizeof(test_data));
        assert(ret == sizeof(test_data));
        ret = appfs_lseek(fd, 0, SEEK_SET);
        assert(ret >= 0);
        ret = appfs_read(fd, test_data2, sizeof(test_data2));
        assert(ret == sizeof(test_data2));
        assert(memcmp(test_data2, test_data, sizeof(test_data)) == 0);
        appfs_close(fd);
    }

    int dir_fd = appfs_open("", O_RDWR, 0777);

    assert(dir_fd >= 0);
    struct dirent _dirent;
    while ((ret = appfs_readdir(dir_fd, &_dirent)) >= 0)
    {
        printf("%s\n", _dirent.d_name);
    }
    printf("\n");
    assert(appfs_lseek(dir_fd, 0, SEEK_SET) >= 0);
    while ((ret = appfs_readdir(dir_fd, &_dirent)) >= 0)
    {
        printf("%s\n", _dirent.d_name);
    }
    printf("\n");
    assert(appfs_lseek(dir_fd, 120, SEEK_SET) >= 0);
    while ((ret = appfs_readdir(dir_fd, &_dirent)) >= 0)
    {
        printf("%s\n", _dirent.d_name);
    }
    appfs_close(dir_fd);

    for (int i = 0; i < APPFS_FILE_NR; i++)
    {
        snprintf(file_name, sizeof(file_name), "%d.bin", i);

        appfs_remove(file_name);
    }
}
void appfs_test_func(void)
{
    int ret;
    printf("appfs testing.\n\n");
    hw_init_block_test(&fs);
    appfs_format(&fs, APPFS_FILE_NR);
    appfs_init(&fs);
    appfs_test_offset_write();

    appfs_print_files(&fs);
    appfs_test();
    appfs_print_files(&fs);
    appfs_test2();
    appfs_print_files(&fs);
    appfs_test_create_mult_file_and_delete_mid_file();
    appfs_print_files(&fs);
    appfs_create_mult_file_and_arrange_file();
    appfs_print_files(&fs);
    appfs_posix_test();
    appfs_print_files(&fs);
    appfs_posix_readdir_test();
    appfs_print_files(&fs);
}