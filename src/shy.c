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

    // If we hit end of file, replace it with null char and return
    if (c == EOF || c== '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we exceed buffer size, reallocate
    if(position >= bufsize) {
      bufsize += SHY_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if(!buffer) {
        fprintf(stderr, "shy: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
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

