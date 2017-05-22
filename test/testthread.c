#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <sys/socket.h>
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libevent.h"
redisContext *redis;
static int count = 0;
void *thread_function(void *arg);
void *thread_function(void *arg)
{
    while (1)
    {
        printf("asdasda");
    }
    //pthread_exit(NULL);
}
int redisSubscribeRun()
{
    int res;
    pthread_t a_thread;
    void *thread_result;
    while (1)
    {
        res = pthread_create(&a_thread, NULL, thread_function, NULL);
        res = pthread_join(a_thread, &thread_result);
    }
    return 0;
}
static void Data_handle(void *sock_fd)
{
    int BUFFER_LENGTH = 1024;
    int fd = *((int *)sock_fd);
    int i_recvBytes;
    char data_recv[BUFFER_LENGTH];
    //const char *data_send = "Server has received your request!\n";

    while (1)
    {
        //printf("waiting for request...\n");
        //Reset data.
        memset(data_recv, 0, BUFFER_LENGTH);
        i_recvBytes = read(fd, data_recv, BUFFER_LENGTH);
        int i;
        printf("%s",data_recv);
        printf("reqdata=(");
        for (i = 0; i < i_recvBytes; i++)
        {
            printf("%02hhx ", data_recv[i]);
        }
        printf(" ) socketCount =\n");
        //write(fd, data_send, strlen(data_send));
        //saveMessage(data_recv, i_recvBytes);
        write(fd, "data_recv", i_recvBytes);

        close(fd);
        break;
    }
    
    //Clear
    printf("terminating current client_connection...\n");
    close(fd);          //close a file descriptor.
    pthread_exit(NULL); //terminate calling thread!
}
int createSocket()
{
    int sockfd_server;
    int sockfd;
    int fd_temp;
    struct sockaddr_in s_addr_in;
    struct sockaddr_in s_addr_client;
    int client_length;
    sockfd_server = socket(AF_INET, SOCK_STREAM, 0); //ipv4,TCP
    assert(sockfd_server != -1);
    memset(&s_addr_in, 0, sizeof(s_addr_in));
    s_addr_in.sin_family = AF_INET;
    s_addr_in.sin_addr.s_addr = htonl(INADDR_ANY); //trans addr from uint32_t host byte order to network byte order.
    int port = 8888;

    s_addr_in.sin_port = htons(port); //trans port from uint16_t host byte order to network byte order.
    fd_temp = bind(sockfd_server, (struct scokaddr *)(&s_addr_in), sizeof(s_addr_in));

    if (fd_temp == -1)
    {
        fprintf(stderr, "bind error!\n");
        exit(1);
    }

    fd_temp = listen(sockfd_server, 512);
    if (fd_temp == -1)
    {
        fprintf(stderr, "listen error!\n");
        exit(1);
    }

    while (1)
    {
        printf("waiting for new connection...\n");
        pthread_t thread_id;
        client_length = sizeof(s_addr_client);

        //Block here. Until server accpets a new connection.
        sockfd = accept(sockfd_server, (struct sockaddr_ *)(&s_addr_client), (socklen_t *)(&client_length));

        if (sockfd == -1)
        {
            fprintf(stderr, "Accept error!\n");
            continue; //ignore current socket ,continue while loop.
        }
        printf("A new connection occurs!\n");
        if (pthread_create(&thread_id, NULL, (void *)(&Data_handle), (void *)(&sockfd)) == -1)
        {
            fprintf(stderr, "pthread_create error!\n");
            break; //break while loop
        }
    }
    return 0;
}

int main()
{
    createSocket();
    //redisSubscribeRun();
    return 0;
}
