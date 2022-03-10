#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 Declarations of builtins
*/

int shy_cd(char **args);
int shy_help(char **args);
int shy_exit(char **args);

/*
 Builtin List
*/

char *builtin_str[] = {
  "cd",
  "help",
  "exit",
};

int (*builtin_func[]) (char **) = {
  &shy_cd,
  &shy_help,
  &shy_exit
};

int shy_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
 * Builtin Implementations
*/

int shy_cd(char **args)
{
  if(args[1] == NULL) {
    fprintf(stderr, "shy: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("shy");
    }
  }
  return 1;
}

int shy_help(char **args)
{
  int i;
  printf("Shylex's Shyll\n");
  printf("Type program names and arguments, then hit enter.\n");
  printf("The following are built into Shyll:\n");
  for(i=0; i < shy_num_builtins(); i++) {
    printf(" %s\n", builtin_str[i]);
  }

  printf("Use the man command for info for other installed programs.\n");
  return 1;
}

int shy_exit(char **args)
{
  return 0;
}


int shy_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    //Child Process
    if (execvp(args[0], args) == -1) {
      perror("shy");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // error forking
    perror("shy");
  } else {
    // Parent Process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int shy_execute(char **args)
{
  int i;

  if(args[0] == NULL) {
    // No Command Entered
    return 1;
  }

  for (i = 0; i < shy_num_builtins(); i++) {
    if(strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }
  
  return shy_launch(args);
}

#define SHY_RL_BUFSIZE 1024
char *shy_read_line(void)
{
  int bufsize = SHY_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if(!buffer) {
    fprintf(stderr, "shy: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while(1) {
    // Read a character
    c = getchar();

    if(c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // Reallocate if exceeding buffer
    if (position >= bufsize) {
      bufsize += SHY_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "shy: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define SHY_TOK_BUFSIZE 64
#define SHY_TOK_DELIM " \t\r\n\a"
char **shy_split_line(char *line)
{
  int bufsize = SHY_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "shy: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, SHY_TOK_DELIM);
  while(token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += SHY_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "shy: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token=strtok(NULL, SHY_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

void shy_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = shy_read_line();
    args = shy_split_line(line);
    status = shy_execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv)
{
  // Load Configs
  //
  // Run Command Loop
  shy_loop();

  // Shutdown + Cleanup

  return EXIT_SUCCESS;
}

