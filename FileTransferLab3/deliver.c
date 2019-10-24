//
//  deliver.c
//  FileTransferLab3
//
//  Created by Sagar Chadha & Pratiksha Shenoy on 2019-10-23
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

void settingTimeout(struct timeval* timeOut, double retimeout) {
    timeOut->tv_sec = retimeout/1;
    timeOut->tv_usec = (retimeout - timeOut->tv_sec)*1000000;
}

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
    
    //Initializing values for finding the timeout
    struct timeval timeOut;
    double samplertt, devrtt, retimeout;
    samplertt = rtt;
    devrtt = rtt/2;
    retimeout = samplertt + 4*devrtt;
    
    bool retransmissionFlag = false;
    
    double alpha = 0.125;
    double beta = 0.25;
    
    //Setting the file descriptor for receiving the packets
    fd_set readFileDescriptor;
    FD_ZERO(&readFileDescriptor);
    
    //Sending Packets
    struct packet* rootPacket = fileConvert(fileName);
    struct packet* currentPacket = rootPacket;
    int length;
    int timeOutCounter = 1;
    
    while(currentPacket != NULL) {
        char* compressedPacket = compressPacket(currentPacket, &length);
        
        //Sending the packet
        startTime = clock();
        bytesRecveived = sendto(socketDescriptor, compressedPacket, length, 0, serverAddress->ai_addr, serverAddress->ai_addrlen);
        
        //Monitoring the socket descriptor and setting the timeout
        settingTimeout(&timeOut, retimeout);
        FD_SET(socketDescriptor, &readFileDescriptor);
        select(socketDescriptor+1, &readFileDescriptor, NULL, NULL, &timeOut);
        
        //There is a timeout, so need to retransmit the packet
        if(!FD_ISSET(socketDescriptor, &readFileDescriptor)) {
            printf("Timeout Occurred: did not receive acknowledgement\n");
            retransmissionFlag = true;
            retimeout = retimeout*2;
            free(compressedPacket);
            timeOutCounter++;
            if (timeOutCounter >= 7) {
                printf("Timed out too many times. Ending the process.\n");
                return 0;
            }
            continue;
        }
        
        //Receiving packets
        bytesRecveived = recvfrom(socketDescriptor, messageReceived, MAXLEN - 1, 0, (struct sockaddr *)&serverSocketAddress, &addressLength);
        endTime = clock();
        
        //Adding \0 for string comparison
        messageReceived[bytesRecveived] = '\0';
        
        //Checking to see if the packets have been acknowledged, if not then retransmit
        if (strcmp(messageReceived, "ACK") != 0) {
            free(compressedPacket);
            retransmissionFlag = true;
            continue;
        }
        
        printf("Packet %d has been sent and acknowledged.\n", currentPacket->fragmentNumber);
        
        //Calculate new timeout values if did not need to retransmit
        if (retransmissionFlag) retransmissionFlag = false;
        else {
            rtt = ((double) (endTime - startTime)/CLOCKS_PER_SEC);
            devrtt = (1-beta)*devrtt + beta*fabs(samplertt - rtt);
            samplertt = (1-alpha)*samplertt + alpha*rtt;
            retimeout = samplertt + 4*devrtt;
            
            if (retimeout < 1) retimeout = 1;
        }
        
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
