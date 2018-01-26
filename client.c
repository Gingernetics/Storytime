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
  int first_prompt = 1;

  while (1) {
    if (edit) {
      //printf("enter addition: ");

      //write buffer to file
      char name[17] = "storyXXXXXX";
      int fd = mkstemp(name);
      //printf("fd: %d\tname: %s\n", fd, name);
      write(fd, buffer, strlen(buffer));
      
      //printf("buffer: %s\n", buffer);
      close(fd);
      
      //use default text editor by getting environment variables
      //use nano if not found
      char *editor = getenv("EDITOR");
      if (!editor) {
        editor = "nano";
        printf("default editor not found\n");
      }
      printf("editor: %s\n", editor);
      
      //fork, exec, wait
      if (fork() == 0) {
        if (execlp(editor, editor, name, NULL) == -1)
          printf("exec failed. Set a valid text editor?\n");
        exit(1);
      } else {
        int status;
        wait(&status);
        //do something w/ status?
      }

      //then take file and put that in buffer
      fd = open(name, O_RDONLY);
      if (fd == -1) {
        printf("Error opening file again\n");
      } else {
        int len = read(fd, buffer, sizeof(buffer));
        buffer[len] = 0;
        //printf("buffer: %s\n", buffer);
        close(fd);
        remove(name);
      }
      
      if (strlen(buffer) == 0)
        strcpy(buffer, no_text);

      edit = 0;
    }
    else {
      if (first_prompt){
        first_prompt--;
      }
      else{
        printf("\n\t\t--^--^--\n");
      }
      printf("\nEnter command: \n\t");
      fgets(buffer, sizeof(buffer), stdin);

      *strchr(buffer, '\n') = 0;
      if (strlen(buffer) == 0)
        strcpy(buffer, "temp");

      edit = has_edit(buffer);
    }

    write(server_socket, buffer, strlen(buffer));

    int len = read(server_socket, buffer, sizeof(buffer));
    //printf("len: %d\n", len);
    buffer[len] = 0;
    //printf("received: [%s]\n", buffer);
    printf("\n%s\n", buffer);

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
