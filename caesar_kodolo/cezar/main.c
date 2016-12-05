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
#include <string.h>
#include <ctype.h>

/*
 * int yes=1;

       if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
           perror("setsockopt");
           exit(1);
       }*/


#define PORT "1111"
#define MAXCLIENTS 2

struct client
{
    int fd;
    char ip[INET6_ADDRSTRLEN];
    char port[10];
    struct client *next;
};

int main(void)
{
    struct addrinfo hints, hints2, *res, *res2;
    struct client *cl;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    memset(&hints2, 0, sizeof(hints2));
    hints2.ai_family = AF_UNSPEC;
    hints2.ai_socktype = SOCK_STREAM;

    char servIP[INET6_ADDRSTRLEN];
    char servPORT[10];
    char servNAME[64]="";

    gethostname(servNAME, sizeof(servNAME));

    getaddrinfo(servNAME, PORT, &hints2, &res2);

    if (res2->ai_family == AF_INET)
    {
        inet_ntop(AF_INET, &(((struct sockaddr_in*)(res2->ai_addr))->sin_addr), servIP, INET_ADDRSTRLEN);
        sprintf(servPORT, "%d", ntohs(((struct sockaddr_in*)&res2)->sin_port));
    }
    else
    {
        inet_ntop(AF_INET6, &(((struct sockaddr_in6*)(res2->ai_addr))->sin6_addr), servIP, INET6_ADDRSTRLEN);
        sprintf(servPORT, "%d", ntohs(((struct sockaddr_in6*)&res2)->sin6_port));
    }

    printf("SZERVER: %s - %s - %s\n", servNAME, servIP, servPORT);

    getaddrinfo(NULL, PORT, &hints, &res);
    int listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    int yes=1;

           if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
               perror("setsockopt");
               exit(1);
           }
    bind(listener, res->ai_addr, res->ai_addrlen);
    listen(listener, 10);

    int i,j,cnum=0,fdmax=listener;
    char buff[512]="";

    fd_set master, read;
    FD_ZERO(&master);
    FD_ZERO(&read);
    FD_SET(listener, &master);

    struct sockaddr claddr;
    socklen_t claddrlen;


    while (1)
    {
        read = master;
        select(fdmax+1, &read, NULL, NULL, NULL);
        for (i=0; i<=fdmax; i++)
        {
            if (FD_ISSET(i, &read))
            {
                if (i == listener)
                {
                    claddrlen = sizeof(claddr);
                    int new_fd = accept(i, &claddr, &claddrlen);
                    if (cnum == MAXCLIENTS)
                    {
                        send(new_fd, "nem\n", strlen("nem\n"), 0);
                        close(new_fd);
                    }
                    else
                    {
                        struct client *uj = (struct client*)malloc(sizeof(struct client));
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
                        cnum++;
                        cl = uj;
                        FD_SET(new_fd, &master);
                        if (fdmax < new_fd) fdmax = new_fd;
                        send(new_fd ,"udvozollek\n", strlen("udvozollek\n"), 0);
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
                            cl=cl->next;
                            free(tmp);
                        }
                        else
                        {
                            struct client *akt = cl->next;
                            for (; akt->fd != i; tmp=tmp->next, akt=akt->next);
                            tmp->next = akt->next;
                            free(akt);
                            free(tmp);
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
                        int hossz = strlen(buff);
                        if (hossz != 0)
                        {
                            char msg[1024]="";
                            char cezar[64]="";
                            sprintf(msg, "A cezar kod: ");

                            for (j=0; j<hossz; j++)
                            {
                                char offset = isupper(buff[j]) ? 'A' : 'a';
                                cezar[j] = (char)((((buff[j] + 3) - offset) % 26) + offset);
                            }
                            strcat(msg, cezar);
                            strcat(msg, "\n");

                            struct client *temp = cl;
                            for (; temp->fd != i; temp=temp->next);

                            strcat(msg, "IP: ");
                            strcat(msg, temp->ip);
                            strcat(msg, " port: ");
                            strcat(msg, temp->port);
                            strcat(msg, "\nserver IP: ");
                            strcat(msg, servIP);
                            strcat(msg, " server port: ");
                            strcat(msg, servPORT);
                            strcat(msg, "\n");

                            for (j=0; j<=fdmax; j++)
                            {
                                if (FD_ISSET(j, &master) && j!=listener && j!=i)
                                {
                                    send(j, msg, sizeof(msg), 0);
                                }
                            }
                        }
                        else
                        {
                            send(i, "adj meg valamit\n", strlen("adj meg valamit\n"), 0);
                        }
                    }
                }
            }
        }
    }
















close(listener);
freeaddrinfo(res);
freeaddrinfo(res2);












    return 0;
}

