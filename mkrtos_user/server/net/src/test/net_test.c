#include "pthread.h"
#include "unistd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <lwip/sockets.h>
#define PORT 8080
#define BUFFER_SIZE 128

static int tcp_server(void)
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    // 创建服务器套接字
    if ((server_socket = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("lwip_socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("create server lwip_socket is success:%d\n", server_socket);
    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // 绑定套接字到本地地址和端口
    if (lwip_bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("lwip_bind failed");
        lwip_close(server_socket);
        exit(EXIT_FAILURE);
    }

    // 监听连接请求
    if (lwip_listen(server_socket, 3) < 0)
    {
        perror("lwip_listen failed");
        lwip_close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    socklen_t acc_len;
    // 接收客户端连接
    client_socket = lwip_accept(server_socket, (struct sockaddr *)&client_addr, &acc_len);
    if (client_socket < 0)
    {
        perror("lwip_accept failed");
        lwip_close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Client connected!\n");

    // 发送数据
    const char *message = "Hello from server!";
    lwip_send(client_socket, message, strlen(message), 0);

    // 接收数据
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_received = lwip_recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0)
    {
        perror("lwip_recv failed");
        lwip_close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Received message from client: %s\n", buffer);

    // 关闭套接字
    lwip_close(client_socket);
    lwip_close(server_socket);

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
    if ((server_socket = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("lwip_socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 使用本机IP地址
    server_addr.sin_port = htons(PORT);

    // 连接到服务器
    if (lwip_connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        lwip_close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server!\n");

    // 发送数据
    const char *message = "Hello from client!";
    lwip_send(server_socket, message, strlen(message), 0);

    // 接收数据
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_received = lwip_recv(server_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0)
    {
        perror("lwip_recv failed");
        lwip_close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Received message from server: %s\n", buffer);

    // 关闭套接字
    lwip_close(server_socket);

    return 0;
}
static void *net_client_func(void *arg)
{
    tcp_client();
    return NULL;
}

static pthread_t net_server_pt;
static pthread_t net_client_pt;
int net_test(void)
{
    sleep(1);
    if (pthread_create(&net_server_pt, NULL, net_server_func, NULL))
    {
        printf("net server pthread create failed.\n");
        return -1;
    }
    sleep(1);
    if (pthread_create(&net_server_pt, NULL, net_client_func, NULL))
    {
        printf("net server pthread create failed.\n");
        return -1;
    }
    return 0;
}