#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);

  printf("$ ");

  char command[1024];
  if (fgets(command, sizeof(command), stdin))
  {
    // remove the new line character from input
    command[strlen(command) - 1] = '\0';

    printf("%s: command not found", command);
  }

  return 0;
}
