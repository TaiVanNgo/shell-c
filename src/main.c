#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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

      // Parse input into arguments
      char *args[64]; // max 64 arguments
      int arg_count = 0;

      char *token = strtok(input, " ");
      while (token != NULL && arg_count < 63)
      {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
      }
      args[arg_count] = NULL; // NULL-terminate the array

      if (arg_count == 0)
        continue; // empty input

      char *command = args[0];
      char *arg = (arg_count > 1) ? args[1] : "";

      if (strcmp(command, "exit") == 0)
      {
        break;
      }
      else if (strcmp(command, "echo") == 0)
      {
        // printf the output without the "echo " part
        for (int i = 1; i < arg_count; i++)
        {
          if (i > 1)
            printf(" ");
          printf("%s", args[i]);
        }
        printf("\n");
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
        char path[4096];
        strncpy(path, DIR, sizeof(path) - 1);

        char *dir = strtok(path, ":");
        int found = 0;

        while (dir != NULL)
        {
          char full_path[1024];
          // format the `full_path` into "dir/arg"
          snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);

          if (access(full_path, X_OK) == 0)
          {
            pid_t pid = fork();
            if (pid == 0)
            {
              // Child process - use the full_path we found
              execv(full_path, args);
              // If execv fails
              perror("execv failed");
              exit(1);
            }
            else if (pid > 0)
            {
              // Parent process - wait for child
              wait(NULL);
              found = 1;
              break; // Don't continue searching once executed
            }
            else
            {
              perror("fork failed");
            }
          }
          dir = strtok(NULL, ":");
        }
        if (!found)
        {
          printf("%s: command not found\n", command);
        }
      }
    }
  }

  return 0;
}
