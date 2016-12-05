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

/*
 * int yes=1;

       if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
           perror("setsockopt");
           exit(1);
       }*/


#define MAXCLIENT 3
#define PORT "1111"
#define TIMING 10

struct client
{
    int fd;
    char ip[INET6_ADDRSTRLEN];
    char port[10];
    struct client *next;
};

int main(void)
{/*
    struct addrinfo hints, *res;
    struct client *cl = NULL;

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

    int i,j,cnum=0,fdmax=listener, maxlicit=0, socket=0;
    char buff[512];
    int megy = 0;

    fd_set master, readfds;
    FD_ZERO(&master);
    FD_ZERO(&readfds);
    FD_SET(listener, &master);

    struct sockaddr claddr;
    socklen_t claddrlen;

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int retval=0, eltelt=0;

    while (1)
    {
        readfds = master;
        retval = select(fdmax+1, &readfds, NULL, NULL, &tv);

        if (retval)
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
                            uj->next = cl;
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
                            char msg[128]="";
                            if (megy == 0)
                            {
                                sprintf(msg, "Varom a teteket\n");
                                send(new_fd, msg, sizeof(msg), 0);
                            }
                            else
                            {
                                sprintf(msg, "Varom a teteket\nJelenlegi legnagyobb:\n %d - licit: %d\n", socket, maxlicit);
                                send(new_fd, msg, sizeof(msg), 0);
                            }
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
                                megy = 1;
                                if (maxlicit < hasznalhato)
                                {
                                    eltelt = 0;
                                    char msg[512]="";
                                    struct client * temp = cl;
                                    for (; temp->fd != i; temp=temp->next);
                                    socket = temp->fd;
                                    sprintf(msg, "kliens: %d - licit: %d\n", socket, hasznalhato);

                                    maxlicit = hasznalhato;

                                    for (j=0; j<=fdmax; j++)
                                    {
                                        if (FD_ISSET(j, &master) && j!=listener && j!=i)
                                        {
                                            send(j, msg, sizeof(msg), 0);
                                        }
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
        if (retval == 0 && megy == 1)
        {
            eltelt++;
            printf(">> Utolsó licit óta eltelt %d sec...\n",eltelt);
            tv.tv_sec = 1;
            if(eltelt==TIMING)    {
                char sendbuf[512]="";
                sprintf(sendbuf,"\n>> %d másodperce nem érkezett nagyobb licit, így az aukció lezárult!\n>> A nyertes a(z) %d. socket, aki %d kreditet ajánlott a termékért!\n>> Helóka!\n\n",TIMING,socket,maxlicit);
                for(j=0;j<=fdmax;j++)
                {
                    if(FD_ISSET(j, &master) && j!=listener && j!=i)
                    {
                        send(j,sendbuf,sizeof(sendbuf),0);
                    }
                }
                close(listener);

                struct client * del = cl;
                    while (del != NULL) {
                        del = cl;
                        cl = cl->next;
                        free(del);
                        del = NULL;
                    }
                free(cl);
                break;
            }
        }
        if (retval == 0 && megy == 0) tv.tv_sec = 1;
    }
*/
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

    int i,j,cnum=0,fdmax=listener, socket=0, maxlicit=0, megy=0;
    char buff[512];

    fd_set master, readfds;
    FD_ZERO(&master);
    FD_ZERO(&readfds);
    FD_SET(listener, &master);

    int retval=0, eltelt=0;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    struct sockaddr claddr;
    socklen_t claddrlen;

    while (1)
    {
        readfds = master;
        retval = select(fdmax+1, &readfds, NULL, NULL, &tv);

        if (retval)
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
                            send(new_fd, "Sajnos mar nincs tobb hely az aukciora\n", strlen("Sajnos mar nincs tobb hely az aukciora\n"), 0);
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
                            if (megy == 0)
                            {
                                char msg[128]="";
                                sprintf(msg, "Üdv!\nVarom a teteket: ");
                                send(new_fd, msg, sizeof(msg), 0);
                            }
                            else
                            {
                                char msg[128]="";
                                sprintf(msg, "Üdv!\nVarom a teteket!\nJelenlegi legnagyobb: %d socket - licit: %d\n", socket, maxlicit);
                                send(new_fd, msg, sizeof(msg), 0);
                            }
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
                            int hasznos = atoi(buff);
                            if (hasznos != 0)
                            {
                                megy = 1;
                                printf("licit: %d\n", hasznos);
                                if (maxlicit < hasznos)
                                {
                                    eltelt = 0;
                                    maxlicit = hasznos;
                                    char msg[256]="";

                                    struct client *temp = cl;
                                    for (; temp->fd != i; temp=temp->next);
                                    socket = temp->fd;
                                    sprintf(msg, "%d. socket - %d licittel\n", socket, hasznos);

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
                                    send(i, "Keves licit, adj meg nagyobb osszeget!\n", strlen("Keves licit, adj meg nagyobb osszeget!\n"), 0);
                                }
                            }
                            else
                            {
                                send(i, "Adj meg rendes erteket\n", strlen("Adj meg rendes erteket\n"), 0);
                            }
                        }
                    }
                }
            }
        }
        if (retval == 0 && megy == 1)
        {
            eltelt++;
            tv.tv_sec = 1;
            printf(">> %d. masodperce nem erkezett licit!\n", eltelt);
            if (eltelt == TIMING)
            {
                char sendbuff[512]="";
                sprintf(sendbuff, "%d masodperc eltelte utan nem erkezett licit!\nA nyertes a %d. socket, melynek licitje %d kredit volt\nGratulalunk, viszlat\n", TIMING,socket,maxlicit);
                for (j=0; j<=fdmax; j++)
                {
                    if (FD_ISSET(j, &master) && j!=i && j!=listener)
                    {
                        send(j, sendbuff, sizeof(sendbuff), 0);
                    }
                }
                break;
            }
        }
        if (retval == 0 && megy == 0) tv.tv_sec = 1;
    }

*/

    struct addrinfo hints, *res;
    struct client *cl;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &res);
    int listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    int yes=1;

       if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
           perror("setsockopt");
           exit(1);
       }
    bind(listener, res->ai_addr, res->ai_addrlen);
    listen(listener, 10);

    int i,j,cnum=0,fdmax=listener,megy=0,socket=0,maxlicit=0;
    int retval, eltelt=0;
    char buff[512]="";

    fd_set master, read;
    FD_ZERO(&master);
    FD_ZERO(&read);
    FD_SET(listener, &master);

    struct sockaddr claddr;
    socklen_t claddrlen;

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (1)
    {
        read = master;
        retval = select(fdmax+1, &read, NULL, NULL, &tv);
        if (retval)
        {
            for (i=0; i<=fdmax; i++)
            {
                if (FD_ISSET(i, &read))
                {
                    if (i == listener)
                    {
                        claddrlen = sizeof(claddr);
                        int new_fd = accept(i, &claddr, &claddrlen);
                        if (cnum == MAXCLIENT)
                        {
                            send(new_fd, "sajnos nem fersz be\n", strlen("sajnos nem fersz be\n"), 0);
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
                            if (megy == 0)
                            {
                                send(new_fd, "Varom a teteket!\n", strlen("Varom a teteket!\n"), 0);
                            }
                            else
                            {
                                char msg[128]="";
                                sprintf(msg, "Varom a teteket!\nJelenlegi maxlicit: %d. socket - %d kredit\n", socket, maxlicit);
                                send(new_fd, msg, sizeof(msg), 0);
                            }
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
                            int hasz = atoi(buff);
                            if (hasz != 0)
                            {
                                struct client *temp = cl;
                                for (; temp->fd != i; temp=temp->next);
                                socket = temp->fd;

                                if (maxlicit < hasz)
                                {
                                    megy = 1;
                                    eltelt = 0;
                                    maxlicit = hasz;

                                    char msg[64]="";
                                    sprintf(msg, "Jelenlegi max: %d. socket - %d kredit\n", socket, maxlicit);
                                    for (j=0; j<=fdmax; j++)
                                    {
                                        if ((FD_ISSET(j, &master) && j!=listener) && j!=i)
                                        {
                                            send(j, msg, sizeof(msg), 0);
                                        }
                                    }
                                    send(i, "Tied a legnagyobb licit!\n", strlen("Tied a legnagyobb licit!\n"), 0);
                                }
                                else
                                {
                                    send(i, "Nem eleg nagy a licited!\n", strlen("Nem eleg nagy a licited!\n"), 0);
                                }
                            }
                            else
                            {
                                send(i, "NEM ERTEM\n", strlen("NEM ERTEM\n"), 0);
                            }
                        }
                    }
                }
            }
        }
        if (retval == 0 && megy == 1)
        {
            eltelt++;
            tv.tv_sec = 1;
            printf("%d seconds left until end\n", TIMING-eltelt);
            if (eltelt == TIMING)
            {
                char msg[256]="";
                sprintf(msg, "%d seconds of inactivity, so the winner is the %d. socket with %d credit\nCongrats!\nSee you next time!\n", TIMING, socket, maxlicit);
                for (j=0; j<=fdmax; j++)
                {
                    if ((FD_ISSET(j, &master) && j!=listener) || j==i)
                    {
                        send(j, msg, sizeof(msg), 0);
                    }
                }
                break;
            }
        }
        if (retval == 0 && megy == 0) tv.tv_sec = 1;
    }





































    return 0;
}

