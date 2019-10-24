//
//  packet.h
//  FileTransferLab2
//
//  Created by Sagar Chadha & Pratiksha Shenoy on 2019-10-07.
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

#define FRAGMENTSIZE 1000

struct packet {
    unsigned int totalFragments;        // Total number of fragments
    unsigned int fragmentNumber;        // Fragment number of packet
    unsigned int fragmentSize;          // Size of data in bytes
    char* fileName;                     //File that is being transferred
    char fileData[FRAGMENTSIZE];        //Data of the provided file
    struct packet * nextPacket;         //Pointer to the next packet
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
