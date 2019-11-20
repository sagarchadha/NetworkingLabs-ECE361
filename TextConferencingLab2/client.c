#include "packet.h"
#define PORT 3000 
#define MAXLEN 1000
#define TIMEOUT 2
   
int main(int argc, char const *argv[]) 
{ 
    int client_socket = 0; 
    struct sockaddr_in server_address; 
    char message_buffer[MAXLEN] = {0}; 
    char clientID[MAXLEN];

    char command_buffer[MAXLEN];
    bool loggedIn = false;
    char* command;

    fd_set read_fds, input_fds;
    
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    while(1) {
        while (loggedIn) {
            FD_ZERO(&input_fds);
            FD_SET(client_socket, &input_fds);
            int select_num = select(client_socket + 1, &input_fds, NULL, NULL, &timeout);
            if (select_num < 0){
                perror("Error: Select\n"); 
                exit(EXIT_FAILURE); 
            }
            if (select_num == 0){
                break;
            }
            if (select_num != 0){
                read(client_socket, message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                if (rec_pack->type == MESSAGE){
                    printf("%s\n", rec_pack->data);
                }
                else {
                    printf("Could not read the message.\n");
                }
            }
        }
        
        FD_ZERO(&input_fds);
        FD_SET(0, &input_fds);

        if (select(0 + 1, &input_fds, NULL, NULL, &timeout) < 0){
            perror("Error: Select\n"); 
            exit(EXIT_FAILURE); 
        }

        if (FD_ISSET(0, &input_fds)){
            fgets(command_buffer, MAXLEN, stdin);
            if (strcmp(command_buffer, "\n") == 0) {
                printf("Please enter a valid command\n");
                continue;
            }
            
            //Obtained the command from the input
            if (strchr(command_buffer, ' ') != NULL)
                command = strtok(command_buffer, " ");
            else
                command = strtok(command_buffer, "\n");
            
            if (strcmp(command, "/login") == 0) {
                if (loggedIn) {
                    printf("Error: Cannot login to multiple users.\n");
                    continue;
                }

                char* username = strtok(NULL, " ");
                char* password = strtok(NULL, " ");
                char* ipAddress = strtok(NULL, " ");
                char* serverPort = strtok(NULL, " ");
                
                //Convert the inputs to a packet
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = LOGIN;
                strcpy(pack->source, username);
                char* temp_buffer = (char*)malloc(1+strlen(username)+strlen(password));
                strcpy(temp_buffer, username);
                strcat(temp_buffer, ",");
                strcat(temp_buffer, password);
                strcpy(pack->data, temp_buffer);
                pack->size = strlen(temp_buffer);

                if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                { 
                    printf("\n Socket creation error \n"); 
                    return -1; 
                } 
            
                server_address.sin_family = AF_INET; 
                server_address.sin_port = htons(atoi(serverPort)); 
                
                // Convert IPv4 and IPv6 addresses from text to binary form 
                if(inet_pton(AF_INET, ipAddress, &server_address.sin_addr)<=0)  
                { 
                    printf("\nInvalid address/ Address not supported \n"); 
                    return -1; 
                } 
            
                if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
                { 
                    printf("\nConnection Failed \n"); 
                    return -1; 
                } 

                send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                read(client_socket , message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                if (rec_pack->type == LO_ACK){
                    printf("%s successfully logged in\n", username);
                    loggedIn = true;
                    strcpy(clientID, username);
                }
                else {
                    printf("%s could not succesffuly login.\n", username);
                    break;
                }
            }
            else if (strcmp(command, "/createsession") == 0 && loggedIn) {
                char* sessionID = strtok(NULL, "\n");
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = NEW_SESS;
                strcpy(pack->source, clientID);
                strcpy(pack->data, sessionID);
                pack->size = strlen(sessionID);

                send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                read(client_socket , message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                if (rec_pack->type == NS_ACK){
                    printf("Successfully created session %s\n", sessionID);
                }
                else {
                    printf("Could not successfully create session %s\n", sessionID);
                }
            }
            else if (strcmp(command, "/joinsession") == 0 && loggedIn) {
                char* sessionID = strtok(NULL, "\n");
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = JOIN;
                strcpy(pack->source, clientID);
                strcpy(pack->data, sessionID);
                pack->size = strlen(sessionID);

                send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                read(client_socket , message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                if (rec_pack->type == JN_ACK){
                    printf("Successfully joined session %s\n", sessionID);
                }
                else {
                    printf("Could not successfully join session %s\n", sessionID);
                }
            }
            else if (strcmp(command, "/leavesession") == 0 && loggedIn) {
                char* sessionID = strtok(NULL, "\n");
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = LEAVE_SESS;
                strcpy(pack->source, clientID);
                strcpy(pack->data, sessionID);
                pack->size = strlen(sessionID);

                send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                read(client_socket , message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                if (rec_pack->type == LEAVE_ACK){
                    printf("Successfully left session %s\n", sessionID);
                }
                else {
                    printf("Could not successfully leave session %s\n", sessionID);
                }
            }
            else if (strcmp(command, "/list") == 0 && loggedIn) {
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = QUERY;
                strcpy(pack->source, clientID);
                strcpy(pack->data, "0");
                pack->size = strlen(pack->data);

                send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                read(client_socket , message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                if (rec_pack->type == QU_ACK){
                    printf("%s", rec_pack->data);
                }
                else {
                    printf("Could not send list of users and sessions\n");
                }
            }
            else if (strcmp(command, "/logout") == 0 && loggedIn) {
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = EXIT;
                strcpy(pack->source, clientID);
                strcpy(pack->data, "0");
                pack->size = strlen(pack->data);

                send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                read(client_socket , message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                if (rec_pack->type == EXIT){
                    printf("%s has successfully logged out\n", clientID);
                    loggedIn = false;
                    close(client_socket);
                    continue;
                }
                else {
                    printf("Could not successfully logout %s\n", clientID);
                }
            }
            else if (strcmp(command, "/quit") == 0 && loggedIn) {
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = EXIT;
                strcpy(pack->source, clientID);
                strcpy(pack->data, "0");
                pack->size = strlen(pack->data);

                send(client_socket, compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                read(client_socket, message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                if (rec_pack->type == EXIT){
                    printf("%s has successfully logged out\nExiting the application.\n", clientID);
                    loggedIn = false;
                    close(client_socket);
                    break;
                }
                else {
                    printf("Could not successfully quit %s\n", clientID);
                }
            }
            else if (loggedIn) {
                char* session_id = command;
                
                //strcpy(session_id, command);
                char* message = strtok(NULL, "\n");
                
                printf("Session: %s\n", session_id);
                printf("Message: %s\n", message);
                
                char* temp_buffer = (char*)malloc(1+strlen(session_id)+strlen(message));
                strcpy(temp_buffer, session_id);
                strcat(temp_buffer, " ");
                strcat(temp_buffer, message);
                
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = MESSAGE;
                strcpy(pack->source, clientID);
                strcpy(pack->data, temp_buffer);
                pack->size = strlen(pack->data);

                send(client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0); 
                read(client_socket, message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                if (rec_pack->type != MESSAGE){
                    printf("Error with sending the message.\n");
                }
                else{
                    printf("Successfully sent the message.\n");
                }
            }
            else {
                printf("Please login to use the application.\n");
            }
        }
    }
    close(client_socket);
    return 0; 
} 