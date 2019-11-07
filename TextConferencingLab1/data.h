//
//  packet.h
//  TextConferencingLab1
//
//  Created by Sagar Chadha & Pratiksha Shenoy on 2019-11-04.
//  Copyright © 2019 Sagar Chadha & Pratiksha Shenoy. All rights reserved.
//

#ifndef DATA_H
#define DATA_H

#include "packet.h"
#define MAX_DATA 1024

//Session Log Struct
struct session {
    char session_id[MAX_DATA];
    int user_count;
    struct account_info* user_list;
    bool active;
    struct session* next_session;
};

//Account Info Struct
struct account_info{
    char clientID[MAX_DATA];
    char password[MAX_DATA];
    bool connected;
    struct account_info* next_account;
};

//Creating the user account
struct account_info* create_account(char* id, char* p){
    struct account_info* new_account = (struct account_info*)malloc(sizeof(struct account_info));
    strcpy(new_account->clientID,id);
    strcpy(new_account->password, p);
    new_account->connected = false;
    new_account->next_account = NULL;
    return new_account;
}

//Search in the account log
bool search_account(struct account_info* root, char* id){
    struct account_info* current_account = root;
    while (current_account != NULL) {
        if (strcmp(current_account->clientID, id) == 0){
            return true;
        }
    }
    return false;
}

//Adding a user to the list
struct account_info* add_to_account_list(struct account_info* root, struct account_info* new_account){
    if (root == NULL) {
        root = new_account;
        return root;
    }

    struct account_info* curret_account = root;
    while (curret_account->next_account != NULL){
        curret_account = curret_account->next_account;
    }
    curret_account->next_account = new_account;
    
    return root;
}

//Prints all of the account details
void print_account_info(struct account_info* account){
    printf("Client ID: %s\n", account->clientID);
    printf("Password: %s\n", account->password);
    if (account->connected)
        printf("User is connected to a session\n");
    else
        printf("User is not connected to a session\n");
}

struct session* create_session(char* id) {
    struct session* new_session = (struct session*)malloc(sizeof(struct session));
    strcpy(new_session->session_id, id);
    new_session->user_count = 0;
    new_session->active = false;
    new_session->user_list = NULL;
    new_session->next_session = NULL;
    return new_session;
}

//Adding an account to a session
struct session* add_account_to_session(struct session* root, struct account_info* new_account, char* id) {
    struct session* current_session = root;
    while (current_session != NULL) {
        if (strcmp(current_session->session_id, id) == 0) {
            struct account_info* current_account = current_session->user_list;
            
            while (current_account->next_account != NULL)
                current_account = current_account->next_account;
            
            current_account = new_account;
            current_session->user_count = current_session->user_count + 1;
            current_session->active = true;
            return root;
        }
    }
    return NULL;
}

//Adding a session to the linked list of sessions
struct session* add_to_session_list(struct session* root, struct session* new_session) {
    if (root == NULL) {
        root = new_session;
        return root;
    }

    struct session* curret_session = root;
    while (curret_session->next_session != NULL){
        curret_session = curret_session->next_session;
    }
    curret_session->next_session = new_session;
    
    return root;
}

//Print the list of sessions and active users
// char* print_session_users(struct session* root) {
//     if (root == NULL) {
//         return NULL;
//     }
//     char string_to_print[MAX_DATA];
    
//     struct session* curret_session = root;


// }


#endif
