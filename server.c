#include "networking.h"
#include "server.h"

#include <signal.h>
#include <ctype.h>

int main() {
  chdir("stories");
  
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
  char *input_error = "Please input a story name";
  
  if (strcmp(*args, "help") == 0) {
    help(client_socket, buf);
  } else if (strcmp(*args, "create") == 0) {
    if (args[1])
      create(client_socket, buf, args[1]);
    else
      write(client_socket, input_error, strlen(input_error));
  } else if (strcmp(*args, "read") == 0) {
    if (args[1])
      read_story(client_socket, buf, args[1]);
    else
      write(client_socket, input_error, strlen(input_error));
  } else if (strcmp(*args, "edit") == 0) {
    if (args[1])
      edit(client_socket, buf, args[1]);
    else
      write(client_socket, input_error, strlen(input_error));
  } else {
    write(client_socket, "Unknown command", BUFFER_SIZE);
  }

  free(args);
}

//print a list of valid commands
void help(int client_socket, char *buf) {
  char *s = "Valid commands:\n\thelp - get this list\n\tcreate [name of story]\n\tread [name of story]\n\tedit [name of story]";
  write(client_socket, s, strlen(s));
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
  int len = read(fd, buf, BUFFER_SIZE);
  printf("len: %d\n", len);
  if (len == 0) {
    char *s = "<There is no text>";
    write(client_socket, s, strlen(s));
  } else 
    write(client_socket, buf, len);
  close(fd);
}

//read the file, prompt client for addition, then append to file
void edit(int client_socket, char *buf, char *filename) {
  read_story(client_socket, buf, filename);
  //int fd = open(filename, O_EXCL | , 0666
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
