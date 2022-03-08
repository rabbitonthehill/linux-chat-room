#include "../include/client_sk.h"

sock_fd init_sk(const char* connect_addr,const unsigned short port)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    sock_fd fd = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > fd)
    {
        perror("create socket fail!\n");
        
        return -1;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(connect_addr);
    addr.sin_port = htons(port);
    if(0 > connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)))
    {
       perror("connect the address of a socket fail......\n");

       return -1;
    }
    printf("Successful connection to socket address!\n");

    return fd;
}

void socket_message(const sock_fd fd, const struct sockaddr_in send_addr)
{
    fd_set fds;
    char send_str[MAX_MSG_BUFF];
    char *recv_str[10];
    char *find = NULL;
    int send_byte = 0;
    struct sockaddr_in recv_addr;
    struct timeval tv;
    memset(&tv, 0, sizeof(tv));
    while (1)
    {
        //在循环中设置超时时间否则第二次不会设置超时
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        int maxfd = fd + 1;
        FD_ZERO(&fds);
        FD_SET(0,&fds);
        FD_SET(fd,&fds);
        int select_fds = select(maxfd, &fds, NULL, NULL, &tv);
        if(-1 == select_fds)
        {
            perror("select error!\n");
            exit(EXIT_FAILURE);
        }
        else if(0 == select_fds)
        {
            continue;
        }
        else
        {
            if(FD_ISSET(0, &fds))//如果是输入事件
            {
                bzero(send_str, sizeof(send_str));
                fgets(send_str, sizeof(send_str), stdin);
                find = strchr(send_str, '\n');
                if(find) *find = '\0';
                send_byte = send(fd, send_str, strlen(send_str), 0);
                if(-1 == send_byte)
                {
                    char err_msg[100];
                    sprintf(err_msg, "Failed to send message to %s:%d\n", inet_ntoa(send_addr.sin_addr), ntohs(send_addr.sin_port));
                    perror(err_msg);
                    exit(-1);
                }
                else
                {
                    printf("Successful delivery![%s]\n", send_str);
                }
            }
            if(FD_ISSET(fd, &fds))//连接的事件
            {
                memset(recv_str, 0, sizeof(recv_str));
                int recv_addr_size = sizeof(recv_addr);
                memset(&recv_addr, 0, recv_addr_size);
                int length = sizeof(struct sockaddr_in);
                if(-1 == getpeername(fd, (struct sockaddr *)&recv_addr, &recv_addr_size))
                {
                    perror("Get the user address fail!\n");
                    exit(EXIT_FAILURE);
                }
                //是需要读取,每次接收的长度、接收的总长度、读取次数[作为数组下标]
                int is_read = 1,recv_len = 1,total_buff_len = 0,num = 0;
                while (is_read > 0)
                {
                    char *msg_buf = NULL;
                    msg_buf = (char *)malloc(MAX_MSG_BUFF);
                    if(NULL == msg_buf)
                    {
                        perror("Memory application failed!\n");
                        exit(EXIT_FAILURE);
                    }
                    bzero(msg_buf, MAX_MSG_BUFF);
                    recv_len = recv(fd, msg_buf, sizeof(recv_str), 0);
                    if(-1 == recv_len || 0 == recv_len) break;
                    recv_str[num] = msg_buf;
                    total_buff_len += recv_len;
                    //如果本次读取的内容等于设置的最大BUFF长度则证明还可以继续读取
                    if(recv_len == MAX_MSG_BUFF) 
                    {
                        num ++;
                    }
                    else
                    {
                        is_read = 0;
                    }
                }
                
                if(-1 == recv_len)
                {
                    perror("Unopened service!\n");
                    exit(EXIT_FAILURE);
                }
                else if(0 == recv_len)
                {
                    perror("Server is closed!\n");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    fflush(stdout);
                    int index = 0;
                    for(; index <= num; ++index)
                    {
                        if(0 == index)
                            printf("Received from %s:%d message -> %s",inet_ntoa(recv_addr.sin_addr), ntohs(recv_addr.sin_port), recv_str[index]);
                        else
                            printf("%s", recv_str[index]);
                        free(recv_str[index]);
                        recv_str[index] = NULL;
                    }
                    printf("\n");
                }
            }  
        }
    }
}

