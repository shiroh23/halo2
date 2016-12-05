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

#define MAXCLIENTS 3
#define PORT "1111"

struct client
{
    int fd;
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
    listen(listener, 10);

    int i,j,cnum=0, fdmax=listener;
    char buff[512]="";

    struct sockaddr claddr;
    socklen_t claddrlen;

    fd_set master, read;
    FD_ZERO(&master);
    FD_ZERO(&read);
    FD_SET(listener, &master);

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
                    int newfd = accept(i, &claddr, &claddrlen);
                    if (cnum == MAXCLIENTS)
                    {
                        send(newfd, "bocsi nem fersz be\n", strlen("bocsi nem fersz be\n"), 0);
                        close(newfd);
                    }
                    else
                    {
                        struct client *uj = (struct client*)malloc(sizeof(struct client));
                        uj->fd = newfd;
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
                        FD_SET(newfd, &master);
                        if (fdmax < newfd) fdmax = newfd;
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
                            char abc[27]="";
                            for (j=0; j<26; j++)
                            {
                                abc[j] = 'a'+j;
                                //printf("%c,", abc[j]);
                            }
                            //printf("\n");
                            int k, tarolo[27];
                            int abcHossz = strlen(abc);

                            for (j=0; j<abcHossz; j++)
                            {
                                tarolo[j] = 0;
                            }

                            for (j=0; j<hossz; j++)
                            {
                                for (k=0; k<abcHossz; k++)
                                {
                                    if (buff[j] == abc[k])
                                    {
                                        tarolo[k]++;
                                        //printf("%c talalt\n", abc[k]);
                                    }
                                }
                            }

                            char msg[512]="";

                            struct client *temp = cl;
                            for (; temp->fd != i; temp=temp->next);
                            int socket = temp->fd;

                            sprintf(msg, "A(z) %d. socket szavanak hisztogramja: %s\n", socket, buff);
                            for (k=0; k<abcHossz; k++)
                            {
                                if (tarolo[k] != 0)
                                {
                                    char betu[1]="";
                                    sprintf(betu, "%c", abc[k]);
                                    strcat(msg, betu);
                                    strcat(msg, " betu: ");
                                    char szam[1]="";
                                    sprintf(szam, "%d", tarolo[k]);
                                    strcat(msg, szam);
                                    strcat(msg, " db\n");
                                }
                            }
                            strcat(msg, "\n");
                            for (j=0; j<=fdmax; j++)
                            {
                                if ((FD_ISSET(j, &master) && j!=listener) && j!=i)
                                {
                                    send(j, msg, sizeof(msg), 0);
                                }
                            }
                        }
                        else
                        {
                            send(i, "Adj meg valamit\n", strlen("Adj meg valamit\n"), 0);
                        }
                    }
                }
            }
        }
    }



    close(listener);
    freeaddrinfo(res);
    struct client * del = cl;
        while (del != NULL) {
            del = cl;
            cl = cl->next;
            free(del);
            del = NULL;
        }
    free(cl);

    return 0;
}

