//
//  server.c
//  FileTransferLab4
//
//  Created by Sagar Chadha & Pratiksha Shenoy on 2019-11-04.
//  Copyright © 2019 Sagar Chadha & Pratiksha Shenoy. All rights reserved.
//

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <unistd.h>

#include "packet.h"

#define MAXLEN 1100
#define MAXPACKETS 1000000

//Checking if the packet sent is a duplicate in the log of packets
bool checkForDuplicate(int packetLog[], int len, int fragmentNumber) {
    for (int i = 0; i < len; ++i) {
        if (packetLog[i] == fragmentNumber) return true;
        else if (packetLog[i] == 0) {
            packetLog[i] = fragmentNumber;
            return false;
        }
    }
}

int main(int argc, char const *argv[]){
    //Arguments include server and the port number
    
    //Obtaining port from argument
    int port;
    switch(argc) {
        case 2:
            port = atoi(argv[1]);
            break;
        default:
            fprintf(stderr, "usage: server (TCP listen port)\n");
            exit(0);
    }
    
    int socketDescriptor;
    
    //Creates a socket that delivers data
    if ((socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        fprintf(stderr, "socket error\n");
        exit(1);
    }
    
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(serverAddress.sin_zero, 0, sizeof(serverAddress.sin_zero));
    
    //Assigns address to the socket
    if (bind(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1 ) {
        fprintf(stderr, "Can't bind name to socket\n");
        exit(1);
    }
    listen(socketDescriptor, 10);
    
    int client_socket = accept(socketDescriptor, NULL, NULL);
    
    char message_buffer[MAXLEN] = {0};
    struct sockaddr_in clientAddress;
    unsigned clientMessageLength = sizeof(struct sockaddr_in);
    
    //Receives message from client
    fprintf(stdout, "Server is waiting for message.\n");
    if(recvfrom(socketDescriptor, message_buffer, MAXLEN, 0, (struct sockaddr*) &clientAddress, &clientMessageLength) < 0) {
        fprintf(stderr, "Can't receive data\n");
        exit(1);
    }
    
    //Compares message from client to then send message
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
    
    int command = 0;
    while(command == EXIT){
        int bytesReceived = recvfrom(socketDescriptor, message_buffer, MAXLEN, 0, (struct sockaddr*) &clientAddress, &clientMessageLength);
        message_buffer[bytesReceived] = '\0';
        struct packet* currentPacket = extract_packet(message_buffer);
        
        command = currentPacket->type;
        
        if (currentPacket->type == LOGIN) {
            //Sending a login acknowledge response
            if (sendto(socketDescriptor, "LO_ACK", strlen("LO_ACK"), 0, (struct sockaddr*) &clientAddress, clientMessageLength) == -1) {
                fprintf(stderr, "Acknowledgement message not sent to client\n");
                exit(1);
            }
        }
        else if (currentPacket->type == EXIT) {
            //Sending a login acknowledge response
            if (sendto(socketDescriptor, "EXITING", strlen("EXITING"), 0, (struct sockaddr*) &clientAddress, clientMessageLength) == -1) {
                fprintf(stderr, "Acknowledgement message not sent to client\n");
                exit(1);
            }
        }
        
        free(currentPacket);
    }
    //Closes socket
    close(socketDescriptor);
    return 0;
}
