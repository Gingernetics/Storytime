#include "networking.h"

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

  while (1) {
    printf("enter data: ");
    fgets(buffer, sizeof(buffer), stdin);
    *strchr(buffer, '\n') = 0;
    if (strlen(buffer) == 0)
    strcpy(buffer, "temp");
    write(server_socket, buffer, strlen(buffer));
    int len = read(server_socket, buffer, sizeof(buffer));
    printf("len: %d\n", len);
    buffer[len] = 0;
    printf("received: [%s]\n", buffer);
  }
}
