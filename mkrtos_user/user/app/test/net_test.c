#include "pthread.h"
#include "unistd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 128

static int tcp_server(void)
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    // 创建服务器套接字
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("create server socket is success:%d\n", server_socket);
    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 绑定套接字到本地地址和端口
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // 监听连接请求
    if (listen(server_socket, 3) < 0)
    {
        perror("listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    socklen_t acc_len;
    // 接收客户端连接
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &acc_len);
    if (client_socket < 0)
    {
        perror("accept failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Client connected!\n");

    while (1)
    {
        ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0)
        {
            perror("recv failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }
        #if 0
        printf("svr recv: %s, len:%d\n", buffer, bytes_received);
        #endif
        send(client_socket, buffer, bytes_received, 0);
    }

    // 关闭套接字
    close(client_socket);
    close(server_socket);

    return 0;
}
static void *net_server_func(void *arg)
{
    tcp_server();
    return NULL;
}

static int tcp_client(void)
{
    int server_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 创建服务器套接字
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 使用本机IP地址
    server_addr.sin_port = htons(PORT);

    // 连接到服务器
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server!\n");

    while (1)
    {
        // 发送数据
        const char *message = "Hello from client!";
        send(server_socket, message, strlen(message), 0);

        // 接收数据
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_received = recv(server_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0)
        {
            perror("recv failed");
            close(server_socket);
            exit(EXIT_FAILURE);
        }
        #if 0
        printf("client recv: %s, len:%d\n", buffer, bytes_received);
        #endif
        usleep(1000);
    }

    // 关闭套接字
    close(server_socket);

    return 0;
}
static pthread_t net_server_pt;
int net_test(void)
{
    if (pthread_create(&net_server_pt, NULL, net_server_func, NULL))
    {
        printf("net server pthread create failed.\n");
        return -1;
    }
    sleep(1);
    tcp_client();
    pthread_join(net_server_pt, NULL);
    return 0;
}