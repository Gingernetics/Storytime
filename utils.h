#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#ifndef UTILS_H
#define UTILS_H

#define BUFFER_SIZE 10000
#define PORT "9000"
#define TEST_IP "127.0.0.1"

void error_check(int i, char *s);
int server_setup();
int server_connect(int sd);
int client_setup(char * server);

#define no_story "There is no such story."
#define invalid_story "Invalid file. Please don't change directories or use directories."
#define no_text "<There is no text>"
#define editing_story "Someone is editing the story. Please wait."

char ** parse_args(char *line);

#endif
