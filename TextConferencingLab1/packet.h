//
//  packet.h
//  TextConferencingLab1
//
//  Created by Sagar Chadha & Pratiksha Shenoy on 2019-11-04.
//  Copyright © 2019 Sagar Chadha & Pratiksha Shenoy. All rights reserved.
//

#ifndef PACKET_H
#define PACKET_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>     //includes sockaddr
#include <netdb.h>          //includes addrinfo
#include <arpa/inet.h>      //includes inet_ntop etc...
#include <unistd.h>         //includes close()
#include <stdlib.h>         //includes exit(1)
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define MAX_SOURCE 64
#define MAX_DATA 1024

//Different types of messages
enum msgType {
    LOGIN,
    LO_ACK,
    LO_NAK,
    EXIT,
    JOIN,
    JN_ACK,
    JN_NAK,
    LEAVE_SESS,
    NEW_SESS,
    NS_ACK,
    MESSAGE,
    QUERY,
    QU_ACK
};

//Packet contents
struct packet {
    unsigned int type;                      //Type of message
    unsigned int size;                      //Size of data
    unsigned char source[MAX_SOURCE];       //Source of the packet
    unsigned char data[MAX_DATA];           //Data contained in the packet
};

//Prints all of the packet details (members of the packet struct)
void printPacket(struct packet * p);

//Prints all of the packets
void printAllPackets(struct packet * p);

//Frees the memory that was used for the linked list
void freePackets(struct packet * root);

//Converting the packet to a string format for sending
char * compressPacket(struct packet * pack, int * len){
    //Finding the size of each element to allocate spaace
    int s1 = snprintf(NULL, 0, "%d", pack->type);
    int s2 = snprintf(NULL, 0, "%d", pack->size);
    int s3 = strlen((char*)pack->source);
    int s4 = strlen((char*)pack->data);
    int total_size = s1+s2+s3+s4;
    
    //Allocating space for the string to be stored
    char * compressedPacket = malloc((total_size+4)*sizeof(char));
    
    //Converting and storing the packet details into the string
    sprintf(compressedPacket, "%d:%d:%s:%s:", pack->type, pack->size, pack->source, pack->data);
    return compressedPacket;
}

//Converting the packet to its individual elements from string format
struct packet * extractPacket(char * packet_str) {
    if (packet_str == NULL) return NULL;
    
    //Declaring Variables
    struct packet* extracted_Packet = malloc(sizeof(struct packet));
    char* type_str, size_str, source_str, data_str;

    //Splitting the string based on the the : delimeter
    type_str = strtok(packet_str, ":");
    size_str = strtok(NULL, ":");
    source_str = strtok(NULL, ":");
    data_str = strtok(NULL, ":");
    
    //Setting the members of the packet struct
    extracted_Packet->type = atoi(type_str);
    extracted_Packet->size = atoi(size_str);
    strcpy((char*)extracted_Packet->source,source_str);
    strcpy((char*)extracted_Packet->data,data_str);
    
    return extracted_Packet;
}

#endif
