#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PATH_LEN 4096
#define MAX_ARGS_CNT 64 // the command can holds up to 64 arguments

// Find executable in PATH, returns 1 if found and store path in result
int find_in_path(const char *command, char *result, size_t result_size)
{
  char *path_env = getenv("PATH");
  if (path_env == NULL)
    return 0;

  char path[MAX_PATH_LEN];
  strncpy(path, path_env, sizeof(path - 1));

  char *dir = strtok(path, ":"); // separate `path` into token by ":"
  while (dir != NULL)
  {
    snprintf(result, result_size, "%s/%s", dir, command);

    if (access(result, X_OK) == 0)
      return 1;

    // move to the next token
    dir = strtok(NULL, ":");
  }

  return 0;
}

// Check if command is a builtin
int is_builtin(const char *command)
{
  return strcmp(command, "exit") == 0 ||
         strcmp(command, "echo") == 0 ||
         strcmp(command, "type") == 0;
}

// Handle 'echo' command
void cmd_echo(char *args[], int arg_count)
{
  for (int i = 1; i < arg_count; i++)
  {
    if (i > 1)
      printf(" ");

    printf("%s", args[i]);
  }

  printf("\n");
}

void cmd_type(const char *arg)
{
  if (is_builtin(arg))
  {
    printf("%s is a shell builtin\n", arg);
  }
  else
  {
    char full_path[1024];
    if (find_in_path(arg, full_path, sizeof(full_path)))
      printf("%s is %s\n", arg, full_path);
    else
      printf("%s: not found\n", arg);
  }
}

void execute_command(char **args)
{
  char *command = args[0]; // get the command name

  char full_path[1024];
  if (!find_in_path(command, full_path, sizeof(full_path)))
  {
    printf("%s: command not found\n", command);
    return;
  }

  pid_t pid = fork();
  if (pid == 0)
  {
    // the execv get the path and args with the first arg is file name
    execv(full_path, args);
    perror("execv failed");
    exit(1);
  }
  else if (pid > 0)
    wait(NULL); // Wait for child to finish
  else
    perror("fork failed");
}

// Parse input into args array, returns argument count
int parse_input(char *input, char *args[])
{
  int cnt = 0;
  char *token = strtok(input, " ");

  while (token != NULL && cnt < MAX_ARGS_CNT - 1)
  {
    args[cnt++] = token;
    strtok(NULL, " ");
  }

  args[cnt] = NULL; // NULL-terminate the array
}

int main(int argc, char *argv[])
{
  // Flush after every printf
  setbuf(stdout, NULL);

  // REPL implementation
  while (1)
  {
    // print prefix
    printf("$ ");

    char input[1024];

    if (!fgets(input, sizeof(input), stdin))
      break;

    // remove the new line character from input
    input[strlen(input) - 1] = '\0';

    // Parse input into arguments
    char *args[64]; // max 64 arguments
    int arg_count = parse_input(input, args);

    if (arg_count == 0) // empty input
      continue;

    char *command = args[0];
    char *arg = (arg_count > 1) ? args[1] : "";

    if (strcmp(command, "exit") == 0)
      break;
    else if (strcmp(command, "echo") == 0)
      cmd_echo(args, arg_count);
    else if (strcmp(command, "type") == 0)
      cmd_type(arg);
    else
      execute_command(args);
  }

  return 0;
}
