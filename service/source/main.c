#include "../include/service_sk.h"

int main(int argc, char *argv[])
{
    sock_fd fd = init_sk();
    if(-1 == fd) exit(-1);
    socket_message(fd);

    close(fd);
    return 0;
}