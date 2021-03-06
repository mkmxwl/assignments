#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 60006

#define MAX_STR 96

// Function Prototypes
void readString(char*);
void readInt(int*);

void wait();
void talk();
void talkMode(char);