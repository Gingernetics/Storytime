#include "pipe_networking.h"
#include "forking_server.h"

#include <signal.h>
#include <ctype.h>

void process(char *s);
void subserver(int from_client);

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
    } else {
      close(from_client);
    }
  }
}

void subserver(int from_client) {
  int to_client = server_connect(from_client);
  char buf[BUFFER_SIZE];
  
  while (1) {
    read(from_client, buf, sizeof(buf));
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
