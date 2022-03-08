#include "../include/client_sk.h"

int main(int argc, char *argv[])
{
    if(3 > argc)
    {
        perror("Too few parameters,please at least pass in IP address and port!\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in send_addr;
    memset(&send_addr, 0, sizeof(struct sockaddr_in));
    send_addr.sin_family = AF_INET;
    send_addr.sin_addr.s_addr = inet_addr(argv[1]);
    send_addr.sin_port = htons((unsigned short)atoi(argv[2]));
    sock_fd fd = init_sk(argv[1], (unsigned short)atoi(argv[2]));
    if(-1 == fd)
    {
        exit(EXIT_FAILURE);
    }
    socket_message(fd, send_addr);

    close(fd);
    return 0;
}