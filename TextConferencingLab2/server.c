#include "data.h"
#define PORT 3000 
#define MAXLEN 1024
#define MAXCLIENTS 5

bool find_client(char* clientID_str, char* password_str);

int main(int argc, char const *argv[]) { 
    //int port = 4095;
    
    //Obtaining the port id from the user input
    int port;
    switch(argc) {
        case 2:
            port = atoi(argv[1]);
            break;
        default:
            fprintf(stderr, "Use the format: ./server <Port Number>\n");
            exit(0);
    }
    printf("%d", &port);
    
    open("output", O_CREAT|O_WRONLY, 0777);
    //Initializing all of the variables
    int server_socket, client_socket, read_value; 
    struct sockaddr_in server_address; 
    int addrlen = sizeof(server_address); 
    int option = 1;
    char message_buffer[MAXLEN] = {0};  
    int command = 0;
    struct account_info* account_list = NULL;
    struct session* session_list = NULL;
    
    //Set of file descriptors
    fd_set read_fds, fds;
    int max_sd, sd;
    int client_socket_list[MAXCLIENTS];

    // Creating socket file descriptor 
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 

    //Ensuring that the socket is available
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    
    //Setting the conditions for the server address
    server_address.sin_family = AF_INET; 
    server_address.sin_addr.s_addr = INADDR_ANY; 
    server_address.sin_port = htons(port); 
    
    //Binding the socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    //The socket is listening
    if (listen(server_socket, 7) < 0) { 
        perror("Error: Listen\n"); 
        exit(EXIT_FAILURE); 
    } 

    while(true){
        //Initializing the file descriptors
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        max_sd = server_socket;

        //Adding the list of client sockets to the file descriptor
        int i = 0;
        for (i = 0; i < MAXCLIENTS; ++i){
            sd = client_socket_list[i];
            if (sd > 0) FD_SET(sd, &read_fds);
            if (sd > max_sd) max_sd = sd;
        }
        
        //Monitoring the file descriptors
        if (select(max_sd + 1, &read_fds, NULL, NULL, NULL) < 0){
            perror("Error: Select\n"); 
            exit(EXIT_FAILURE); 
        }
        
        //Checking if there is activity on the server socket (only with a new connection)
        if (FD_ISSET(server_socket, &read_fds)){
            //Accepting the socket connection
            if ((client_socket = accept(server_socket, (struct sockaddr *)&server_address, (socklen_t*)&addrlen)) < 0) { 
                perror("Error: Accept\n"); 
                exit(EXIT_FAILURE); 
            } 

            //Reading in the data
            read(client_socket , message_buffer, MAXLEN); 
            
            //Extracting the data into packet form
            struct packet* currentPacket = extractPacket(message_buffer);
            printPacket(currentPacket);
            command = currentPacket->type;

            //Actions for the login command
            if (command == LOGIN) {
                //Store the data in an array so to avoid overwriting it
                char temp_buffer[MAXLEN];
                strcpy(temp_buffer, currentPacket->data);

                //Seperate the username and password from the data           
                char username[MAXLEN], password[MAXLEN];
                sscanf(temp_buffer, "%[^,],%s", username, password);
                
                //Check to see if the client is registered in the database
                if (find_client(username, password)) {
                    //Add the client into linked list of accounts
                    struct account_info* new_account = create_account(username, password, client_socket);
                    account_list = add_to_account_list(account_list, new_account);

                    //Creating a packet of acknowledgement of login to the client
                    struct packet* pack = malloc(sizeof(struct packet));
                    pack->type = LO_ACK;
                    strcpy(pack->source, "Server");
                    strcpy(pack->data, "0");
                    pack->size = 0;

                    //Send the packet back to the client
                    send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 

                    //Adding the client socket to the list of client sockets
                    for (i = 0; i < MAXCLIENTS; ++i){
                        if (client_socket_list[i] == 0){
                            client_socket_list[i] = client_socket;
                            break;
                        }
                    }
                }
                else {
                    //Creating a packet to show that the user was not logged in
                    struct packet* pack = malloc(sizeof(struct packet));
                    pack->type = LO_NAK;
                    strcpy(pack->source, "Server");
                    strcpy(pack->data, "0");
                    pack->size = 0;

                    //Sending negative acknowledgement of login back to the client
                    send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                }
            }
        }

        //Iterating through the client sockets
        for (i = 0; i < MAXCLIENTS; ++i){
            client_socket = client_socket_list[i];
            
            //Check if there is any activity on a client socket
            if (FD_ISSET(client_socket, &read_fds)) {
                //Reading and extracting the message sent from the client
                read(client_socket, message_buffer, MAXLEN); 
                struct packet* currentPacket = extractPacket(message_buffer);
                printPacket(currentPacket);
                command = currentPacket->type;

                //The command was to create a new session
                if (command == NEW_SESS) {
                    //If the session already exists, then send a negative acknowledgement
                    if (search_session(session_list, currentPacket->data) != NULL){
                        struct packet* pack = malloc(sizeof(struct packet));
                        pack->type = NS_NAK;
                        strcpy(pack->source, "Server");
                        strcpy(pack->data, "The session already exists.");
                        pack->size = strlen(currentPacket->data);
                        send(client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0); 
                        continue;
                    }

                    //Create a session and add it to the linked list of sessions
                    struct session* new_session = create_session(currentPacket->data);
                    session_list = add_to_session_list(session_list, new_session);
                    
                    //Connect the session with the account by the information to the account and session linked-lists
                    struct account_info* new_account = search_account(account_list, currentPacket->source);
                    new_account->connected = true;
                    add_session_to_account(new_account, currentPacket->data);
                    session_list = add_account_to_session(session_list, new_account, currentPacket->data);
                    
                    //Sending an acknowledgement back to the client
                    struct packet* pack = malloc(sizeof(struct packet));
                    pack->type = NS_ACK;
                    strcpy(pack->source, "Server");
                    strcpy(pack->data, currentPacket->data);
                    pack->size = strlen(currentPacket->data);
                    send(client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0); 
                }
                //The command was to join a session
                else if (command == JOIN) {
                    //Search for the session in the linked list of sessions
                    struct session* current_session = search_session(session_list, currentPacket->data);
                    
                    //If the session does not exist, send a negative acknowledgement
                    if (current_session == NULL){
                        struct packet* pack = malloc(sizeof(struct packet));
                        pack->type = JN_NAK;
                        strcpy(pack->source, "Server");
                        strcpy(pack->data, "The session does not exist.");
                        pack->size = strlen(currentPacket->data);
                        send(client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0);
                        continue; 
                    }

                    //Connect the session with the account by the information to the account and session linked-lists
                    struct account_info* new_account = search_account(account_list, currentPacket->source);
                    new_account->connected = true;
                    add_session_to_account(new_account, currentPacket->data);
                    session_list = add_account_to_session(session_list, new_account, currentPacket->data);
                    
                    //Sending an acknowledgement back to the client
                    struct packet* pack = malloc(sizeof(struct packet));
                    pack->type = JN_ACK;
                    strcpy(pack->source, "Server");
                    strcpy(pack->data, currentPacket->data);
                    pack->size = strlen(currentPacket->data);
                    send(client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0); 
                }
                //The command was to leave a session
                else if (command == LEAVE_SESS) {
                    //If the session does not exist, send a negative acknowledgement
                    if (search_session(session_list, currentPacket->data) == NULL){
                        struct packet* pack = malloc(sizeof(struct packet));
                        pack->type = LEAVE_NAK;
                        strcpy(pack->source, "Server");
                        strcpy(pack->data, "The session does not exist.");
                        pack->size = strlen(currentPacket->data);
                        send(client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0); 
                        continue;
                    }
                    
                    //Searching for the account from the account linked list
                    struct packet* pack = malloc(sizeof(struct packet));
                    struct account_info* account = search_account(account_list, currentPacket->source);
                    
                    //Removing the account from the session information and vice versa 
                    if (remove_session_from_account(account, currentPacket->data) &&
                    remove_account_from_session(session_list, account->clientID, currentPacket->data)) {
                        pack->type = LEAVE_ACK;
                        strcpy(pack->data, currentPacket->data);
                    }
                    //The account was not a part of the session
                    else {
                        pack->type = LEAVE_NAK;
                        strcpy(pack->data, "You are not a part of this session.");
                    }

                    //If there are no more users in the session, remove the session
                    if (search_session(session_list, currentPacket->data)->user_list == NULL)
                        session_list = remove_session(session_list, currentPacket->data);

                    //Sending an acknowledgement back to the client
                    strcpy(pack->source, "Server");
                    pack->size = strlen(pack->data);
                    send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                }
                //The command was to list the active users and sessions
                else if (command == QUERY) {
                    //Creating a string to store the information
                    char list[MAXLEN];
                    memset(list, 0, MAXLEN);

                    //Beginning to list the active users
                    strcat(list,"Active Users\n");
                    struct account_info* current_account = account_list;

                    //Iterating through the list of accounts and adding to the list
                    while (current_account != NULL){
                        strcat(list, current_account->clientID);
                        strcat(list,"\n");
                        
                        //Iterating through the list of sessions that the account is a part of and adding to the list
                        struct session_id* current_session = current_account->session_id_list;
                        while (current_session != NULL){
                            strcat(list, current_session->session_id);
                            strcat(list, "\n");
                            current_session = current_session->next_session;
                        }
                        strcat(list, "\n");
                        current_account = current_account->next_account;
                    }

                    //Sending an acknowledgement and the list back to the client
                    struct packet* pack = malloc(sizeof(struct packet));
                    pack->type = QU_ACK;
                    strcpy(pack->source, "Server");
                    strcpy(pack->data, list);
                    pack->size = strlen(pack->data);
                    send(client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0); 
                }
                //The client wanted to send a message
                if (command == MESSAGE) {
                    //Created a temporary string so don't have to overwrite the data
                    char temp_buffer[MAXLEN];
                    memset(temp_buffer, 0, MAXLEN);
                    strcpy(temp_buffer, currentPacket->data);
                    
                    //Extract the message and the session from the data of the packet
                    char session_id[MAXLEN];
                    char message[MAXLEN]; 
                    strcpy(session_id, strtok(temp_buffer, " "));
                    strcpy(message, strtok(NULL, "\n"));
                    
                    //If the session does not exist, send a negative acknowledgement
                    if (search_session(session_list, session_id) == NULL){
                        struct packet* pack = malloc(sizeof(struct packet));
                        pack->type = MESSAGE_NAK;
                        strcpy(pack->source, "Server");
                        strcpy(pack->data, "The session does not exist.");
                        pack->size = strlen(pack->data);
                        send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                        continue;
                    }

                    //Obtain the account information
                    struct account_info* receiver_account = search_account(account_list, currentPacket->source);
                    
                    //If the account is not a part of the specified session, send a negative acknowledgement
                    if (!search_session_from_account(receiver_account, session_id)){
                        struct packet* pack = malloc(sizeof(struct packet));
                        pack->type = MESSAGE_NAK;
                        strcpy(pack->source, "Server");
                        strcpy(pack->data, "You are not an active user of that session.");
                        pack->size = strlen(pack->data);
                        send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                        continue;
                    }

                    //Formatting the message to be sent to the other clients (~<session> <message>)
                    memset(temp_buffer, 0, MAXLEN);
                    strcpy(temp_buffer, "~");
                    strcat(temp_buffer, session_id);
                    strcat(temp_buffer, " ");
                    strcat(temp_buffer, message);
                    
                    //Creating the packet to send to the other accounts in the session
                    struct packet* pack = malloc(sizeof(struct packet));
                    pack->type = MESSAGE;
                    strcpy(pack->source, "Server");
                    strcpy(pack->data, temp_buffer);
                    pack->size = strlen(temp_buffer);

                    //Search for the session and get the account list of that session
                    struct session* current_session = search_session(session_list, session_id);
                    struct account_info* current_account = current_session->user_list;
                    
                    //Iterating through the accounts and forwarding the message
                    while(current_account != NULL){
                        //Check to not send the message back to the sender of the message
                        if (current_account->clientID != currentPacket->source)
                            send(current_account->client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0); 
                        current_account = current_account->next_account;
                    }

                    //Sending an acknowledgement to the sender
                    pack->type = MESSAGE_ACK;
                    send(client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0);
                }
                //The command was to invite another account into a session
                if (command == INVITE) {
                    //Create a list so the data isn't overwritten
                    char temp_buffer[MAXLEN];
                    memset(temp_buffer, 0, MAXLEN);
                    strcpy(temp_buffer, currentPacket->data);

                    //Extract the session and the account    
                    char session_id[MAXLEN];
                    char user[MAXLEN]; 
                    strcpy(session_id, strtok(temp_buffer, " "));
                    strcpy(user, strtok(NULL, "\n"));
                    
                    //Get the account information of the account to be invited
                    struct account_info* invited_account = search_account(account_list, user);
                    
                    //If the invited account does not exist, send a negative acknowledgement
                    if (invited_account == NULL){
                        struct packet* pack = malloc(sizeof(struct packet));
                        pack->type = INVITE_REFUSAL;
                        strcpy(pack->source, "Server");
                        strcpy(pack->data, "The user does not exist.");
                        pack->size = strlen("The user does not exist.");
                        send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                        continue;
                    }

                    //Get the session information
                    struct session* invited_session = search_session(session_list, session_id);
                    
                    //If the session does not exist, send a negative acknowledgement
                    if (invited_session == NULL){
                        struct packet* pack = malloc(sizeof(struct packet));
                        pack->type = INVITE_REFUSAL;
                        strcpy(pack->source, "Server");
                        strcpy(pack->data, "The session does not exist.");
                        pack->size = strlen("The session does not exist.");
                        send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 
                        continue;
                    }
                    
                    //If the user is already registered for that session, send a negative acknowledgement
                    if (search_session_from_account(invited_account, session_id)){
                        struct packet* pack = malloc(sizeof(struct packet));
                        pack->type = INVITE_REFUSAL;
                        strcpy(pack->source, "Server");
                        strcpy(pack->data, "The user is already registered in that session.");
                        pack->size = strlen("The user is already registered in that session.");
                        send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0); 
                        continue;
                    }

                    //Creating the packet to send to the invited account
                    struct packet* pack = malloc(sizeof(struct packet));
                    pack->type = INVITE;
                    strcpy(pack->source, currentPacket->source);
                    
                    //Formatting the message that is send to the invited account
                    char invite_message[MAXLEN];
                    strcpy(invite_message, currentPacket->source);
                    strcat(invite_message, " has sent you an invite to join ");
                    strcat(invite_message, session_id);
                    strcat(invite_message, ". Would you like to join? (yes/no)");
                    strcpy(pack->data, invite_message);
                    pack->size = strlen(pack->data);

                    //Send the message to the invited account
                    send(invited_account->client_socket, compressPacket(pack), strlen(compressPacket(pack)), 0); 
                    
                    //Read the response from the invited account
                    read(invited_account->client_socket, message_buffer, MAXLEN);

                    //Forward the message back to the sender of the invite 
                    send(client_socket, message_buffer, strlen(message_buffer), 0);

                    //Extract the informaiton from the invited account into the packet
                    struct packet* invitee_packet = extractPacket(message_buffer);

                    //If the invited account accepted the invite, add the account to the session
                    if (invitee_packet->type == INVITE_ACCEPT) {
                        invited_account->connected = true;
                        add_session_to_account(invited_account, session_id);
                        session_list = add_account_to_session(session_list, invited_account, session_id);
                    }
                }
                //The command was to exit the appliaction (quit or logout)
                if (command == EXIT){
                    //Search for the account and remove all of the information related to the account
                    //from the session and account linked lists
                    struct account_info* current_account = search_account(account_list, currentPacket->source);
                    remove_account_from_all_sessions(session_list, current_account);
                    remove_all_sessions_from_account(session_list, current_account);
                    remove_account(account_list, currentPacket->source);
                    
                    //Send an acknowledgement to the account
                    struct packet* pack = malloc(sizeof(struct packet));
                    pack->type = EXIT;
                    strcpy(pack->source, "Server");
                    strcpy(pack->data, currentPacket->data);
                    pack->size = strlen(currentPacket->data);
                    send(client_socket , compressPacket(pack) , strlen(compressPacket(pack)) , 0 ); 

                    //Closing the socket related to that account
                    close(client_socket);
                    client_socket_list[i] = 0;
                }
            }
        }
    }
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
