#include "packet.h"
#define PORT 3000 
#define MAXLEN 1000
#define TIMEOUT 1
   
int main(int argc, char const *argv[]) 
{ 
    //Initializing the variables
    int client_socket = 0; 
    struct sockaddr_in server_address; 
    char message_buffer[MAXLEN] = {0}; 
    char clientID[MAXLEN];

    char command_buffer[MAXLEN];
    bool loggedIn = false;
    char* command;

    fd_set read_fds, input_fds;
    
    //Setting the value for the timeout 
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    while(1) {
        //If the user is logged in, need to monitor the socket in case server sends packets
        while (loggedIn) {
            //Setting the file descriptor
            FD_ZERO(&input_fds);
            FD_SET(client_socket, &input_fds);
            
            //Monitoring the socket
            int select_num = select(client_socket + 1, &input_fds, NULL, NULL, &timeout);
            if (select_num < 0){
                perror("Error: Select\n"); 
                exit(EXIT_FAILURE); 
            }
            //If there is a timeout, break out
            if (select_num == 0){
                break;
            }

            if (select_num != 0){
                //Read and extract the information from the server
                read(client_socket, message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                //If the packet was a forwarded message, print the message
                if (rec_pack->type == MESSAGE){
                    printf("%s\n", rec_pack->data);
                }
                //If the packet was an invite to join a session
                else if (rec_pack->type == INVITE){
                    while(1){
                        //Print the data of the packet
                        printf("%s\n", rec_pack->data);
                        
                        //Get the response of the account
                        char* invite_Response;
                        fgets(command_buffer, MAXLEN, stdin);
                        invite_Response = strtok(command_buffer, "\n");
                        
                        //If the response was 'yes', send the acknowledgement back to the server
                        if (strcmp(invite_Response, "yes\0") == 0) {
                            strcpy(rec_pack->source, clientID);
                            rec_pack->type = INVITE_ACCEPT;
                            send(client_socket, compressPacket(rec_pack), strlen(compressPacket(rec_pack)), 0); 
                            break;
                        }
                        //If the response was 'yes', send the negative acknowledgement back to the server
                        else if (strcmp(invite_Response, "no\0") == 0) {
                            strcpy(rec_pack->source, clientID);
                            rec_pack->type = INVITE_REFUSAL;
                            strcpy(rec_pack->data, "Your invite has been rejected.");
                            send(client_socket, compressPacket(rec_pack), strlen(compressPacket(rec_pack)), 0); 
                            break;
                        }
                    }
                }
            }
        }
        
        //Setting a file descriptor
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
            
            //Login command
            if (strcmp(command, "/login") == 0) {
                //Check to make sure the user isn't already logged in
                if (loggedIn) {
                    printf("Error: Cannot login to multiple users.\n");
                    continue;
                }

                //Extract the data from the input
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

                //Creating the socket for the client
                if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                { 
                    printf("\n Socket creation error \n"); 
                    return -1; 
                } 

                //Adding the proper configuration
                server_address.sin_family = AF_INET; 
                server_address.sin_port = htons(atoi(serverPort)); 
                
                // Convert IPv4 and IPv6 addresses from text to binary form 
                if(inet_pton(AF_INET, ipAddress, &server_address.sin_addr)<=0)  
                { 
                    printf("\nInvalid address/ Address not supported \n"); 
                    return -1; 
                } 

                //Connecting to the server
                if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
                { 
                    printf("\nConnection Failed \n"); 
                    return -1; 
                } 

                //Sending the login information to the server
                send(client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0); 

                //Reading deciphering the information sent back from the server
                read(client_socket, message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                //If the server has sent back an acknowledgement, the login is successful
                if (rec_pack->type == LO_ACK){
                    printf("%s successfully logged in\n", username);
                    loggedIn = true;
                    strcpy(clientID, username);
                }
                //Did not successfully login
                else {
                    printf("%s could not succesffuly login.\n", username);
                    break;
                }
            }
            //Creating a session command
            else if (strcmp(command, "/createsession") == 0 && loggedIn) {
                //Preparing the packet to be sent to the server
                char* sessionID = strtok(NULL, "\n");
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = NEW_SESS;
                strcpy(pack->source, clientID);
                strcpy(pack->data, sessionID);
                pack->size = strlen(sessionID);

                //Sending the packet to the server
                send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                
                //Reading the packet from the server and extracting it
                read(client_socket , message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                //Acknowledgement is sent and session is created
                if (rec_pack->type == NS_ACK){
                    printf("Successfully created session %s\n", sessionID);
                }
                else {
                    printf("%s\n", rec_pack->data);
                }
            }
            //Joining a session command
            else if (strcmp(command, "/joinsession") == 0 && loggedIn) {
                //Preparing the packet to send to the server
                char* sessionID = strtok(NULL, "\n");
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = JOIN;
                strcpy(pack->source, clientID);
                strcpy(pack->data, sessionID);
                pack->size = strlen(sessionID);

                //Sending the packet to the server
                send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                
                //Reading and extracting the packet from the server
                read(client_socket , message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                //Acknowledgement is received and the user has joined the session
                if (rec_pack->type == JN_ACK){
                    printf("Successfully joined session %s\n", sessionID);
                }
                else {
                    printf("%s\n", rec_pack->data);
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
                    printf("%s\n", rec_pack->data);
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
            else if (strcmp(command, "/invite") == 0 && loggedIn) {
                char* session_id = strtok(NULL, " ");
                char* user = strtok(NULL, "\n");
                
                char* temp_buffer = (char*)malloc(1+strlen(session_id)+strlen(user));
                strcpy(temp_buffer, session_id);
                strcat(temp_buffer, " ");
                strcat(temp_buffer, user);
                
                struct packet* pack = malloc(sizeof(struct packet));
                pack->type = INVITE;
                strcpy(pack->source, clientID);
                strcpy(pack->data, temp_buffer);
                pack->size = strlen(pack->data);

                send(client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0); 
                read(client_socket, message_buffer, MAXLEN); 
                struct packet* rec_pack = extractPacket(message_buffer);

                if (rec_pack->type == INVITE_ACCEPT){
                    printf("%s has accepted your invite.\n", rec_pack->source);
                }
                else if (rec_pack->type == INVITE_REFUSAL){
                    printf("%s\n", rec_pack->data);
                }
            }
            else if (loggedIn) {
                char* session_id = command;
                char* message = strtok(NULL, "\n");
                
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
                    printf("%s\n", rec_pack->data);
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