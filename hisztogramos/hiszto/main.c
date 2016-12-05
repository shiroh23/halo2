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

#define PORT "1111"
#define MAXCLIENTS 3

struct client
{
    int fd;
    char ip[INET6_ADDRSTRLEN];
    char port[10];
    struct client *next;
};

int main(void)
{
    /*
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
    listen(listener, 10);

    int i,j,cnum=0,fdmax=listener;
    char buff[512]="";

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
                    if (cnum == MAXCLIENTS)
                    {
                        send(new_fd, "Bocsi nincs mar tobb hely\n", strlen("Bocsi nincs mar tobb hely\n"), 0);
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
                            int k=0, talalt=0;
                            printf("Atjott: %s, hossza: %d\n", buff, hossz);
                            int tarolo[128];
                            char msg[512]="";

                            struct client *temp = cl;
                            for (; temp->fd != i; temp=temp->next);
                            int socket = temp->fd;
                            sprintf(msg, "A %d socket szava: %s\n", socket, buff);

                            char betuk[128]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','v','w','u','x','y','z'};
                            int bhossz= strlen(betuk);
                            printf("ABC hossza: %d\n", bhossz);
                            for (j=0;j<128;j++) tarolo[j]=0;

                            for (j=0; j<hossz; j++)
                            {
                                for (k=0; k<bhossz; k++)
                                {
                                    if (buff[j] == betuk[k])
                                    {
                                        tarolo[k]++;
                                        talalt++;
                                    }
                                }
                            }
                            for (j=0; j<128; j++)
                            {
                                if (tarolo[j] != 0)
                                {
                                    strcat(msg, "betu: ");
                                    char betu[1]="";
                                    sprintf(betu, "%c", betuk[j]);
                                    strcat(msg, betu);
                                    strcat(msg, " - ");
                                    char betu2[1]="";
                                    sprintf(betu2, "%d", tarolo[j]);
                                    strcat(msg, betu2);
                                    strcat(msg, " db\n");
                                }
                            }
                            strcat(msg, "\n");
                            for (j=0; j<=fdmax; j++)
                            {
                                if (FD_ISSET(j, &master) && j!=i && j!=listener)
                                {
                                    send(j, msg, sizeof(msg), 0);
                                }
                            }
                        }
                        else
                        {
                            send(i, "Adj meg valamit!\n", strlen("Adj meg valamit!\n"), 0);
                        }
                    }
                }
            }
        }
    }
*/

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
    listen(listener, 10);

    int i,j,cnum=0,fdmax=listener;
    char buff[512]="";

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
                    if (cnum == MAXCLIENTS)
                    {
                        send(new_fd, "Bocsi nincs tobb hely szamodra!\n", strlen("Bocsi nincs tobb hely szamodra!\n"), 0);
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
                            for (; akt->fd != i; akt=akt->next, tmp=tmp->next);
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
                            char msg[512]="";

                            struct client *temp = cl;
                            for (; temp->fd != i; temp=temp->next);
                            int socket = temp->fd;

                            sprintf(msg, "A %d socketen levo kliens szava: %s\n", socket, buff);
                            int tarolo[35];
                            for (j=0; j<35; j++) tarolo[j]=0;
                            int k;

                            char betuk[35]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
                            int bhossz = strlen(betuk);
                            for (j=0; j<hossz; j++)
                            {
                                for (k=0; k<bhossz; k++)
                                {
                                    if (buff[j] == betuk[k])
                                    {
                                        tarolo[k]++;
                                    }
                                }
                            }
                            for (j=0; j<35; j++)
                            {
                                if (tarolo[j] != 0)
                                {
                                    strcat(msg, "betu: ");
                                    char betu[1]="";
                                    sprintf(betu, "%c", betuk[j]);
                                    strcat(msg, betu);
                                    strcat(msg, " - ");
                                    char betu2[1]="";
                                    sprintf(betu2, "%d", tarolo[j]);
                                    strcat(msg, betu2);
                                    strcat(msg, " db\n");
                                }
                            }
                            strcat(msg, "\n-----------------\n");

                            for (j=0; j<=fdmax; j++)
                            {
                                if (FD_ISSET(j, &master) && j!=i && j!=listener)
                                {
                                    send(j, msg, sizeof(msg), 0);
                                }
                            }
                        }
                        else
                        {
                            send(i, "Adj meg egy szot!\n", strlen("Adj meg egy szot!\n"), 0);
                        }
                    }
                }
            }
        }
    }





































    return 0;
}

