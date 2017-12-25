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

/*
  Input:

  help
  create <name of story>
  read <name of story>
  edit <name of story>
 */
void process(char * s) {
  char **args = parse_args(s);
  
  if (strcmp(*args, "help") == 0) {
    help(s);
  } else if (strcmp(*args, "create") == 0) {
    create();
  } else if (strcmp(*args, "read") == 0) {
    read_story();
  } else if (strcmp(*args, "edit") == 0) {
    edit();
  } else {
    strcpy(s, "Unknown command");
  }
}

//print a list of valid commands
void help(char *buf) {
  strcpy(buf, "Valid commands:\nhelp - get this list\ncreate [name of story]\nread [name of story]\nedit [name of story]");
}

//make the file with the given name, then let client edit it
void create() {

}

//read the file, write to client
void read_story() {

}

//read the file, prompt client for addition, then append to file
void edit() {

}

/*
  ex: takes "ls -a -l"
  returns array ["ls", "-a", "-l", NULL]

  assume no more than 19 args
*/
char **parse_args(char *line) {
  //pointer to "array"; free this later
  char **args = malloc(20 * sizeof(char *));

  int i = 0;
  while (line) {
    char *arg = strsep(&line, " ");
    //get rid of extra spaces
    if (strcmp(arg, "") == 0){
      continue;
    }
    args[i] = arg;
    i++;
  }
  args[i] = NULL;
  
  return args;
}
