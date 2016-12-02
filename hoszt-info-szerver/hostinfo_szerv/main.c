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

#define MAX_CLIENTS 2
#define PORT "2323"

struct client
{
    int fd;
    char ip[INET6_ADDRSTRLEN];
    char port[10];
    struct client * next;
};

int main(void)
{
    struct addrinfo hints, hints2, *res, *res2, *seged;
    struct client *cl;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    memset(&hints2, 0, sizeof(hints2));
    hints2.ai_family = AF_UNSPEC;
    hints2.ai_socktype = SOCK_STREAM;

    getaddrinfo(NULL, PORT, &hints, &res);

    int listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    int yes=1;

       if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
           perror("setsockopt");
           exit(1);
       }

    bind(listener, res->ai_addr, res->ai_addrlen);
    listen(listener, 5);

    int i,j,cnum=0,fdmax=listener;
    char buff[128]="";

    fd_set master, readfds;
    FD_ZERO(&master);
    FD_ZERO(&readfds);
    FD_SET(listener, &master);

    struct sockaddr claddr;
    socklen_t claddrlen;

    while (1)
    {
        readfds = master;
        select(fdmax+1, &readfds, NULL, NULL, NULL);
        for (i=0; i<=fdmax; i++)
        {
            if (FD_ISSET(i, &readfds))
            {
                if (i == listener)
                {
                    claddrlen = sizeof(claddr);
                    int new_fd = accept(i, &claddr, &claddrlen);
                    if (cnum == MAX_CLIENTS)
                    {
                        send(new_fd, "Sajnos nem fersz be\n", strlen("Sajnos nem fersz be\n"), 0);
                        close(new_fd);
                    }
                    else
                    {
                        struct client * uj = (struct client*)malloc(sizeof(struct client));
                        uj->fd = new_fd;
                        uj->next = cl;
                        if (claddr.sa_family == AF_INET)
                        {
                            inet_ntop(AF_INET, &(((struct sockaddr_in*)&claddr)->sin_addr), uj->ip, INET_ADDRSTRLEN);
                            sprintf(uj->port, "%d", ntohs(((struct sockaddr_in*)&claddr)->sin_port));
                        }
                        else
                        {
                            inet_ntop(AF_INET6, &(((struct sockaddr_in6*)&claddr)->sin6_addr), uj->ip, INET6_ADDRSTRLEN);
                            sprintf(uj->port, "%d", ntohs(((struct sockaddr_in6*)&claddr)->sin6_port));
                        }
                        cl = uj;
                        cnum++;
                        FD_SET(new_fd, &master);
                        if (fdmax < new_fd) fdmax = new_fd;
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
                            for (; akt->fd != i; tmp=tmp->next, akt=akt->next);
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
                        if (strncmp(buff, "IP", 2) == 0)
                        {
                            char s[2] = " ";
                            char * token;
                            token = strtok(buff, s);
                            token = strtok(NULL, s);

                            char msg[512]="";
                            sprintf(msg, "Az ip cimek a kovetkezok: \n");
                            void *addr;
                            char ip[INET6_ADDRSTRLEN];

                            getaddrinfo(token, NULL, &hints2, &res2);

                            for (seged = res2; seged != NULL; seged = seged->ai_next)
                            {
                                if (seged->ai_family == AF_INET)
                                {
                                    struct sockaddr_in *ipv4 = (struct sockaddr_in*)seged->ai_addr;
                                    addr = &(ipv4->sin_addr);
                                    //sprintf(port, "%d", ntohs(((struct sockaddr_in*)&seged)->sin_port));
                                }
                                else
                                {
                                    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)seged->ai_addr;
                                    addr = &(ipv6->sin6_addr);
                                    //sprintf(port, "%d", ntohs(((struct sockaddr_in*)&seged)->sin_port));
                                }
                                inet_ntop(seged->ai_family, addr, ip, sizeof(ip));
                                printf("%s", ip);
                                strcat(msg, ip);
                                strcat(msg, "\n");
                            }
                            send(i, msg, sizeof(msg), 0);
                        }
                        else
                        {
                            if (strncmp(buff, "NEV", 3) == 0)
                            {
                                printf("%s\n", buff);
                            }
                            else
                            {
                                send(i, "Nem ertelmezett parancs\n", strlen("Nem ertelmezett parancs\n"), 0);
                            }
                        }
                    }
                }
            }
        }
    }





    return 0;
}

