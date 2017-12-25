#include "pipe_networking.h"

int main() {

  int to_server;
  int from_server;
  char buffer[BUFFER_SIZE];

  from_server = client_handshake( &to_server );

  while (1) {
    printf("enter data: ");
    if (!fgets(buffer, sizeof(buffer), stdin)) //if fgets only gets eof char
      *buffer = 0;
    if (strchr(buffer, '\n'))    //buffer might not have newline
      *strchr(buffer, '\n') = 0;
    write(to_server, buffer, sizeof(buffer));
    read(from_server, buffer, sizeof(buffer));
    printf("received: [%s]\n", buffer);
  }
}
