#include "pipe_networking.h"
#include "server.h"

#include <signal.h>
#include <ctype.h>

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove("luigi");
    exit(0);
  }
}

int main() {
  signal(SIGINT, sighandler);
  
  int from_client;

  while (1) {
    from_client = server_setup();

    if (fork() == 0) {
      subserver(from_client);
      printf("subserver done\n");
      exit(0);
    } else {
      close(from_client);
    }
  }
}

void subserver(int from_client) {
  int to_client = server_connect(from_client);
  char buf[BUFFER_SIZE];

  while (read(from_client, buf, sizeof(buf))) {
    process(buf);
    write(to_client, buf, sizeof(buf));
  }
}

void process(char * s) {
  
  if (strcmp(s, "help") == 0) {
    help();
  } else if (strcmp(s, "create") == 0) {
    create();
  } else if (strcmp(s, "read") == 0) {
    read_story();
  } else if (strcmp(s, "edit") == 0) {
    edit();
  }
}

void help() {

}

void create() {

}

void read_story() {

}

void edit() {

}
