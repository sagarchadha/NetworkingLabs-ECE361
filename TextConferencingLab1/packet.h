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

//Creates the linked list of packets based on the file
struct packet * fileConvert(char * filename);

//Frees the memory that was used for the linked list
void freePackets(struct packet * root);

//Converting the packet to a string format for sending
char * compressPacket(struct packet * pack, int * len);

//Converting the packet to its individual elements from string format
struct packet * extractPacket(char * packet_str);

#endif
