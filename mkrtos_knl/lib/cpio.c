// #include "mk_sys.h"
#include "cpio.h"
#include <util.h>
#include <string.h>
#include <arch.h>
// #define MIN(a, b) ((a) < (b) ? (a) : (b))							   //!< 最小值
// #define ALIGN(mem, align) (((mem) + ((align) - 1)) & (~((align) - 1))) //!< 向上对齐
static int htoi(char *str, int len);

umword_t cpio_get_size(umword_t st)
{
	cpio_fs_t *file_info;
	umword_t i;

	for (i = st; st < (umword_t)(-1);)
	{
		file_info = (cpio_fs_t *)i;

		if (check_magic((char *)file_info) < 0)
		{
			return 0;
		}
		int name_size = htoi(file_info->c_namesize, 8);

		const char *f_name = (char *)(i + sizeof(cpio_fs_t));

		if (strcmp("TRAILER!!!", f_name) == 0)
		{
			break;
		}
		int file_size = htoi(file_info->c_filesize, 8);

		i = ALIGN(ALIGN(i + sizeof(cpio_fs_t) + name_size, 4) +
					  file_size,
				  4);
	}

	return ALIGN(i, PAGE_SIZE) - st;
}
umword_t cpio_find_file(umword_t st, umword_t en, const char *name, size_t *size)
{
	uint8_t rByte;
	int32_t bk_inx;
	cpio_fs_t *file_info;

	for (umword_t i = st; st < en;)
	{
		file_info = (cpio_fs_t *)i;

		if (check_magic((char *)file_info) < 0)
		{
			return 0;
		}
		int name_size = htoi(file_info->c_namesize, 8);
		const char *f_name = (char *)(i + sizeof(cpio_fs_t));
		if (strcmp(f_name, name) == 0)
		{
			if (size)
			{
				*size = htoi(file_info->c_filesize, 8);
			}
			return (mword_t)(ALIGN(i + sizeof(cpio_fs_t) + name_size, 4));
		}

		if (strcmp("TRAILER!!!", f_name) == 0)
		{
			return 0;
		}
		int file_size = htoi(file_info->c_filesize, 8);

		i = ALIGN(ALIGN(i + sizeof(cpio_fs_t) + name_size, 4) +
					  file_size,
				  4);
	}
	return 0;
}

static int htoi(char *str, int len)
{
	int n = 0;
	int i = 0;

	if (str == NULL || len <= 0)
		return -1;
	for (i = 0; i < len; i++)
	{
		if (str[i] != '0')
		{
			break;
		}
	}
	str += i;
	while (1)
	{
		if (*str >= '0' && *str <= '9')
		{
			n = 16 * n + (*str - '0');
		}
		else if (*str >= 'A' && *str <= 'F') // 十六进制还要判断字符是不是在A-F或者a-f之间
		{
			n = 16 * n + (*str - 'A' + 10);
		}
		else if (*str >= 'a' && *str <= 'f')
		{
			n = 16 * n + (*str - 'a' + 10);
		}
		else
		{
			break;
		}
		str++;
		i++;
		if (i >= len)
		{
			break;
		}
	}
	return n;
}
