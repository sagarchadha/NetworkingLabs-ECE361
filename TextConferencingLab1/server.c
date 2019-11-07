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
#define PORT 3000 
#define MAXLEN 1024

bool find_client(char* clientID_str, char* password_str);

int main(int argc, char const *argv[]) { 
    int server_socket, client_socket, read_value; 
    struct sockaddr_in server_address; 
    int addrlen = sizeof(server_address); 
    int option = 1;
    char message_buffer[MAXLEN] = {0};  
    int command = 0;
       
    // Creating socket file descriptor 
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    
    server_address.sin_family = AF_INET; 
    server_address.sin_addr.s_addr = INADDR_ANY; 
    server_address.sin_port = htons( PORT ); 
    
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    if (listen(server_socket, 7) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    if ((client_socket = accept(server_socket, (struct sockaddr *)&server_address, (socklen_t*)&addrlen)) < 0) { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 

    while(command != EXIT){
        read(client_socket , message_buffer, MAXLEN); 
        struct packet* currentPacket = extractPacket(message_buffer);
        printPacket(currentPacket);
        command = currentPacket->type;

        if (command == LOGIN) {
            char temp_buffer[MAXLEN];
            strcpy(temp_buffer, currentPacket->data);
                        
            char username[MAXLEN], password[MAXLEN];
            sscanf(temp_buffer, "%[^,],%s", username, password);
            if (find_client(username, password)) {
                //Add the client into a data structure
                // struct account_info* new_account = create_account(username, password);
                // account_list = add_to_account_list(account_list, new_account);
                // print_account_info(account_list);
                
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = LO_ACK;
                strcpy(pack->source, "Server");
                strcpy(pack->data, "0");
                pack->size = 0;

                send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 

                //send(client_socket, "LO_ACK", strlen("LO_ACK"), 0); 
            }
            else
                send(client_socket, "LO_NAK", strlen("LO_NAK"), 0); 
        }
        else if (command == EXIT){
            send(client_socket, "EXIT", strlen("EXIT"), 0);
            break;
        }
        else if (command == NEW_SESS) {
            send(client_socket, "NS_ACK", strlen("NS_ACK"), 0);
        }
    }
    close(server_socket);
    return 0; 
} 

//Searching for a client in the database
bool find_client(char* clientID_str, char* password_str) {
    char* file_name = "userData.txt";
    char* file_buffer[MAXLEN];
    FILE* file_pointer = fopen(file_name, "r");
    while (fscanf(file_pointer,"%s",file_buffer) == 1) {
        if (strcmp(file_buffer, clientID_str) == 0) {
            if (fscanf(file_pointer,"%s\n",file_buffer) == 1) {
                char password_copy[MAXLEN];
                strcpy(password_copy, password_str);
                strncat(password_copy, "\\", 1);
                if (strcmp(file_buffer, password_copy) == 0) {
                    return true;
                }
            }
        }
    }
    return false;
}