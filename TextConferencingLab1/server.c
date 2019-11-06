// //
// //  server.c
// //  FileTransferLab4
// //
// //  Created by Sagar Chadha & Pratiksha Shenoy on 2019-11-04.
// //  Copyright © 2019 Sagar Chadha & Pratiksha Shenoy. All rights reserved.
// //

// //#include <stdio.h>
// //#include <stdlib.h>
// //#include <string.h>
// //#include <sys/types.h>
// //#include <sys/socket.h>
// //#include <netinet/in.h>
// //#include <unistd.h>

// #include "packet.h"

// #define MAXLEN 1100
// #define MAXPACKETS 1000000

// //Checking if the packet sent is a duplicate in the log of packets
// bool checkForDuplicate(int packetLog[], int len, int fragmentNumber) {
//     for (int i = 0; i < len; ++i) {
//         if (packetLog[i] == fragmentNumber) return true;
//         else if (packetLog[i] == 0) {
//             packetLog[i] = fragmentNumber;
//             return false;
//         }
//     }
// }

// int main(int argc, char const *argv[]){
//     //Arguments include server and the port number
    
//     //Obtaining port from argument
//     int port;
//     switch(argc) {
//         case 2:
//             port = atoi(argv[1]);
//             break;
//         default:
//             fprintf(stderr, "usage: server (TCP listen port)\n");
//             exit(0);
//     }
    
//     int socketDescriptor;
    
//     //Creates a socket that delivers data
//     if ((socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
//         fprintf(stderr, "socket error\n");
//         exit(1);
//     }
//     printf("1\n");
//     struct sockaddr_in serverAddress;
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_port = htons(port);
//     serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
//     memset(serverAddress.sin_zero, 0, sizeof(serverAddress.sin_zero));
//     printf("2\n");
//     //Assigns address to the socket
//     if (bind(socketDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1 ) {
//         fprintf(stderr, "Can't bind name to socket\n");
//         exit(1);
//     }
//     listen(socketDescriptor, 5);
//     printf("3\n");
//     int client_socket = accept(socketDescriptor, (struct sockaddr *) &serverAddress, (socklen_t*)sizeof(serverAddress));
//     printf("4\n");
//     char message_buffer[MAXLEN] = {0};
//     struct sockaddr_in clientAddress;
//     unsigned clientMessageLength = sizeof(struct sockaddr_in);
    
//     //Receives message from client
//     printf("Server is waiting for message.\n");
// //    if(recvfrom(socketDescriptor, message_buffer, MAXLEN, 0, (struct sockaddr*) &clientAddress, &clientMessageLength) < 0) {
// //        fprintf(stderr, "Can't receive data\n");
// //        exit(1);
// //    }
    
//     int command = 0;
//     while(command != EXIT){
//         // int bytesReceived = recvfrom(socketDescriptor, message_buffer, MAXLEN, 0, (struct sockaddr*) &clientAddress, &clientMessageLength);
//         recv(client_socket, message_buffer, 1024, 0);
//         // message_buffer[bytesReceived] = '\0';
//         struct packet* currentPacket = extractPacket(message_buffer);
//         command = currentPacket->type;
//         printf("1\n");
//         if (command == LOGIN) {
//             //Sending a login acknowledge response
//             printf("2\n");
//             // if (sendto(socketDescriptor, "LO_ACK", strlen("LO_ACK"), 0, (struct sockaddr*) &clientAddress, clientMessageLength) == -1) {
//             //     fprintf(stderr, "Acknowledgement message not sent to client\n");
//             //     exit(1);
//             // }
//             send(client_socket, "LO_ACK", strlen("LO_ACK"), 0);
//         }
//         else if (command == EXIT) {
//             //Sending a login acknowledge response
//             printf("3\n");
//             // if (sendto(socketDescriptor, "EXITING", strlen("EXITING"), 0, (struct sockaddr*) &clientAddress, clientMessageLength) == -1) {
//             //     fprintf(stderr, "Acknowledgement message not sent to client\n");
//             //     exit(1);
//             // }
//             send(client_socket, "EXITING", strlen("EXITING"), 0);
//         }
        
//         free(currentPacket);
//     }
//     //Closes socket
//     close(socketDescriptor);
//     return 0;
// }

#include "packet.h"
#define PORT 8080 
int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    if (listen(server_fd, 7) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
    valread = read( new_socket , buffer, 1024); 
    printf("%s\n",buffer ); 
    send(new_socket , hello , strlen(hello) , 0 ); 
    printf("Hello message sent\n"); 
    close(server_fd);
    return 0; 
} 