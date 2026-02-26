#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

  // REPL implementation
  while (1)
  {
    // Flush after every printf
    setbuf(stdout, NULL);

    // print prefix
    printf("$ ");

    char command[1024];
    if (fgets(command, sizeof(command), stdin))
    {
      // remove the new line character from input
      command[strlen(command) - 1] = '\0';

      // check exit command
      if (strcmp(command, "exit") == 0)
      {
        break;
      }
      else if (strncmp(command, "echo ", 5) == 0)
      {
        // printf the output without the "echo " part
        printf("%s\n", command + 5);
      }
      else
      {
        printf("%s: command not found\n", command);
      }
    }
  }

  return 0;
}
