#include "utils.h"

int has_edit(char *s);

int main(int argc, char **argv) {

  int server_socket;
  char buffer[BUFFER_SIZE];

  if (argc == 2)
  server_socket = client_setup( argv[1]);
  else
  server_socket = client_setup( TEST_IP );

  printf("\t\t---------------------------------------------------\n");
  printf("\tWelcome to Storytime! Please type 'help' to recieve a list of commands.\n");
  printf("\t\t---------------------------------------------------\n");

  int edit = 0;

  while (1) {
    if (edit) {
      //printf("enter addition: ");

      //write buffer to file
      char name[17] = "storyXXXXXX";
      int fd = mkstemp(name);
      printf("fd: %d\tname: %s\n", fd, name);
      write(fd, buffer, strlen(buffer));

      printf("buffer: %s\n", buffer);

      //use default text editor by getting environment variables
      //use nano for now
      //fork, exec, wait
      if (fork() == 0) {
        if (execlp("nano", "nano", name, NULL) == -1)
        printf("exec failed\n");
        exit(1);
      } else {
        int status;
        wait(&status);
        //do something w/ status?
      }

      //then take file and put that in buffer
      lseek(fd, 0, SEEK_SET);
      int len = read(fd, buffer, sizeof(buffer));
      buffer[len] = 0;
      printf("buffer: %s\n", buffer);
      close(fd);
      unlink(name);

    }
    else {
      printf("\n\tEnter command: \n\t\t");
      fgets(buffer, sizeof(buffer), stdin);
    }

    if (!edit){
      *strchr(buffer, '\n') = 0;
      if (strlen(buffer) == 0)
      strcpy(buffer, "temp");

      edit = has_edit(buffer);
    } else {
      if (strlen(buffer) == 0)
      strcpy(buffer, no_text);

      edit = 0;
    }

    write(server_socket, buffer, strlen(buffer));

    int len = read(server_socket, buffer, sizeof(buffer));
    //printf("len: %d\n", len);
    buffer[len] = 0;
    //printf("received: [%s]\n", buffer);
    printf("\n\t%s\n", buffer);

    //if story actually doesn't exist, don't go into edit mode
    if (strcmp(buffer, no_story) == 0)
      edit = 0;
    //if no text, don't have text
    else if (strcmp(buffer, no_text) == 0)
      *buffer = 0;
  }
}

int has_edit(char *s) {
  char copy[BUFFER_SIZE];
  strcpy(copy, s);

  char **args = parse_args(copy);
  int edit = 0;
  if (strcmp(args[0], "edit") == 0)
  edit = 1;
  free(args);
  return edit;
}
