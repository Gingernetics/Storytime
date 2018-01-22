void process(int client_socket, char * buf);
void subserver(int client_socket);

void help(int client_socket, char *buf);
void create(int client_socket, char *buf, char *filename);
void read_story(int client_socket, char *buf, char *filename);
void edit(int client_socket, char *buf, char *filename);

char ** parse_args(char *line);

union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
  (Linux-specific) */
};
