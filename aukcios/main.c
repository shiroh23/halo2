#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include  <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define MAXCLIENT 3
#define PORT "1111"

struct client
{
    int fd;
    char licit[128];
    char ip[INET6_ADDRSTRLEN];
    char port[10];
    struct client *next;
};

int main(void)
{
    struct addrinfo hints, *res;
    struct client *cl;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(NULL, PORT, &hints, &res);
    int listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    int yes=1;

       if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
           perror("setsockopt");
           exit(1);
       }

    bind(listener, res->ai_addr, res->ai_addrlen);
    listen(listener, 5);

    int i,j,cnum=0,fdmax=listener, maxlicit=0;
    char buff[512];

    fd_set master, readfds;
    FD_ZERO(&master);
    FD_ZERO(&readfds);
    FD_SET(listener, &master);

    struct sockaddr claddr;
    socklen_t claddrlen;

    struct timeval tv;
    tv.tv_sec = 20;
    tv.tv_usec = 0;
    int retval=0, eltelt=0;

    while (1)
    {
        readfds = master;
        select(fdmax+1, &readfds, NULL, NULL, NULL);

        if (retval == 0)
        {
            for (i=0; i<=fdmax; i++)
            {
                if (FD_ISSET(i, &readfds))
                {
                    if (i == listener)
                    {
                        claddrlen = sizeof(claddr);
                        int new_fd = accept(i, &claddr, &claddrlen);
                        if (cnum == MAXCLIENT)
                        {
                            send(new_fd, "nincs tobb hely\n", strlen("nincs tobb hely\n"), 0);
                            close(new_fd);
                        }
                        else
                        {
                            struct client *uj = (struct client*)malloc(sizeof(struct client));
                            uj->fd = new_fd;
                            if (claddr.sa_family == AF_INET)
                            {
                                inet_ntop(AF_INET, &(((struct sockaddr_in*)&claddr)->sin_addr), uj->ip, INET_ADDRSTRLEN);
                                sprintf(uj->port, "%d", ntohs(((struct sockaddr_in*)&claddr)->sin_port));
                            }
                            else
                            {
                                inet_ntop(AF_INET6, &(((struct sockaddr_in6*)&claddr)->sin6_addr), uj->ip,INET6_ADDRSTRLEN);
                                sprintf(uj->port, "%d", ntohs(((struct sockaddr_in6*)&claddr)->sin6_port));
                            }
                            cnum++;
                            cl = uj;
                            FD_SET(new_fd, &master);
                            if (fdmax < new_fd) fdmax = new_fd;
                            send(new_fd, "Varom a teteket\n", strlen("Varom a teteket\n"), 0);
                        }
                    }
                    else
                    {
                        int recbytes = recv(i, buff, sizeof(buff), 0);
                        if (recbytes == 0)
                        {
                            struct client *tmp = cl;
                            if (tmp->fd == i)
                            {
                                cl = cl->next;
                                free(tmp);
                            }
                            else
                            {
                                struct client *akt = cl->next;
                                for (; akt->fd != i; akt=akt->next, tmp=tmp->next);
                                tmp->next = akt->next;
                                free(akt);
                            }
                            close(i);
                            cnum--;
                            FD_CLR(i, &master);
                            int fdmaxuj = 0;
                            for (j=0; j<=fdmax; j++)
                            {
                                if (FD_ISSET(j, &master))
                                {
                                    fdmaxuj = j;
                                }
                            }
                            fdmax = fdmaxuj;
                        }
                        else
                        {
                            buff[recbytes-2] = '\0';
                            int hasznalhato = atoi(buff);
                            printf("%d\n", hasznalhato);
                            if (hasznalhato != 0)
                            {
                                for (j=0; j<=fdmax; j++)
                                {
                                    if (FD_ISSET(j, &master) && j!=listener)
                                    {
                                        send(j, "szuper sikerult\n", strlen("szuper sikerult\n"), 0);
                                    }
                                }
                            }
                            else
                            {
                                send(i, "valosat adj meg\n", strlen("valosat adj meg\n"), 0);
                            }
                        }
                    }
                }
            }
        }

    }























    return 0;
}

