//
//  data.h
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
    struct session_id* session_id_list;
    int client_socket;
};

struct session_id{
    char session_id[MAX_DATA];
    struct session_id* next_session;
};

//Creating the user account
struct account_info* create_account(char* id, char* p, int sd){
    struct account_info* new_account = (struct account_info*)malloc(sizeof(struct account_info));
    strcpy(new_account->clientID,id);
    strcpy(new_account->password, p);
    new_account->connected = false;
    new_account->next_account = NULL;
    new_account->session_id_list = NULL;
    new_account->client_socket = sd;
    return new_account;
}

//Copy an account
struct account_info* copy_account(struct account_info* old_account) {
    struct account_info* new_account = (struct account_info*)malloc(sizeof(struct account_info));
    strcpy(new_account->clientID, old_account->clientID);
    strcpy(new_account->password, old_account->password);
    new_account->connected = old_account->connected;
    new_account->next_account = NULL;
    new_account->client_socket = old_account->client_socket;
    return new_account;
}

//Search in the account log
struct account_info* search_account(struct account_info* root, char* id){
    struct account_info* current_account = root;
    while (current_account != NULL) {
        if (strcmp(current_account->clientID, id) == 0){
            //struct account_info* new_account = copy_account(current_account);
            struct account_info* new_account = current_account;
            return new_account;
        }
        current_account = current_account->next_account;
    }
    return NULL;
}

//Check if a user is within a session
bool search_session_from_account(struct account_info* account, char* id){
    struct session_id* current_session = account->session_id_list;
    while(current_session != NULL){
        if (strcmp(current_session->session_id, id) == 0)
            return true;
        current_session = current_session->next_session;
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

//Adds a session to the list of sessions that the account is in
void add_session_to_account(struct account_info* account, char* id){
    if (account->session_id_list == NULL) {
        struct session_id* new_session = (struct session*)malloc(sizeof(struct session_id));
        strcpy(new_session->session_id, id);
        new_session->next_session = NULL;
        account->session_id_list = new_session;
    }
    else {
        struct session_id* current_session = account->session_id_list;
        while (current_session->next_session != NULL) {
            current_session->next_session;
        }
        struct session_id* new_session = (struct session*)malloc(sizeof(struct session_id));
        strcpy(new_session->session_id, id);
        new_session->next_session = NULL;
        current_session->next_session = new_session;
    }
}

//Removes a session from the list of sessions that the account is in
bool remove_session_from_account(struct account_info* account, char* id){
    struct session_id* current_session = account->session_id_list;
    struct session_id* previous_session;

    if (current_session == NULL) return false;

    if (current_session->next_session == NULL && strcmp(current_session->session_id, id) == 0){
        free(current_session);
        account->session_id_list = NULL;
        account->connected = false;
        return true;
    } 

    while (current_session->next_session != NULL) {
        if (strcmp(current_session->session_id, id) == 0){
            break;
        }
        previous_session = current_session;
        current_session = current_session->next_session;
    }

    if (strcmp(current_session->session_id, id) == 0) {
        previous_session->next_session = current_session->next_session;
        free(current_session);
        return true;
    }
    return false;
}

//Remove an account from the account list
struct account_info* remove_account(struct account_info* root, char* id) {
    struct account_info* current_account = root;
    struct account_info* previous_account;

    if (strcmp(current_account->clientID, id) == 0) {
        root = current_account->next_account;
        return root;
    }

    //Iterate through list and find
    while (current_account->next_account != NULL){
        if (strcmp(current_account->clientID, id) == 0){
            break;
        }
        previous_account = current_account;
        current_account = current_account->next_account;
    }

    previous_account->next_account = current_account->next_account;
    free(current_account);
    return root;
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

//Search for a session in the list of sessions
struct session* search_session(struct session* root, char* id) {
    struct session* current_session = root;
    while (current_session != NULL) {
        if (strcmp(current_session->session_id, id) == 0)
            return current_session;
        else
            current_session = current_session->next_session;
    }
    return NULL;        
}

//Adding an account to a session
struct session* add_account_to_session(struct session* root, struct account_info* new_account, char* id) {
    struct session* current_session = search_session(root, id);
    if (current_session != NULL){
        if (current_session->user_list == NULL) {
            current_session->user_list = copy_account(new_account);
            current_session->user_list->connected = true;
            current_session->user_count = current_session->user_count + 1;
            current_session->active = true;
            return root;
        }

        struct account_info* current_account = current_session->user_list;
            
        while (current_account->next_account != NULL) {
            current_account = current_account->next_account;
        }
        
        current_account->next_account = copy_account(new_account);
        current_account->next_account->connected = true;
        current_session->user_count = current_session->user_count + 1;
        current_session->active = true;
        return root;
    }
    return NULL;
    // struct session* current_session = root;
    // while (current_session != NULL) {
    //     if (strcmp(current_session->session_id, id) == 0) {
    //         if (current_session->user_list == NULL) {
    //             current_session->user_list = copy_account(new_account);
    //             current_session->user_list->connected = true;
    //             //print_account_info(current_session->user_list);
    //             current_session->user_count = current_session->user_count + 1;
    //             current_session->active = true;
    //             return root;
    //         }

    //         struct account_info* current_account = current_session->user_list;
            
    //         while (current_account != NULL) {
    //             current_account = current_account->next_account;
    //         }
            
    //         current_account = copy_account(new_account);
    //         current_account->connected = true;
    //         //print_account_info(current_account);
    //         current_session->user_count = current_session->user_count + 1;
    //         current_session->active = true;
    //         return root;
    //     }
    //     else
    //         current_session = current_session->next_session;
    // }
    // return NULL;
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

//Removing an account from a session
bool remove_account_from_session(struct session* root, char* account_id, char* id){
    struct session* current_session = search_session(root, id);
    
    if (current_session == NULL) return false;

    struct account_info* current_account = current_session->user_list;
    struct account_info* previous_account;

    if (strcmp(current_account->clientID, account_id) == 0) {
        current_session->user_list = current_account->next_account;
        free(current_account);
        current_session->user_count = current_session->user_count - 1;
        return true;
    }

    while (current_account->next_account != NULL) {
        if (strcmp(current_account->clientID, account_id) == 0) {
            break;
        }
        previous_account = current_account;
        current_account = current_account->next_account;
    }
    if (strcmp(current_account->clientID, account_id) == 0) {
        previous_account->next_account = current_account->next_account;
        free(current_account);
        return true;
    }
    return false;
}

//Remove an account from all of the sessions that they are in
void remove_account_from_all_sessions(struct session* root, struct account_info* account){
    struct session_id* current_session = account->session_id_list;
    while (current_session != NULL) {
        remove_account_from_session(root, account->clientID, current_session->session_id);
        current_session = current_session->next_session;
    }
}

//Removes an account from the list of users that a session has
struct session* remove_session(struct session* root, char* id){
    struct session* current_session = root;
    struct session* previous_session;

    if (current_session == NULL) return NULL;
    
    if (strcmp(current_session->session_id, id) == 0) {
        root = current_session->next_session;
        free(current_session);
        return root;
    }

    while (current_session->next_session != NULL) {
        if (strcmp(current_session->session_id, id) == 0){
            break;
        }
        previous_session = current_session;
        current_session = current_session->next_session;
    }
    
    previous_session->next_session = current_session->next_session;
    free(current_session);
    return root;
}

//Remove all of the sessions associated with an account
void remove_all_sessions_from_account(struct session* root, struct account_info* account){
    struct session_id* current_session = account->session_id_list;
    struct session_id* following_session;
    
    while (current_session != NULL) {
        following_session = current_session->next_session;
        if (search_session(root, current_session->session_id)->user_list == NULL)
            root = remove_session(root, current_session->session_id);
        free(current_session);
        current_session = following_session;
    }
    account->session_id_list = NULL;
}

//Print the list of sessions and active users
void print_session_info(struct session* root) {
    if (root == NULL) {
        return;
    }
    printf("Session contents:\n");
    printf("Session ID: %s", root->session_id);
    printf("Num of Users: %d\n", root->user_count);
    printf("Client: %s\n", root->user_list->clientID);
    printf("Active Status: %d\n", root->active);
}

#endif