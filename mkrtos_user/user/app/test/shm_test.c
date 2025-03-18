#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHM_NAME "/my_shm"
#define SHM_SIZE 1024 // 共享内存大小

int posix_shem_test(void)
{
    int shm_fd;
    void *ptr;

    // 1. 创建共享内存对象
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // 2. 设置共享内存大小
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // 3. 映射共享内存
    ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // 4. 写入数据到共享内存
    const char *message = "Hello, POSIX shared memory!";
    sprintf((char *)ptr, "%s", message);

    // 5. 读取共享内存中的数据
    printf("Data in shared memory: %s\n", (char *)ptr);

    // 6. 解除映射
    if (munmap(ptr, SHM_SIZE) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    // 7. 关闭共享内存对象
    close(shm_fd);

    // 8. 删除共享内存对象
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    printf("Shared memory test completed successfully.\n");
    return 0;
}

int system_v_shm_test(void)
{
    int shmid;
    key_t key = 1234; // 共享内存的键值
    char *shmaddr;

    // 1. 创建共享内存段
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // 2. 将共享内存映射到进程地址空间
    if ((shmaddr = shmat(shmid, NULL, 0)) == (char *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // 3. 写入数据到共享内存
    const char *message = "Hello, System V shared memory!";
    strncpy(shmaddr, message, SHM_SIZE);

    // 4. 读取共享内存中的数据
    printf("Data in shared memory: %s\n", shmaddr);

#if 0
    // 5. 解除共享内存映射
    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
#endif
    // 6. 删除共享内存段
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    printf("System V shared memory test completed successfully.\n");
    return 0;
}
