//
//  server.c
//  FileTransferLab1
//
//  Created by Sagar Chadha & Pratiksha Shenoy on 2019-09-24.
//  Copyright © 2019 Sagar Chadha & Pratiksha Shenoy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLEN 4096

int main(int argc, char const *argv[]){
    
    int port;
    
    switch(argc) {
        case 2:
            port = atoi(argv[1]);
            break;
        default:
            fprintf(stderr, "usage: server (UDP listen port)\n");
            exit(0);
    }
    
    int socketDescriptor;
    
    //Creates a socket
    if ((socketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        fprintf(stderr, "socket error\n");
        exit(1);
    }
    
    struct sockaddr_in serverAddress;
    
    //Signals that we are using IPV4
    serverAddress.sin_family = AF_INET;
    //Converts port to a correct format
    serverAddress.sin_port = htons(port);
    //**idk
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    //Sets it to the right memory space
    memset(serverAddress.sin_zero, 0, sizeof(serverAddress.sin_zero));
    
    //Assigns address to the socket
    if (bind(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1 ) {
        fprintf(stderr, "Can't bind name to socket\n");
        exit(1);
    }
    
    
    char message_buffer[MAXLEN] = {0};
    struct sockaddr_in clientAddress;
    unsigned clientMessageLength = sizeof(struct sockaddr_in);
    
    if(recvfrom(socketDescriptor, message_buffer, MAXLEN, 0, (struct sockaddr*) &clientAddress, &clientMessageLength) < 0) {
        fprintf(stderr, "Can't receive data\n");
        exit(1);
    }
    
    if (strcmp(message_buffer, "ftp") == 0) {
        if (sendto(socketDescriptor, "yes", strlen("yes"), 0, (struct sockaddr*) &clientAddress, clientMessageLength) == -1) {
                fprintf(stderr, "Message not sent to client\n");
                exit(1);
        }
        else if (sendto(socketDescriptor, "no", strlen("no"), 0, (struct sockaddr*) &clientAddress, clientMessageLength) == -1) {
            fprintf(stderr, "Message not sent to client\n");
            exit(1);
        }
    }

    close(socketDescriptor);
    return 0;
}
