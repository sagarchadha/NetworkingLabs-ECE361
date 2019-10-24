//
//  server.c
//  FileTransferLab3
//
//  Created by Sagar Chadha & Pratiksha Shenoy on 2019-10-23.
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
            fprintf(stderr, "usage: server (UDP listen port)\n");
            exit(0);
    }
    
    int socketDescriptor;
    
    //Creates a socket that delivers data
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

    FILE* filePointer;
    int flag = 1;
    
    int packetLog[MAXPACKETS] = {0};
    srand(time(0));
    while(flag) {
        //Receiving message from client
        int bytesReceived = recvfrom(socketDescriptor, message_buffer, MAXLEN, 0, (struct sockaddr*) &clientAddress, &clientMessageLength);
        message_buffer[bytesReceived] = '\0';
        
        //Creating the packet and extracting the message into packets
        struct packet* currentPacket = extractPacket(message_buffer);
        int totalFragments = currentPacket->totalFragments;
        int fragmentNumber = currentPacket->fragmentNumber;
        int fragmentSize = currentPacket->fragmentSize;
        char* fileName = currentPacket->fileName;
        char* fileData = currentPacket->fileData;
        printf("Received Packet %d\n", fragmentNumber);
        
        //Checking for if a duplicate packet is sent so doesn't write to the file again
        if (!checkForDuplicate(packetLog, MAXPACKETS, fragmentNumber)) {
            //First fragment should open the file and begin to write data in binary
            if(fragmentNumber == 1) {
                filePointer = fopen(fileName, "wb");
                printf("Opening File\n");
            }
            fwrite(fileData, 1, fragmentSize, filePointer);
        }
        else {
            printf("Received a duplicate packet\n");
        }
        
        //Sending an acknowledge response
        if (sendto(socketDescriptor, "ACK", strlen("ACK"), 0, (struct sockaddr*) &clientAddress, clientMessageLength) == -1) {
            fprintf(stderr, "Acknowledgement message not sent to client\n");
            exit(1);
        }
        
        //Freeing the current packet pointer
        free(currentPacket);
        printf("Freed Packet %d\n", fragmentNumber);
        //Reached the end of the fragment
        
        if (fragmentNumber == totalFragments) flag = 0;
    }
    fclose(filePointer);
    
    //Closes socket
    close(socketDescriptor);
    return 0;
}
