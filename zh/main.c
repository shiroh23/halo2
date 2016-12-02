#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define MAXCLIENTS 3
#define PORT "2222"

struct DNS{
    struct DNS * next;
    char zone[50];
    int priority;
    char hostname[50];
    int timeout;
    int istimeout;
    time_t timestamp;
    int valid;
};

struct client{
    struct client * next;
    int fd;
    char ip[INET6_ADDRSTRLEN];
    char port[6];
    struct DNS * dns;
};

struct command{
    int fd;
    char comm[200];
    struct command * next;
    char ip[INET6_ADDRSTRLEN];
    char port[6];
};

int main(void)
{
    printf("Szerver Program!\n");

    struct client * cl = NULL;

    struct sockaddr claddr;

    struct addrinfo hints, *res;

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL,PORT,&hints,&res);

    int listener, new_fd;

    listener = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    bind(listener,res->ai_addr,res->ai_addrlen);
    listen(listener,10);

    int fdmax = listener;

    fd_set master, readfds;
    FD_ZERO(&master);
    FD_ZERO(&readfds);
    FD_SET(listener, &master);

    int i,j;
    int cnum = 0;

    char buff[100];

    struct command * allcomms = NULL;

    while(1){
        time_t actualTime = time(NULL);

        struct client * timeoutcl = cl;
        while(timeoutcl != NULL){
            struct DNS * timeoutdns = timeoutcl->dns;
            while(timeoutdns != NULL){
                if(timeoutdns->istimeout == 1){
                    if(timeoutdns->timeout < (difftime(actualTime,timeoutdns->timestamp)) && timeoutdns->valid == 1){
                        timeoutdns->valid = 0;
                    }
                }
                timeoutdns = timeoutdns->next;
            }
            timeoutcl = timeoutcl->next;
        }

        readfds = master;
        select(fdmax+1,&readfds,NULL,NULL,NULL);

        for(i=0;i<fdmax+1;i++){
            if(FD_ISSET(i,&readfds)){
                if(i == listener){
                    socklen_t claddrlen = sizeof(claddr);
                    new_fd = accept(listener,&claddr,&claddrlen);

                    if(cnum == MAXCLIENTS){
                        send(new_fd,"Nem fersz be! Bye!",strlen("Nem fersz be! Bye!"),0);
                        close(new_fd);
                    }else{
                        struct client * ujcl = (struct client *)malloc(sizeof(struct client));
                        ujcl->fd = new_fd;
                        ujcl->next = cl;
                        ujcl->dns = NULL;
                        cl = ujcl;

                        if(claddr.sa_family == AF_INET){
                            inet_ntop(AF_INET,&(((struct sockaddr_in *)&claddr)->sin_addr),ujcl->ip,INET_ADDRSTRLEN);
                            sprintf(ujcl->port,"%d",(((struct sockaddr_in *)&claddr)->sin_port));
                        }else{
                            inet_ntop(AF_INET6,&(((struct sockaddr_in6 *)&claddr)->sin6_addr),ujcl->ip,INET6_ADDRSTRLEN);
                            sprintf(ujcl->port,"%d",(((struct sockaddr_in6 *)&claddr)->sin6_port));
                        }

                        FD_SET(new_fd, &master);

                        if(new_fd > fdmax){
                            fdmax = new_fd;
                            FD_SET(fdmax, &master);
                        }

                        cnum++;
                    }
                }else{
                    int recbytes = recv(i,buff,sizeof(buff),0);
                    buff[recbytes-2] = '\0';

                    if(recbytes <= 0){
                        struct client * seged;
                        if(cl->fd == i){
                            seged = cl;
                            cl = cl->next;
                            free(seged);
                        }else{
                            seged = cl->next;
                            struct client * pre = cl;

                            while(seged->fd != i){
                                seged = seged->next;
                                pre = pre->next;
                            }

                            pre->next = seged->next;
                            free(seged);
                        }

                        close(i);
                        cnum--;

                        FD_CLR(i,&master);

                        int ujfdmax = 0;
                        for(j=0;j<fdmax+1;j++){
                            if(FD_ISSET(j,&master)){
                                ujfdmax = j;
                            }
                        }
                        fdmax = ujfdmax;
                    }else{
                        struct command * newcomm = (struct command *)malloc(sizeof(struct command));
                        newcomm->fd = i;
                        strcpy(newcomm->comm,buff);

                        struct client * commseged = cl;
                        while(commseged->fd != i){
                            commseged = commseged->next;
                        }

                        strcpy(newcomm->ip,commseged->ip);
                        strcpy(newcomm->port,commseged->port);

                        newcomm->next = allcomms;
                        allcomms = newcomm;

                        if(strncmp(buff,"PUTMX",5) == 0){
                            struct client * seged = cl;

                            while(seged->fd != i){
                                seged = seged->next;
                            }

                            strtok(buff," ");

                            struct DNS * ujdns = (struct DNS *)malloc(sizeof(struct DNS));
                            strcpy(ujdns->zone,strtok(NULL," "));
                            ujdns->priority = atoi(strtok(NULL," "));
                            strcpy(ujdns->hostname,strtok(NULL," "));

                            ujdns->next = seged->dns;
                            seged->dns = ujdns;

                            char * comm = (char*)malloc(sizeof(char)*5);

                            if((comm = strtok(NULL," ")) != NULL){
                                printf("%s\n",comm);
                                ujdns->istimeout = 1;
                                int szam = atoi(comm);
                                ujdns->timeout = szam;
                                ujdns->timestamp = time(NULL);
                                ujdns->valid = 1;
                            }else{
                                ujdns->istimeout = 0;
                                ujdns->timeout = 0;
                                ujdns->timestamp = time(NULL);
                                ujdns->valid = 1;
                            }

                            /*struct client * kiir = cl;
                            while(kiir != NULL){
                                while(kiir->dns != NULL){
                                    printf("zone: %s , priority: %d , hostname: %s \n",kiir->dns->zone,kiir->dns->priority,kiir->dns->hostname);
                                    kiir->dns = kiir->dns->next;
                                }
                                kiir = kiir->next;
                            }*/
                        }else{
                            if(strncmp(buff,"MX",2) == 0){
                                struct client * seged = cl;

                                strtok(buff," ");

                                char * zonename = strtok(NULL," ");

                                //printf("ciklus elott\n");
                                while(seged != NULL){
                                    char buffsend[200] = "";
                                    struct DNS * seged2 = seged->dns;
                                    //printf("ciklus 1.1\n");
                                    while(seged2 != NULL){
                                        //printf("ciklus 2.1\n");
                                        if(strncmp(zonename,seged2->zone,strlen(zonename)) == 0 && (seged2->valid == 1)){
                                            //printf("feltetel 1\n");
                                            strcat(buffsend,"Priority: ");
                                            char pri[3] = "";
                                            sprintf(pri,"%d",seged2->priority);
                                            strcat(buffsend,pri);
                                            strcat(buffsend," Hostname: ");
                                            strcat(buffsend,seged2->hostname);
                                            strcat(buffsend,"\n");
                                            //printf("feltetel 2\n");
                                        }
                                        //printf("ciklus 2.2\n");
                                        seged2 = seged2->next;
                                    }
                                    //printf("ciklus 1.2\n");

                                    send(i,buffsend,strlen(buffsend),0);

                                    seged = seged->next;
                                }
                            }else{
                                if(strncmp(buff,"IPv4",4) == 0){
                                    strtok(buff," ");

                                    char * hostnametok = strtok(NULL," ");

                                    struct addrinfo hints2, *res2;

                                    memset(&hints2,0,sizeof(hints2));
                                    hints2.ai_family = AF_INET;
                                    hints2.ai_socktype = SOCK_DGRAM;
                                    hints2.ai_flags = 0;
                                    hints2.ai_addrlen = INET_ADDRSTRLEN;

                                    getaddrinfo(hostnametok,NULL,&hints2,&res2);

                                    while(res2 != NULL){
                                        char ipc[INET_ADDRSTRLEN] = "";

                                        inet_ntop(AF_INET,&(((struct sockaddr_in *)res2->ai_addr)->sin_addr),ipc,INET_ADDRSTRLEN);

                                        printf("%s\n",ipc);
                                        char buffsend[200] = "";
                                        strcat(buffsend,ipc);
                                        strcat(buffsend,"\n");
                                        send(i,buffsend,strlen(buffsend),0);

                                        res2 = res2->ai_next;
                                    }

                                    /*struct hostent * ipk = gethostbyname(hostnametok);

                                    for(j=0;j<ipk->h_length;j++){
                                        char ipc[INET_ADDRSTRLEN];

                                        inet_ntop(AF_INET,&(((struct sockaddr_in *)&ipk->h_addr_list[j])->sin_addr),ipc,INET_ADDRSTRLEN);

                                        printf("%s\n",ipc);
                                    }*/

                                    /*inet_ntop(AF_INET,&(((struct sockaddr_in *)&claddr)->sin_addr),ujcl->ip,INET_ADDRSTRLEN);
                                    sprintf(ujcl->port,"%d",(((struct sockaddr_in *)&claddr)->sin_port));*/
                                }else{
                                    if(strncmp(buff,"AAAA",4) == 0){
                                        strtok(buff," ");

                                        char * hostnametok = strtok(NULL," ");

                                        struct addrinfo hints2, *res2;

                                        memset(&hints2,0,sizeof(hints2));
                                        hints2.ai_family = AF_INET6;
                                        hints2.ai_socktype = SOCK_DGRAM;
                                        hints2.ai_flags = 0;
                                        hints2.ai_addrlen = INET6_ADDRSTRLEN;

                                        getaddrinfo(hostnametok,NULL,&hints2,&res2);

                                        while(res2 != NULL){
                                            char ipc[INET6_ADDRSTRLEN] = "";

                                            inet_ntop(AF_INET6,&(((struct sockaddr_in6 *)res2->ai_addr)->sin6_addr),ipc,INET6_ADDRSTRLEN);

                                            printf("%s\n",ipc);
                                            char buffsend[200] = "";
                                            strcat(buffsend,ipc);
                                            strcat(buffsend,"\n");
                                            send(i,buffsend,strlen(buffsend),0);

                                            res2 = res2->ai_next;
                                        }
                                    }else{
                                        if(strncmp(buff,"?",1) == 0){
                                            struct command * segedcomm = allcomms;

                                            while(segedcomm != NULL){
                                                char buff2[200] = "";
                                                char pri[3] = "";
                                                sprintf(pri,"%d",segedcomm->fd);
                                                strcat(buff2,pri);
                                                strcat(buff2,"\t");
                                                strcat(buff2,segedcomm->ip);
                                                strcat(buff2,"\t");
                                                strcat(buff2,segedcomm->port);
                                                strcat(buff2,"\t");
                                                strcat(buff2,segedcomm->comm);
                                                strcat(buff2,"\n");
                                                send(i,buff2,strlen(buff2),0);
                                                segedcomm = segedcomm->next;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

