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
    // find "\n" position
    size_t pos = strcspn(command, "\n");
    command[pos] = '\0'; // replace it with terminal char

    printf("%s: command not found", command);
  }

  return 0;
}
