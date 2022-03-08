#ifndef _CLIENT_SK_H_

#define _CLIENT_SK_H_
#ifdef _cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int sock_fd;

#define MAX_MSG_BUFF 1024

sock_fd init_sk(const char*,const unsigned short);
void socket_message(const sock_fd, const struct sockaddr_in);

#ifdef _cplusplus
}
#endif
#endif