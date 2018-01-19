void process(int client_socket, char * buf);
void subserver(int client_socket);

void help(int client_socket, char *buf);
void create(int client_socket, char *buf, char *filename);
void read_story(int client_socket, char *buf, char *filename);
void edit(int client_socket, char *buf, char *filename);

char ** parse_args(char *line);
