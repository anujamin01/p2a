#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void read_command(char cmd[], char *par[])
{
  char line[1024];
  int count = 0, i = 0, j = 0;
  char *array[100], *pch;

  // read one line of the command

  for (;;)
  {
    // getline 
    int c = fgetc(stdin);
    line[count++] = (char)c;
    if (c == '\n')
      break; // reached end of the line
  }

  /*
  int c = fgetc(stdin);
  line[count++] = char(c)
  while (c != '\n'){
    c = fgetc(stdin);
    line[count++] = char(c);
  }
  */
  if (count == 1)
    return; // read nothing
  pch = strtok(line, " \n");

  // parse the line into words
  while (pch)
  {
    array[i++] = strdup(pch);
    pch = strtok(NULL, " \n");
  }

  // 1st word is command
  strcpy(cmd, array[0]);
  /*
  if (array[1] && array[1][0] == '-')
  {
    strcpy(cmd, array[])
  }
  */
  // grab the paramaters as well
  for (int j = 0; j < i; j++)
  {
    par[j] = array[j];
  }
  par[i] = NULL; // null terminate parameters list
}

void type_prompt()
{
  static int first_time = 1;
  if (first_time)
  {
    // clear screen for the 1st time
    const char *CLEAR_SCREEN_ANSI = " \e[1;1H\e[2J";
    write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
    first_time = 0;
  }
  printf("wish> "); // display prompt
}

int main()
{

  char cmd[256], command[256], *parameters[256];
  char *envp[] = {(char *)"PATH=/bin", 0}; // environment variable

  while (1)
  { // repeats forever
    type_prompt();
    read_command(command, parameters); // read the input from the terminal
    int retval = fork();
    if (retval == 0)
    {
      if (strcmp(command,"exit") == 0){
        exit(0);
      } 
      // TODO: get cd and path working
      strcpy(cmd, "/bin/");
      strcat(cmd, command);
      execv(cmd,parameters);
      //execve(cmd, parameters, envp);
      // exec doesn't return if succeeds
      printf("ERROR: Could not execute %s\n", cmd);
      exit(1);
    }
    else
    {
      // parent process; wait for child to finish
      int pid = retval;
      wait(NULL);
    }

    // exit the shell
    if (strcmp(command, "exit") == 0)
    {
      break;
    }
  }
  return 0;
}