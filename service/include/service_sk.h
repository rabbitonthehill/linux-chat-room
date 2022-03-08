#ifndef _SERVICE_SK_H_

#define _SERVICE_SK_H_

#ifdef _cplusplus
extern "C"{
#endif

typedef int sock_fd;

#define MAX_BUFF_LEN 1024
#define LISTEN_PORT 8018
#define LISTEN_BACKLOG 5
#define MAX_EPOLL_EVENTS 120

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

sock_fd init_sk(void);
static void add_default_event(const sock_fd, int*);
static void accept_new_socket(const sock_fd, const int);
static void close_socket(const sock_fd, const int, const struct sockaddr_in);
static void handle_message(const sock_fd, const int);
static void set_no_block(const sock_fd);
void socket_message(const sock_fd);

#ifdef _cplusplus
}
#endif

#endif