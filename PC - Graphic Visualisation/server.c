#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "base_opengl.h"

#define BUFLEN 512
#define PORT 9930

struct sockaddr_in si_me, si_other;
int s, i, slen=sizeof(si_other);
char buf[BUFLEN];

void diep(char *s)
{
    perror(s);
    exit(1);
}

// Server init
void init_serveur(){
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        diep("socket");
    
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, &si_me, sizeof(si_me))==-1)
        diep("bind");
}

// Wait until it received data
void get_data(int *x, int *y, int *x_acc, int *y_acc){
    char * pch;
    
    if (recvfrom(s, buf, BUFLEN, 0, &si_other, &slen)==-1)
        diep("recvfrom()");
    printf("Received packet from %s:%d\nData: %s\n\n",
           inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
    
    // Parsing
    pch = strtok (buf," ");
    *x = atoi(pch);
    pch = strtok (NULL, " ");
    *y = atoi(pch);
    pch = strtok (NULL, " ");
    *x_acc = atoi(pch);
    pch = strtok (NULL, " ");
    *y_acc = atoi(pch);
}

// Server stop
void stop_serveur(){
    close(s);
}
