#include "utils.h"
#include "server.h"

#include <signal.h>
#include <ctype.h>

static void sig_handler(int signo){
  if (signo == SIGINT){
    printf("SIGINT received\nRemoving all semaphores\n");
    remove_semaphores();
    exit(SIGINT);
  }
}

//Remove all semaphores
void remove_semaphores(){
  //go through all stories and get corresponding semaphore
  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      char *filename = dir->d_name;
      if (strcmp(filename, ".") == 0 ||
          strcmp(filename, "..") == 0)
        continue;

      //create key using filename
      int key = ftok(filename, 42);

      //Gets semaphores for deletion
      int semid = semget(key, 0, 0);
      if (semid != -1)
        semctl(semid, 0, IPC_RMID);
    }
    closedir(d);
  }

}


int main() {
  signal(SIGINT, sig_handler);
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

  while (1) {
    int len = read(client_socket, buffer, sizeof(buffer));
    if (len == 0)
      break;
    buffer[len] = 0;
    printf("[subserver %d] received: [%s]\n", getpid(), buffer);
    process(client_socket, buffer);
  }//end read loop
  close(client_socket);
  exit(0);
}

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
  } else if (strcmp(*args, "remove") == 0) {
    if (args[1])
      remove_story(client_socket, buf, args[1]);
    else
      write(client_socket, input_error, strlen(input_error));
  } else if (strcmp(*args, "list") == 0) {
    list(client_socket, buf);
  } else {
    char *error = "Unknown command";
    write(client_socket, error, strlen(error));
  }

  free(args);
}

//print a list of valid commands
void help(int client_socket, char *buf) {
  char *s = "Valid commands:\n\thelp - get this list\n\tcreate [name of story]\n\tread [name of story]\n\tedit [name of story]\n\tremove [name of story]\n\tlist - view available stories";
  write(client_socket, s, strlen(s));
}

//make the file with the given name, then let client edit it
void create(int client_socket, char *buf, char *filename) {
  if (!filename_handler(client_socket, filename))
    return;

  char f[BUFFER_SIZE]; //don't use filename (part of buf)
  strcpy(f, filename);

  int fd;
  fd = open(f, O_CREAT | O_EXCL , 0666);
  if(fd == -1)
    sprintf(buf, "File already created: %s", f);
  else
    sprintf(buf, "Created file: %s", f);
  write(client_socket, buf, strlen(buf));
  close(fd);
}

//read the file, write to client
void read_story(int client_socket, char *buf, char *filename) {
  if (!filename_handler(client_socket, filename))
    return;

  //check if story exists
  int fd = open(filename, O_EXCL | O_RDONLY);
  if (fd == -1) {
    char *s = "There is no such story.";
    write(client_socket, s, strlen(s));
    return;
  }

  int semid = semaphore_handler(client_socket, filename);
  if (semid == -1)
    return;

  int len = read(fd, buf, BUFFER_SIZE);
  //printf("len: %d\n", len);

  if (len == 0) {
    write(client_socket, no_text, strlen(no_text));
  } else
    write(client_socket, buf, len);
  close(fd);
}

//read the file, prompt client for addition, then append to file
void edit(int client_socket, char *buf, char *filename) {
  if (!filename_handler(client_socket, filename))
    return;

  char f[BUFFER_SIZE]; //don't use filename (part of buf)
  strcpy(f, filename);

  //check if story exists
  int fd = open(f, O_EXCL | O_WRONLY | O_APPEND, 0666);
  if (fd == -1) {
    write(client_socket, no_story, strlen(no_story));
    return;
  }
  close(fd);

  int semid = semaphore_handler(client_socket, filename);
  if (semid == -1)
    return;

  read_story(client_socket, buf, f);

  //down
  struct sembuf arg;
  arg.sem_num = 0;
  arg.sem_op = -1;
  arg.sem_flg = SEM_UNDO | IPC_NOWAIT;
  semop(semid, &arg, 1);

  fd = open(f, O_EXCL | O_WRONLY | O_TRUNC, 0666);
  int len = read(client_socket, buf, BUFFER_SIZE);
  buf[len] = 0;

  //if no input, don't write
  if (len != 0 && strcmp(buf, no_text) != 0)
    write(fd, buf, strlen(buf));
  close(fd);

  //up
  arg.sem_op = 1;
  semop(semid, &arg, 1);

  //read_story(client_socket, buf, f);
  char *message = "\tFile successfully edited.\n";
  write(client_socket, message, strlen(message));
}

//list the stories
void list(int client_socket, char *buf) {
  strcpy(buf, "Stories:\n");
  
  DIR *d;
  struct dirent *dir;
  d = opendir(".");

  if (d) {
    while ((dir = readdir(d)) != NULL) {
      char *name = dir->d_name;
      if (strcmp(name, ".") == 0 ||
          strcmp(name, "..") == 0)
        continue;
      strcat(buf, "\t");
      strcat(buf, dir->d_name);
      strcat(buf, "\n");
    }
    closedir(d);
  }

  if (strlen(buf) == 0)
    strcpy(buf, "There are no stories. :(");
  write(client_socket, buf, strlen(buf));
}

void remove_story(int client_socket, char *buf, char *filename) {
  if (!file_exists(client_socket, filename))
    return;

  if (!filename_handler(client_socket, filename))
    return;

  int semid = semaphore_handler(client_socket, filename);
  if (semid == -1)
    return;


  char *s;
  if (remove(filename) == 0){
    s = "Story successfully deleted.";
  }
  else{
    s = "Unable to delete story.";
  }
  write(client_socket, s, strlen(s));

  
  return;
}


//creates semaphore (if it doesn't exist) and checks the value
int semaphore_handler(int client_socket, char *filename) {
  //create key using filename
  int key = ftok(filename, 42);

  //create semaphore if not created
  int semid = semget(key, 1, 0664 | IPC_CREAT | IPC_EXCL);
  if (semid != -1) {
    union semun sem;
    sem.val = 1;
    semctl(semid, 0, SETVAL, sem);
  }
  //use actual semaphore
  semid = semget(key, 1, 0664);

  //check value of semaphore
  if (semctl(semid, 0, GETVAL) == 0) {
    char *s = "Someone is editing the story. Please wait.";
    write(client_socket, s, strlen(s));
    return -1;
  }

  return semid;
}

//check if the file doesn't change directory
int valid_file(char *s) {
  //check if contains slashes or starts with dot
  if (strchr(s, '/') == NULL &&
      *s != '.')
    return 1;
  return 0;
}

int filename_handler(int client_socket, char *filename) {
  if (!valid_file(filename)) {
    char *s = "Invalid file. Please don't change directories or use directories.";
    write(client_socket, s, strlen(s));
    return 0;
  }
  return 1;
}


int file_exists(int client_socket, char *filename) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1){
    char *s = "This file does not exist.";
    write(client_socket, s, strlen(s));
    return 0;
  }
  return 1;
}
