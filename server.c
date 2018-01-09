#include "pipe_networking.h"
#include "server.h"

#include <signal.h>
#include <ctype.h>

int main() {
  int listen_socket;
  int f;
  listen_socket = server_setup();

  while (1) {

    int client_socket = server_connect(listen_socket);
    f = fork();
    if (f == 0)
      subserver(client_socket);
    else
      close(client_socket);
  }
}

void subserver(int from_client) {
  char buffer[BUFFER_SIZE];

  while (read(client_socket, buffer, sizeof(buffer))) {

    printf("[subserver %d] received: [%s]\n", getpid(), buffer);
    process(buffer);
    write(client_socket, buffer, sizeof(buffer));
  }//end read loop
  close(client_socket);
  exit(0);
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
