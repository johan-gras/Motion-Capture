#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "network.h"

#define BUFLEN 512       
#define PORT 9930
#define SRV_IP "192.168.10.11"


struct sockaddr_in si_other;
int s, slen=sizeof(si_other);
char buf[BUFLEN];


void diep(char *s)
{
	perror(s);
	exit(1);
}


// UDP init
void init_network(){
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        diep("socket");
    
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    if (inet_aton(SRV_IP, &si_other.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
}


// Send data via UDP
void send_data(int *x, int *y, int *x_acc, int *y_acc){
	sprintf(buf, "%d %d %d %d\n", *x, *y, *x_acc, *y_acc);
    if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, slen)==-1)
        diep("sendto()");
}
