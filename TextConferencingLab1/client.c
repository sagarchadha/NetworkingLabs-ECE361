// //
// //  client.c
// //  TextConferencingLab1
// //
// //  Created by Sagar Chadha & Pratiksha Shenoy on 2019-11-04
// //  Copyright © 2019 Sagar Chadha & Pratiksha Shenoy. All rights reserved.
// //

// //#include <stdio.h>
// //#include <stdlib.h>
// //#include <string.h>
// //#include <sys/types.h>
// //#include <sys/socket.h>
// //#include <netinet/in.h>
// //#include <unistd.h>
// //#include <arpa/inet.h>
// //#include <netdb.h> // includes addrinfo
// //#include <ctype.h>

// #include "packet.h"
// #define MAXLEN 1000

// bool find_client(char* clientID_str, char* password_str);
// void login(char* command, char* clientID, char* password, char* ipAddress, char* serverPort);

// int main(int argc, char const *argv[]){
//     char command_buffer[MAXLEN];
//     bool loggedIn = false;
    
//     //Setting up the variables
//     int status, socketDescriptor;
//     // struct addrinfo hints;
//     // struct addrinfo *serverAddress;
//     // struct sockaddr_storage serverSocketAddress;
//     struct sockaddr_in* serverAddress;
    
//     //message variables
//     char message_buffer[MAXLEN];
//     int bytesRecveived;
    
//     while(1) {
//         fgets(command_buffer, MAXLEN, stdin);
//         if (strcmp(command_buffer, "\n") == 0) {
//             printf("Please enter a valid command\n");
//             continue;
//         }
        
//         char* command = strtok(command_buffer, " ");
        
//         if (strcmp(command, "login") == 0) {
//             if (loggedIn) {
//                 printf("Error: Cannot login to multiple users.");
//                 continue;
//             }
            
//             printf("login\n");
//             loggedIn = true;
//             char* clientID = strtok(NULL, " ");
//             char* password = strtok(NULL, " ");
//             char* ipAddress = strtok(NULL, " ");
//             char* serverPort = strtok(NULL, " ");

//             serverAddress->sin_family =  AF_INET;
//             serverAddress->sin_port =  htons(3000);

//             if ((socketDescriptor = socket(serverAddress->sin_family, SOCK_STREAM, 0)) == -1) {
//                 fprintf(stderr, "Error with socket\n");
//                 exit(1);
//             }
//             printf("1\n");
//             inet_pton(serverAddress->sin_family, "127.0.0.1", &(serverAddress->sin_addr));
//             printf("1\n");
//             connect(socketDescriptor, (struct sockaddr *)serverAddress, sizeof(serverAddress));
//             printf("1\n");
//             send(socketDescriptor , "0:4:123:abcd:" , strlen("0:4:123:abcd:") , 0 ); 
//             printf("1\n");
//             read(socketDescriptor , message_buffer, 1024);
//             printf("%s\n", message_buffer);

// //            if (!find_client(clientID, password)) {
// //                printf("Did not find the user in the database.\n");
// //                continue;
// //            }
// //            char* data;
// //            strncat(data, clientID, MAXLEN);
// //            strncat(data, ":", MAXLEN);
// //            strncat(data, password, MAXLEN);
// //            strncat(data, ":", MAXLEN);
// //
// //            struct packet* currentPacket = malloc(sizeof(struct packet));
// //
// //            sprintf(message_buffer, "%d:%d:%s:%s:", LOGIN, pack->size, clientID, pack->data);
            
// //            struct packet* currentPacket = extractPacket("0:4:123:abcd:");
// //            printPacket(currentPacket);
// //            printf("%s", compressPacket(currentPacket));
            
// //            printf("1");
// //            memset(&hints, 0, sizeof hints);
// //            hints.ai_family = AF_UNSPEC;
// //            hints.ai_socktype = SOCK_STREAM;
// //            hints.ai_protocol = IPPROTO_TCP;

// // //            //Obtains address info of the server
// //             status = getaddrinfo("127.0.0.1", "3000", &hints, &serverAddress);
// //             printf("2");

// //             if ((socketDescriptor = socket(serverAddress->ai_family,serverAddress->ai_socktype, serverAddress->ai_protocol)) == -1) {
// //                 fprintf(stderr, "Error with socket\n");
// //                 exit(1);
// //             }
// //             printf("1");
// //             sendto(socketDescriptor, "0:4:123:abcd:", strlen("0:4:123:abcd:"), 0, serverAddress->ai_addr, serverAddress->ai_addrlen);

// //             printf("2");
            
// //             //Receiving response from the server
// //             socklen_t  addressLength = sizeof(struct sockaddr_storage);
// //             bytesRecveived = recvfrom(socketDescriptor, message_buffer, MAXLEN-1 , 0, (struct sockaddr *)&serverSocketAddress, &addressLength);
// //             printf("3");
//             //Adding \0 for string comparison
//             message_buffer[bytesRecveived] = '\0';
            
//             if (strcmp(message_buffer, "LO_ACK")) {
//                 printf("%s", message_buffer);
//             }
//             else {
//                 printf("%s", message_buffer);
//             }
//         }
//         else if (strcmp(command, "logout") == 0 && loggedIn) {
//             printf("logout\n");
//         }
//         else if (strcmp(command, "list" && loggedIn) == 0) {
//             printf("list\n");
//         }
//         else if (strcmp(command, "createsession" && loggedIn) == 0) {
//             printf("createsession\n");
//         }
//         else {
//             //Change this to be text
//             printf("Error: Please enter a valid command\n");
//         }
//     }
//     printf("User has quit the application.");
//     return 0;
// }

// //Searching for a client in the database
// bool find_client(char* clientID_str, char* password_str) {
//     char* file_name = "userData.txt";
//     char* file_buffer[MAXLEN];
//     FILE* file_pointer = fopen(file_name, "r");
//     bool found_user = false;
    
//     while (fscanf(file_pointer,"%s",file_buffer) == 1) {
//         if (strcmp(file_buffer, clientID_str) == 0) {
//             if (fscanf(file_pointer,"%s\n",file_buffer) == 1) {
//                 strncat(password_str, "\\", 1);
//                 if (strcmp(file_buffer, password_str) == 0) {
//                     found_user = true;
//                 }
//             }
//         }
//     }
//     return found_user;
// }

// void login(char* command, char* clientID, char* password, char* ipAddress, char* serverPort) {
//     return;
// }

#include "packet.h"
#define PORT 3000 
#define MAXLEN 1000
   
int main(int argc, char const *argv[]) 
{ 
    int client_socket = 0; 
    struct sockaddr_in server_address; 
    char message_buffer[MAXLEN] = {0}; 

    char command_buffer[MAXLEN];
    bool loggedIn = false;

    while(1) {
        fgets(command_buffer, MAXLEN, stdin);
        if (strcmp(command_buffer, "\n") == 0) {
            printf("Please enter a valid command\n");
            continue;
        }
        
        char* command = strtok(command_buffer, " ");
        
        if (strcmp(command, "login") == 0) {
            if (loggedIn) {
                printf("Error: Cannot login to multiple users.");
                continue;
            }
            
            printf("login\n");
            loggedIn = true;
            char* clientID = strtok(NULL, " ");
            char* password = strtok(NULL, " ");
            char* ipAddress = strtok(NULL, " ");
            char* serverPort = strtok(NULL, " ");

            if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
            { 
                printf("\n Socket creation error \n"); 
                return -1; 
            } 
        
            server_address.sin_family = AF_INET; 
            server_address.sin_port = htons(PORT); 
            
            // Convert IPv4 and IPv6 addresses from text to binary form 
            if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr)<=0)  
            { 
                printf("\nInvalid address/ Address not supported \n"); 
                return -1; 
            } 
        
            if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
            { 
                printf("\nConnection Failed \n"); 
                return -1; 
            } 
            send(client_socket , "0:1000:Sagar:Sagar,iAmAwesome:" , strlen("0:1000:Sagar:Sagar,iAmAwesome:") , 0 ); 
            read(client_socket , message_buffer, MAXLEN); 
            printf("%s\n", message_buffer);
            if (strcmp(message_buffer, "LO_ACK") == 0) {
                printf("Received %s", message_buffer);
                loggedIn = true;
            }
            else {
                printf("Did not receive LO_ACK. Error: %s\n", message_buffer);
            }
        }
    }
    printf("%s\n",message_buffer ); 
    close(client_socket);
    return 0; 
} 
