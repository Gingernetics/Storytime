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
      printf("enter addition: ");
    }
    else {
      printf("enter data: ");
    }
    fgets(buffer, sizeof(buffer), stdin);
    *strchr(buffer, '\n') = 0;
    if (strlen(buffer) == 0)
    strcpy(buffer, "temp");

    if (!edit){
      edit = has_edit(buffer);
    } else {
      edit = 0;
    }

    write(server_socket, buffer, strlen(buffer));

    int len = read(server_socket, buffer, sizeof(buffer));
    printf("len: %d\n", len);
    buffer[len] = 0;
    printf("received: [%s]\n", buffer);
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
