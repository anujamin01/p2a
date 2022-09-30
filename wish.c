#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void read_command(char cmd[], char *args[])
{
  char *parsedCommands[100], *currCommand, *line;
  //char line[1024];
  size_t bufsize = 0;
  // read command from the line
  getline(&line,&bufsize,stdin);

  // break command into words
  int idx = 0;
  currCommand = strtok(line, " \n");
  while (currCommand != NULL){
    parsedCommands[idx] = strdup(currCommand);
    idx++;
    currCommand = strtok(NULL, " \n");
  }

  // put 1st command into cmd
  strcpy(cmd, parsedCommands[0]);

  // grab the arguments 
  for(int a = 1; a < idx; a++){
    args[a] = parsedCommands[a]; 
  }
  args[idx] = NULL; // terminate the args 
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