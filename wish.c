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

  // grab the paramaters as well
  for (int j = 0; j < i; j++)
  {
    par[j] = array[j];
  }
  par[i] = NULL; // null terminate parameters list
}

int main(int argc, char* argv[]){
    int batch_mode = 0; // default is interactive mode
    char* filename;
    FILE *fp;
    // TODO: get batch mode working 

    // check if we're in batch mode or not
    if (argc == 2){
      filename = argv[1];
      fp = fopen(filename, "r");
      if (!(fp)){
        printf("Error: unable to open file");
        exit(1);
      }
      batch_mode = 1;
    }

  char cmd[256], command[256], *parameters[256];

  while (1)
  { // repeats forever
    printf("wish> "); // display prompt
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