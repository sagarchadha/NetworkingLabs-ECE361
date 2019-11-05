//
//  client.c
//  TextConferencingLab1
//
//  Created by Sagar Chadha & Pratiksha Shenoy on 2019-11-04
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

void login(char* command, char* clientID, char* password, char* ipAddress, char* serverPort);

int main(int argc, char const *argv[]){
    char commandBuffer[MAXLEN];
    bool loggedIn = false;
    
    while(1) {
        fgets(commandBuffer, MAXLEN, stdin);
        if (strcmp(commandBuffer, "\n") == 0) {
            printf("Please enter a valid command\n");
            continue;
        }
        
        char* command = strtok(commandBuffer, " ");
        
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
            
        }
        else if (strcmp(command, "logout") == 0) {
            printf("logout\n");
        }
        else if (strcmp(command, "list") == 0) {
            printf("list\n");
        }
        else if (strcmp(command, "createsession") == 0) {
            printf("createsession\n");
        }
        else {
            //Change this to be text
            printf("Error: Please enter a valid command\n");
        }
    }
    printf("User has quit the application.");
    return 0;
}

void login(char* command, char* clientID, char* password, char* ipAddress, char* serverPort) {
    return;
}
