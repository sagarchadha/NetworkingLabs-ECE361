//
//  deliver.c
//  FileTransferLab2
//
//  Created by Sagar Chadha & Pratiksha Shenoy on 2019-10-07
//  Copyright © 2019 Sagar Chadha & Pratiksha Shenoy. All rights reserved.
//

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <unistd.h>
//#include <arpa/inet.h>
//#include <netdb.h> // includes addrinfo
//#include <ctype.h>

#include "packet.h"
#define MAXLEN 1000

int main(int argc, char const *argv[]){
    int port;
    
    //Obtaining port number from argument
    switch(argc) {
        case 3:
            port = atoi(argv[2]);
            break;
        default:
            fprintf(stderr, "usage: deliver -<server address> -<server port number>\n");
            exit(0);
    }
    char* ipAddress = argv[1];
    char* portPointer = argv[2];
    
    //Collecting the input and filename
    printf("Enter filename to transfer in the format: ftp <filename>\n");
    char ftpInput[50], fileName[50];
    scanf("%s %s", ftpInput, fileName);
    
    //Checking the input for ftp
    if(strcmp(ftpInput, "ftp")!= 0){
        printf("Invalid Command: %s\n", ftpInput);
        exit(0);
    }
    
    //Finding the file
    if(access(fileName, F_OK) != 0){
        fprintf(stderr, "File not found\n");
        exit(0);
    }
    
    //Setting up the variables
    int status, socketDescriptor;
    struct addrinfo hints;
    struct addrinfo *serverAddress;
    struct sockaddr_storage serverSocketAddress;     //connector's addr info
    
    //Fills memory with value
    memset(&hints, 0, sizeof hints);
    
    //Specifies UDP connection for data transfer
    hints.ai_family = AF_UNSPEC;        //IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM;     //Connectionless data transfer of fixed max length datagrams
    hints.ai_protocol = IPPROTO_UDP;    //UDP
    
    //Obtains address info of the server
    status = getaddrinfo(ipAddress, portPointer, &hints, &serverAddress);
    
    //Creating the UDP socket
    if ((socketDescriptor = socket(serverAddress->ai_family,serverAddress->ai_socktype, serverAddress->ai_protocol)) == -1) {
        fprintf(stderr, "Error with socket\n");
        exit(1);
    }
    
    //message variables
    char messageReceived[MAXLEN], *ftpResponse;
    int bytesRecveived;
    
    //Beginning the clock to calculate round trip time
    clock_t startTime, endTime;
    startTime = clock();
    
    //Sending ftp response to the server
    ftpResponse = "ftp";
    sendto(socketDescriptor, ftpResponse, strlen(ftpResponse), 0, serverAddress->ai_addr, serverAddress->ai_addrlen);
    
    //Receiving response from the server
    socklen_t  addressLength = sizeof(struct sockaddr_storage);
    bytesRecveived = recvfrom(socketDescriptor, messageReceived, MAXLEN-1 , 0, (struct sockaddr *)&serverSocketAddress, &addressLength);
    
    //Adding \0 for string comparison
    messageReceived[bytesRecveived] = '\0';
    
    //Calculating the RTT
    endTime = clock();
    double rtt = ((double) (endTime - startTime)/CLOCKS_PER_SEC);
    printf("RTT: %f seconds\n", rtt);
    
    //Checking the message from the server
    if (strcmp(messageReceived, "yes") == 0) {
        printf("A file transfer can start.\n");
    }
    
    //*********Sending Packets**********
    struct packet* rootPacket = fileConvert(fileName);
    struct packet* currentPacket = rootPacket;
    int length;
    
    
    while(currentPacket != NULL) {
        char* compressedPacket = compressPacket(currentPacket, &length);
        
        //Sending the packet
        bytesRecveived = sendto(socketDescriptor, compressedPacket, length, 0, serverAddress->ai_addr, serverAddress->ai_addrlen);
        
        //Receiving packets
        bytesRecveived = recvfrom(socketDescriptor, messageReceived, MAXLEN - 1, 0, (struct sockaddr *)&serverSocketAddress, &addressLength);
        
        //Adding \0 for string comparison
        messageReceived[bytesRecveived] = '\0';
        
        //Checking to see if the packets have been acknowledged
        if (strcmp(messageReceived, "ACK") != 0)
            continue;
        printf("Packet %d has been sent.\n", currentPacket->fragmentNumber);
        
        //Go to next packet in the the linked list and free the current packet
        currentPacket = currentPacket->nextPacket;
        free(compressedPacket);
    }
    
    //Freeing the packet struct
    freePackets(rootPacket);
    
    //Freeing the struct and closing the socket
    freeaddrinfo(serverAddress);
    close(socketDescriptor);
    return 0;
}
