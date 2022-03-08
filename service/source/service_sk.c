#include "../include/service_sk.h"

/*****************************************************************************
*   Prototype    : init_sk
*   Description  : init socket/create、bind、listen socket
*   Input        : void  
*   Output       : None
*   Return Value : sock_fd
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2019/8/29
*           Author       : Ydeer
*           Modification : Created function
*
*****************************************************************************/
sock_fd init_sk(void)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    sock_fd fd = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > fd)
    {
        perror("create socket fail!\n");

        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_port = htons(LISTEN_PORT);
    if(0 > bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)))
    {
        perror("bind socket fail!\n");
    
        return -1;
    }
    if(0 > listen(fd, LISTEN_BACKLOG))
    {
        perror("listen request fail!\n");
    
        return -1;
    }

    return fd;
}

/*****************************************************************************
*   Prototype    : add_default_event
*   Description  : add default event
*   Input        : const sock_fd fd  listen socket fd
*                  int *ep_fd        set epoll_fd
*   Output       : None
*   Return Value : static void
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2019/8/29
*           Author       : Ydeer
*           Modification : Created function
*
*****************************************************************************/
static void add_default_event(const sock_fd fd, int *ep_fd)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    int epoll_fd = epoll_create(512);
    if(-1 == epoll_fd)
    {
        perror("epoll_create fail!\n");
        exit(EXIT_FAILURE);
    }
    /*ev.events = EPOLLIN | EPOLLPRI;
    ev.data.fd = 0;
    if(-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &ev))
    {
        perror("add stdin event error!\n");
        exit(EXIT_FAILURE);
    }*/
    ev.events = EPOLLIN | EPOLLPRI;
    ev.data.fd = fd;
    if(-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev))
    {
        perror("add listen socket error!\n");
        exit(EXIT_FAILURE);
    }
    *ep_fd = epoll_fd;
}

/*****************************************************************************
*   Prototype    : accept_new_socket
*   Description  : accept client new socket request
*   Input        : const sock_fd fd    listen socket fd
*                  const int epoll_fd  add epoll events fd
*   Output       : None
*   Return Value : static void
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2019/8/29
*           Author       : Ydeer
*           Modification : Created function
*
*****************************************************************************/
static void accept_new_socket(const sock_fd fd, const int epoll_fd)
{
    struct sockaddr_in addr;
    int size = sizeof(addr);
    memset(&addr, 0, size);
    int new_socket = accept(fd, (struct sockaddr *)&addr, &size);
    if(-1 == new_socket)
    {
        perror("accept connect fail!\n");
        exit(EXIT_FAILURE);
    }
    printf("received connection requests from %s:%d......\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    set_no_block(new_socket);
    struct epoll_event new_ev;
    memset(&new_ev, 0, sizeof(new_ev));
    new_ev.events = EPOLLIN | EPOLLET;
    new_ev.data.fd = new_socket;
    if(-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &new_ev))
    {
        close(new_socket);//如果添加事件失败了则删除这个新连接的socket句柄
        perror("epoll_ctl: add accept socket event fail!\n");
        exit(EXIT_FAILURE);
    }
}

/*****************************************************************************
*   Prototype    : close_socket
*   Description  : close client connect socket
*   Input        : const sock_fd fd                 listen socket fd             
*                  const int epoll_fd               epoll fd
*                  const struct sockaddr_in addr    client addr
*   Output       : None
*   Return Value : static void
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2019/9/10
*           Author       : Ydeer
*           Modification : Created function
*
*****************************************************************************/
static void close_socket(const sock_fd fd, const int epoll_fd, const struct sockaddr_in addr)
{
    if(-1 == epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL))
    {
        char *err;
        memset(err, 0, 4);
        sprintf(err, "Remove failed connection from %s:%d!\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        perror(err);
        exit(EXIT_FAILURE);
    }
    close(fd);
}


/*****************************************************************************
*   Prototype    : set_no_block
*   Description  : set fd no block attribute
*   Input        : const sock_fd *fd        set socket fd
*   Output       : None
*   Return Value : static void
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2019/8/29
*           Author       : Ydeer
*           Modification : Created function
*
*****************************************************************************/
static void set_no_block(const sock_fd fd)
{
    int temp_fd = fcntl(fd, F_GETFL, 0);
    if (temp_fd == -1) {
        printf("get fcntl temp_fd %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(fcntl(fd, F_SETFL, temp_fd | O_NONBLOCK) < 0)
    {
        perror("fcntl: set fd O_NONBLOCK error!\n");
        exit(EXIT_FAILURE);
    }
}

/*****************************************************************************
*   Prototype    : handle_message
*   Description  : handel socket message
*   Input        : const sock_fd fd  socket fd
*   Output       : None
*   Return Value : static void
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2019/8/31
*           Author       : Ydeer
*           Modification : Created function
*
*****************************************************************************/
static void handle_message(const sock_fd fd, const int epoll_fd)
{
    struct sockaddr_in addr;
    int size = sizeof(addr);
    memset(&addr, 0, size);
    if(-1 == getpeername(fd, (struct sockaddr *)&addr, &size))
    {
        perror("Get the user address fail!\n");
        exit(EXIT_FAILURE);
    }
    char *buff[10];
    memset(buff, 0, sizeof(buff));
    //是需要读取,每次读到的长度,一共读取的长度,读取次数[作为数组下标]
    int is_read = 1,buff_len = 0,total_buff_len = 0,num = 0;
    while (is_read > 0)
    {
        char *msg_buf = NULL;
        msg_buf = (char *)malloc(MAX_BUFF_LEN);
        if(NULL == msg_buf)
        {
            perror("Memory application failed!\n");
            exit(EXIT_FAILURE);
        }
        bzero(msg_buf, MAX_BUFF_LEN);
        buff_len = recv(fd, msg_buf, MAX_BUFF_LEN, 0);
        if(-1 == buff_len || 0 == buff_len) break;
        buff[num] = msg_buf;
        //未读取完情况下
        total_buff_len += buff_len;
        //如果本次读取的内容等于设置的最大BUFF长度则证明还可以继续读取
        if(buff_len == MAX_BUFF_LEN) 
        {
            num ++;
        }
        else
        {
            is_read = 0;
        }
    }
    if(-1 == buff_len)
    {
        perror("Received messages fail!\n");
        close_socket(fd, epoll_fd, addr);
    }
    else if(0 == buff_len)
    {
        printf("Reading messages from %s:%d failed. Maybe the other party has quit the connection!\n"\
        ,inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
        close_socket(fd, epoll_fd, addr);
    }
    else
    {
        fflush(stdout);
        int index = 0;
        for(; index <= num; ++index)
        {
            if(0 == index)
                printf("Received messages from %s:%d -> %s", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port),buff[index]);
            else
                printf("%s", buff[index]);
            free(buff[index]);
            buff[index] = NULL;
        }
        printf("\n");
    }
}

/*****************************************************************************
*   Prototype    : socket_message
*   Description  : handle socket message
*   Input        : const sock_fd fd  listen socket fd
*   Output       : None
*   Return Value : void
*   Calls        : 
*   Called By    : 
*
*   History:
* 
*       1.  Date         : 2019/8/29
*           Author       : Ydeer
*           Modification : Created function
*
*****************************************************************************/
void socket_message(const sock_fd fd)
{
    struct epoll_event events[MAX_EPOLL_EVENTS];
    int epoll_fd;
    memset(&epoll_fd, 0, sizeof(epoll_fd));
    memset(&events, 0, sizeof(events));
    add_default_event(fd, &epoll_fd);
    for (;;)
    {
        int nfds = epoll_wait(epoll_fd, events, 120, 2000);
        if(-1 == nfds)
        {
            perror("epoll_wait error!\n");
            exit(EXIT_FAILURE);
        }
        else if(0 == nfds)
        {
            printf("epoll_wait time out!\n");
            continue;
        }
        int i = 0;
        sock_fd event_fd;
        memset(&event_fd, 0, sizeof(event_fd));
        for (; i < nfds; ++i)
        {
            event_fd = events[i].data.fd;
            if((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP))
            {
                close(events[i].data.fd);
                continue;
            }
            else if(events[i].data.fd == fd)
            {
                accept_new_socket(event_fd, epoll_fd);
            }
            else
            {
                handle_message(event_fd, epoll_fd);
            }
        }
    }
}

