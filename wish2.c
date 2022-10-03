
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

// global path variable
char* path[100];

// TODO: get bash_redirection working
int bash_redirection(char* line, char* redir){
  char* args[100];
  char* destination;
  char* currCommand;
  // int index = (int)(redir - line);
  // printf("%d\n", index);
  // printf("%s\n", (line + index));
  // parse the line to get the command and arguments
  int idx = 0;
  currCommand = strtok(line, " \t\r\a\n");
  while (currCommand != NULL)
  {
    printf("currLine: %s\n", currCommand);
    args[idx] = strdup(currCommand);
    //printf("%s\n",args[idx]);
    idx++;
    currCommand = strtok(NULL, " \t\r\a\n");
  }
  // move past the '>' char
  currCommand = strtok(NULL, " \t\r\a\n");
  // grab the destination 
  if (currCommand == NULL){
    printf("Missing destination file\n");
    return -1;
  } else{
    currCommand = strtok(NULL, " \t\r\a\n");
    strcpy(destination, currCommand);
  }
  printf("Destination: %s\n", destination);
  return 1;
}

int read_command(char *args[], FILE *fp)
{
  char *line = malloc(100 * sizeof(char));
  char *currCommand; // *args[256];
  size_t buf_size;
  int read_args;
  char *redirect_char = NULL;
  // grab line
  //fflush(stdin);
  read_args = getline(&line, &buf_size, fp);
  //printf("%s",line);
  // check if there is nothing to read
  if (read_args == -1)
  {
    printf("Nothing to read");
    return -1;
  }
  int line_length = strlen(line);
  //printf("%d\n",line_length);
  // empty line
  if (line_length == 0 || strcmp(line, "") == 0 || strcmp(line, "\n") == 0 || strcmp(line,"\0") == 0)
  {
    return 1;
  }

  // remove the \n last character
  if ('\n' == line[line_length - 1])
  {
    line[line_length - 1] = '\0';
  }
  // reached end of batch file
  if (line[0] == EOF)
  {
    return -1;
  }

  // check if redirection exists within the string
  redirect_char = strchr(line,'>');
  if(redirect_char != NULL){
    // further process redirection and execute the command with
    bash_redirection(line, redirect_char);
    return 1; // some error occured
  }

  // parse the line and separate the inputs
  int idx = 0;
  currCommand = strtok(line, " \t\r\a\n");
  while (currCommand != NULL)
  {
    args[idx] = strdup(currCommand);
    //printf("%s\n",args[idx]);
    idx++;
    currCommand = strtok(NULL, " \t\r\a\n");
  }
  /**
  if(currCommand == NULL){
    printf("End of command\n");
  }
  */

  args[idx] = NULL; // terminate the args
  // handle commands being ran
  // exit command
  if (strcmp(args[0], "exit") == 0)
  {
    if (args[1] != NULL){
      printf("Error: exit has invalid number of arguments\n");
      return -1;      
    }
    exit(0);
  }
  else if (strcmp(args[0], "cd") == 0)
  {
    if (idx != 2)
    { // if we have too many or too little arguments throw error
      printf("Error: cd has invalid number of arguments\n");
      return -1;
    }
    else
    {
      int ret = chdir(args[1]);
      if (ret == 0)
      { // if we were able to change directories return success
        return 1;
      }
      else
      {
        printf("Error: unable to change directories");
        return -1;
      }
    }
  }
  else if (strcmp(args[0], "path") == 0)
  {
    // update the global path variable
    int i = 0;
    for (;;)
    {
      path[i] == args[i + 1];
      i++;
      if (args[i + 1] == NULL)
      {
        break;
      }
    }
    return 1;
  }
  return 0;
}

int bin_cmd_helper(char* args[]){
  //char *parsedParams[100];
  char* binPath;
  // cannot execute command ie no arguments or path specified
  if (path[0] == NULL || args == NULL || args[0] == NULL){
    return -1;
  }

  // iterate through all paths and try to execute a bin command
  int x = 0;
  while (path[x] != NULL){
    // execute the call
    int pid = fork();
    //printf("pid -->%d\n",pid);
    //printf("what is happening");
    if (pid == 0){
        char* cmd = args[0];
        // try executing the bin command
        //printf("%s\n",cmd);
        //strcat(binPath, cmd);
        //printf("%s\n",binPath);
        printf("I'm the child");

        execv("/bin/ls",args);

        // error has occured with the bin path 
        printf("ERROR: Could not execute %s\n", cmd);
        //exit(1);
    } else{
      int status;
      waitpid(pid,&status,0);
    }
    x++;
  }
  return 0;
}


int main(int argc, char *argv[])
{
  path[0] = "/bin";
  int batch_mode = 0; // default is interactive mode
  char *filename;
  FILE *fp;
  char** arguments;
  // TODO: get batch mode working

  // check if we're in batch mode or not
  if (argc == 2)
  {
    filename = argv[1];
    fp = fopen(filename, "r");
    if (!(fp))
    {
      printf("Error: unable to open file");
      exit(1);
    }
    batch_mode = 1;
  }

  while (1)
  {

    if (batch_mode == 1)
    {
      // TODO: get batch mode working

      // read each command line by line
      for(;;){
        arguments = malloc(sizeof(char) * 100);
        int process_command = read_command(arguments, fp);
        if (process_command == -1)
        {
          // some error has occured when we processed the command 
          break;
        }        
        else if (process_command == 1)
        { // if we called in a built in command then continue the loop (exit, path,  etc.)
          continue;
        } else { // need to execute a bin executable such as ls
          // printf("reached else case\n");
          int run_bin_cmd = bin_cmd_helper(arguments);
          //printf("%d\n", run_bin_cmd);
          if (run_bin_cmd == -1){ // some error occured so go back to top of loop 
            continue;
          }
        }
        free(arguments);
      }
      // break; ???
    }
    else
    {  // not in batchmode
      printf("wish> "); // display prompt
      //fflush(stdout);
      //char *arguments[256];
      arguments = malloc(sizeof(char) * 100);
      //for(int i = 0; i < 100; i++){
      //  arguments[i] = malloc(sizeof(char) * 256);
      //}
      int process_command = read_command(arguments, stdin);
      //fflush(stdin);
      //printf("%s\n",arguments[0]);
      if (process_command == -1)
      {
        // some error has occured when we processed the command 
        break;
      }
      else if (process_command == 1)
      { // if we called in a built in command then continue the loop (exit, path,  etc.)
        continue;
      } else { // need to execute a bin executable such as ls
        printf("reached else case\n");
        int run_bin_cmd = bin_cmd_helper(arguments);
        //printf("%d\n", run_bin_cmd);
        if (run_bin_cmd == -1){ // some error occured so go back to top of loop 
          continue;
        }
      }
      //for(int i = 0; i < 100; i++){
      //  free(arguments[i]);
      //}
      free(arguments);
    }
  }
  return 0;
}