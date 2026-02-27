#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  char *DIR = getenv("PATH");

  // REPL implementation
  while (1)
  {
    // Flush after every printf
    setbuf(stdout, NULL);

    // print prefix
    printf("$ ");

    char input[1024];
    if (fgets(input, sizeof(input), stdin))
    {
      // remove the new line character from input
      input[strlen(input) - 1] = '\0';

      char *command = input;
      char *arg = "";

      // find the first space position
      char *space_pos = strchr(input, ' ');
      if (space_pos != NULL)
      {
        space_pos[0] = '\0'; // replace first space with null terminator
        arg = space_pos + 1;
      }

      if (strcmp(command, "exit") == 0)
      {
        break;
      }
      else if (strcmp(command, "echo") == 0)
      {
        // printf the output without the "echo " part
        printf("%s\n", arg);
      }
      else if (strcmp(command, "type") == 0)
      {
        if (strcmp(arg, "exit") == 0 || strcmp(arg, "echo") == 0 || strcmp(arg, "type") == 0)
        {
          printf("%s is a shell builtin\n", arg);
        }
        else
        {
          if (DIR != NULL)
          {
            char path[4096];
            strncpy(path, DIR, sizeof(path) - 1);

            char *dir = strtok(path, ":");
            int found = 0;

            while (dir != NULL)
            {
              char full_path[1024];
              // format the `full_path` into "dir/arg"
              snprintf(full_path, sizeof(full_path), "%s/%s", dir, arg);

              if (access(full_path, X_OK) == 0)
              {
                printf("%s is %s\n", arg, full_path);
                found = 1;
                break;
              }
              dir = strtok(NULL, ":");
            }
            if (!found)
            {
              printf("%s: not found\n", arg);
            }
          }
        }
      }
      else
      {
        printf("%s: command not found\n", command);
      }
    }
  }

  return 0;
}
