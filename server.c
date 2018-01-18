#include "networking.h"
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

void subserver(int client_socket) {
  char buffer[BUFFER_SIZE];

  while (read(client_socket, buffer, sizeof(buffer))) {

    printf("[subserver %d] received: [%s]\n", getpid(), buffer);
    process(client_socket, buffer);
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
void process(int client_socket, char * buf) {
  char **args = parse_args(buf);
  
  if (strcmp(*args, "help") == 0) {
    help(client_socket, buf);
  } else if (strcmp(*args, "create") == 0) {
    create(client_socket, buf, args[1]);
  } else if (strcmp(*args, "read") == 0) {
    read_story(client_socket, buf, args[1]);
  } else if (strcmp(*args, "edit") == 0) {
    edit();
  } else {
    write(client_socket, "Unknown command", BUFFER_SIZE);
  }

  free(args);
}

//print a list of valid commands
void help(int client_socket, char *buf) {
  write(client_socket, "Valid commands:\n\thelp - get this list\n\tcreate [name of story]\n\tread [name of story]\n\tedit [name of story]", BUFFER_SIZE);
}

//make the file with the given name, then let client edit it
void create(int client_socket, char *buf, char *filename) {
  char f[BUFFER_SIZE]; //don't use filename (part of buf)
  strcpy(f, filename);
  sprintf(buf, "created file: %s", f);
  write(client_socket, buf, BUFFER_SIZE);
  int fd;
  fd = open(f, O_CREAT | O_EXCL , 0666);
  close(fd);
}

//read the file, write to client
void read_story(int client_socket, char *buf, char *filename) {
  int fd = open(filename, O_EXCL | O_RDONLY);
  read(fd, buf, BUFFER_SIZE);
  write(client_socket, buf, BUFFER_SIZE);
  close(fd);
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
