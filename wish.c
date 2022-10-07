#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

// global path variable
char *path[100];
//int bin_cmd_helper(char**);
void handle_error(){
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message)); 
}

int run_helper(char *cmd, char *args[])
{
  //for(int i = 0; i < 2; i++){
  //  printf("%s\n", path[i]);
  //}
  //printf("Entering run helper\n");
  char binPath[100];
  int x = 0;
  int exitStatus = 1;
  if (strcmp(args[0], "cd") == 0)
  {
    int idx = 0;
    while (args[idx] != NULL){
      idx++;
    }
    if (idx != 2)
    { // if we have too many or too little arguments throw error
      handle_error();
      //printf("Error: cd has invalid number of arguments\n");
      return -1;
    } else
    {
      int ret = chdir(args[1]);
      if (ret == 0)
      { // if we were able to change directories return success
        return 1;
      }
      else
      {
        handle_error();
        //printf("Error: unable to change directories");
        return -1;
      }
    }
  }
  //printf("%s\n",args[1]);
  while (path[x] != NULL)
  {
    //binPath = "";
    //printf("%s\n", path[x]);
    strcpy(binPath, path[x]);
    strcat(binPath, "/");
    strcat(binPath, cmd);
    //printf("Binpath: %s\n", binPath);
    //printf("Attempting to access: %s\n", binPath);


    if (access(binPath,X_OK) != 0){
      //printf("Couldn't access: %s\n", binPath);
      x++;
      continue;
    } 
    //printf("Empty bin: %s\n",path[x]);
    // execute the call
    int pid = fork();
    // printf("pid -->%d\n",pid);
    //printf("what is happening\n");
    if (pid == 0)
    {
      //printf("%d\n",pid);
      //printf("About to execute\n");
      //printf("binPath: %s\n", binPath);
      execv(binPath, args);
      //printf("Didn't execute");
      //error has occured with the bin path
      handle_error();
      //printf("ERROR: Could not execute %s\n", cmd);
      //exit(1);
    }
    else
    {
      int status;
      waitpid(pid, &status, 0);
      exitStatus = WEXITSTATUS(status);
      //printf("Exit Status: %d\n", exitStatus);
    }
    x++;
    //printf("%d\n",x);
  }
  return exitStatus;
}
int if_cmd_helper(char *args[])
{
  char tk1[100], tk2[100], comparator[100], ret[100];
  char *args1[100];
  char *args2[100];
  int equals = 0;
  /*
  if some_command == 0 then some_other_command fi
  if token ==/!= constant then token fi
  0    1     2     3       4     5   6
  */

  // grab length of command
  int size = 0;
  while (args[size] != NULL)
  {
    //printf("args in if: %s\n", args[size]);
    size++;
  }

  int hasIf = -1;
  int hasFi = -1;
  int hasThen = -1;
  int hasComparator = -1;

  int idx = 0;
  // grab indices of essential arguments of the hasIf
  while (args[idx] != NULL)
  {
    if (strcmp(args[idx], "if") == 0 && hasIf == -1)
    {
      hasIf = idx;
    }
    if (strcmp(args[idx], "fi") == 0 && hasFi == -1)
    {
      hasFi = idx;
    }
    if (strcmp(args[idx], "then") == 0 && hasThen == -1)
    {
      hasThen = idx;
    }
    if ((strcmp(args[idx], "==") == 0 || strcmp(args[idx], "!=") == 0) && hasThen == -1)
    {
      hasComparator = idx;
    }
    idx++;
  }
  if (hasIf == -1 || hasFi == -1 || hasThen == -1 || hasComparator == -1)
  {
    handle_error();
    //printf("Error, invalid args");
    return -1;
  }

  // check size of command should always be at least 7
  if (size < 7)
  {
    handle_error();
    //printf("Error, invalid # args");
    return -1;
  }

  //printf("%s\n", args[0]);
  if(strcmp(args[0],"if") != 0){
    handle_error();
    return -1;
  }
  // start and end at wrong indices
  if (hasIf != 0 || hasFi != size-1){
    handle_error();
    return -1;
  }
  // put the args for 1st token in array
  int i = hasIf + 1;
  while(i< hasComparator){
    args1[i-hasIf-1] = args[i];
    i++;
  }
  args1[i-hasIf-1] = NULL;

  /*
  for (int i = hasIf + 1; i < hasComparator; i++)
  {
    //printf("%s\n", args[i]);
    args1[i-hasIf-1] = args[i];
    //printf("Printing LS: %s\n", args1[i]);
  }
  */
  //printf("%d\n", hasThen);
  //printf("%d\n", hasFi);
  // put the args for 2nd token in array
  
  i = hasThen + 1;
  while (i < hasFi){
    args2[i-hasThen-1] = args[i];
    i++;
  }
  args2[i-hasThen-1] = NULL;


  // put args for 2nd token in array
  // parse the command
  //printf("TK1: %s\n", *(args1+1));
  strcpy(tk1, args1[0]);
  //printf("TK1: %s\n", tk1);
  //printf("TK2: %s\n", args2[0]);
  strcpy(tk2, args2[0]);
  //printf("TK2: %s\n", tk2);
  strcpy(comparator, args[hasComparator]);
  //printf("Compare: %s\n", comparator);
  if (strcmp(comparator, "==") == 0)
  {
    //printf("%d\n",strcmp(comparator, "=="));
    equals = 1;
  }
  strcpy(ret, args[hasComparator + 1]);
  //printf("%s\n",ret);
  // run the first command
  int ret1 = run_helper(tk1,args1);
  //printf("%d\n",ret1);
  if (equals == 1)
  {
    if (ret1 == atoi(ret))
    {
      // run second command
      //printf("running 2nd command");
      run_helper(tk2, args2);
    }
  }
  else
  {
    if (ret1 != atoi(ret))
    {
      // run second command
      run_helper(tk2, args2);
    }
  }

  return 1;
}

int bash_redirection(char *line, char *redir)
{
  char *raw_args[100];
  char *args[100];
  char *destination;
  char *currCommand;
  char *raw_command;
  char *next_destination;
  char binPath[100];
  FILE *fp;
  raw_command = strtok(line, ">");
  int idx = 0;
  while (raw_command != NULL)
  {
    raw_args[idx] = raw_command;
    raw_command = strtok(NULL, ">");
    idx++;
  }
  raw_args[idx] = NULL; // terminate the raw_arguments

  if (idx != 2)
  {
    handle_error();
    //printf("Error redirection has invalid number of arguments\n");
    return -1;
  }
  idx = 0;
  currCommand = raw_args[0];
  // put command arguments into the arg
  raw_command = strtok(currCommand, " \t\r\a\n");
  while (raw_command != NULL)
  {
    args[idx] = raw_command;
    idx++;
    raw_command = strtok(NULL, " \t\r\a\n");
  }
  args[idx] = NULL;

  destination = raw_args[1];
  destination = strtok(destination, " \t\r\a\n");
  next_destination = strtok(NULL, " \t\r\a\n");
  if (next_destination != NULL){
    handle_error();
    return -1;
  }
  // printf("%s\n", destination);
  // iterate through all paths and try to execute a bin command
  // iterate through all paths and try to execute a bin command
  int x = 0;
  while (path[x] != NULL)
  {
    //printf("Empty path: %s\n",path[x]);
    // execute the call
    int pid = fork();
    // printf("pid -->%d\n",pid);
    // printf("what is happening");
    if (pid == 0)
    {
      char *cmd = args[0];
      strcpy(binPath, path[x]);
      strcat(binPath, "/");
      // printf("%s\n", binPath);
      strcat(binPath, args[0]);
      // printf("%s\n", binPath);
      fp = fopen(destination, "w");
      int oldfd = fileno(fp);
      dup2(oldfd, STDOUT_FILENO);
      execv(binPath, args);
      // error has occured with the bin path
      handle_error();
      //printf("ERROR: Could not execute %s\n", cmd);
      // exit(1);
    }
    else
    {
      int status;
      waitpid(pid, &status, 0);
    }
    x++;
  }
  return 1;
}

int read_command(char *args[], FILE *fp)
{
  char *line = malloc(100 * sizeof(char));
  char *currCommand; // *args[256];
  size_t buf_size;
  int read_args;
  char *redirect_char = NULL;
  char *binPth;
  // grab line
  // fflush(stdin);
  read_args = getline(&line, &buf_size, fp);
  //printf("%s\n", line);
  //  check if there is nothing to read
  //printf("%d\n", read_args);
  if (read_args == -1)
  {
    //printf("%s\n", line);
    //handle_error();
    //exit(1);
    //printf("Nothing to read");
    return -1;
  }
  int line_length = strlen(line);
  // printf("%d\n",line_length);
  //  empty line
  if (line_length == 0 || strcmp(line, "") == 0 || strcmp(line, "\n") == 0) // || strcmp(line, "\0") == 0)
  {
  //   //printf("Error\n");
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
    //printf("EOF");
    //exit(1);
    return -1;
  }

  // check if redirection exists within the string
  redirect_char = strchr(line, '>');
  if (redirect_char != NULL)
  {
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
    //printf("%s\n", currCommand);
    //printf("Printing the args: %s\n",args[idx]);
    idx++;
    currCommand = strtok(NULL, " \t\r\a\n");
  }

  args[idx] = NULL; // terminate the args
  // handle commands being ran
  // exit command
  // printf("%d\n",strcmp(args[0], "if"));
  //printf("%s\n",args[0]);
  //printf("\n");

  if (idx == 0) {
    return 1;
  }

  if (strcmp(args[0], "exit") == 0)
  {
    if (args[1] != NULL)
    {
      handle_error();
      //printf("Error: exit has invalid number of arguments\n");
      return -1;
    }
    exit(0);
  }
  else if (strcmp(args[0], "cd") == 0)
  {
    if (idx != 2)
    { // if we have too many or too little arguments throw error
      handle_error();
      //printf("Error: cd has invalid number of arguments\n");
      return -1;
    } else
    {
      int ret = chdir(args[1]);
      if (ret == 0)
      { // if we were able to change directories return success
        return 1;
      }
      else
      {
        handle_error();
        //printf("Error: unable to change directories");
        return -1;
      }
    }
  } else if (strcmp(args[0], "if") == 0)
  {
    //printf("Reached here\n");
    return if_cmd_helper(args);
  } else if (strcmp(args[0], "path") == 0)
  {
    // update the global path variable
    int i = 0;
    for (;;)
    {
      path[i] = args[i+1];
      //printf("Path[i]: %s\n",path[i]);
      if (args[i+1] != NULL){
        //printf("%s\n",args[i+1]);
      }
      i++;
      if (args[i + 1] == NULL)
      {
        break;
      }
    }

    int d = 0;
    while (path[d] != NULL){
      //printf("%s\n", path[d]);
      d++;
    }
    return 1;
  } else if(strcmp(args[0],"bad") == 0){
    handle_error();
    return -1;
  }
  return 0;
}

int bin_cmd_helper(char *args[])
{
  // char *parsedParams[100];
  char binPath[300];
  // cannot execute command ie no arguments or path specified
  if (path[0] == NULL || args == NULL || args[0] == NULL)
  {
    handle_error();
    return -1;
  }
  // iterate through all paths and try to execute a bin command
  int x = 0;
  while (path[x] != NULL)
  {
    strcpy(binPath, path[x]);
    strcat(binPath, "/");
    strcat(binPath, args[0]);
    x++;
    //printf("Attempting to access: %s\n", binPath);


    if (access(binPath,X_OK) != 0){
      //printf("Couldn't access: %s\n", binPath);
      x++;
      continue;
    }

    int pid = fork();
    if (pid == 0) 
    {
     
     // printf("Our bin path: %s\n", binPath);
      //long int stringlen = strlen(binPath);
      //sprintf(char *path, "%s/%s", "bin", ls)
      //strcat(binPath, "/");
      //printf("Our bin path 2: %s\n", binPath);

      //if(strcmp("/",binPath) != 0 || strcmp("./",binPath) != 0){
   //   printf("Args[0]: %s\n", args[1]);

      //}


  //    printf("Hi: %s\n", binPath);
      
    //  printf("PRinting args\n");
      //int j = 0;
      //while (args[j] != NULL){
        //printf("Path value: %s\n",args[j]);
      //  j++;
      //}
      //printf("\n");
      /*
      if(strcmp(args[0], "gcc") == 0){
        args[0] = binPath;
        printf("%s\n",args[0]);
        execv(binPath, args);  
      }
      */
      //printf("1st arg: %s\n", args[4]);
      //printf("\n\n\n\n");
      execv(binPath, args);
      //printf("Couldn't execute gcc\n");
      // error has occured with the bin path
      handle_error();
      //printf("ERROR: Could not execute %s\n", cmd);
      // exit(1);
    }
    else
    {
      int status;
      waitpid(pid, &status, 0);
    }
    x++;
  }
  //handle_error();
  return 0;
}

int main(int argc, char *argv[])
{
  path[0] = "/bin";
  int batch_mode = 0; // default is interactive mode
  char *filename;
  FILE *fp;
  char **arguments;
  int batch_finished = 0;
  // TODO: get batch mode working

  // check if we're in batch mode or not
  // checking for invalid args
  if (argc > 2){
    handle_error();
    exit(1);
  } 
  if(argc < 1){
    handle_error();
    exit(1);
  }
  if (argc == 2)
  {
    filename = argv[1];
    fp = fopen(filename, "r");
    if (!(fp))
    {
      //printf("Unable to open file");
      handle_error();
      //printf("Error: unable to open file");
      exit(1);
    }
    batch_mode = 1;
  }
  while (1)
  {

    if (batch_mode == 1)
    {
      // read each command line by line
      for (;;)
      {
        arguments = malloc(sizeof(char*) * 100);
        for(int i = 0; i < 100; i++){
          arguments[i] = malloc(sizeof(char) * 256);
        }
        int process_command = read_command(arguments, fp);
        if (process_command == -1)
        {
          batch_finished = 1;
          for(int i = 0; i < 100; i++){
            free(arguments[i]);
          }
          free(arguments);
          // some error has occured when we processed the command
          break;
        }
        else if (process_command == 1)
        { // if we called in a built in command then continue the loop (exit, path,  etc.)
          continue;
        }
        else
        { // need to execute a bin executable such as ls
          // printf("reached else case\n");
          int run_bin_cmd = bin_cmd_helper(arguments);
          // printf("%d\n", run_bin_cmd);
          if (run_bin_cmd == -1)
          { // some error occured so go back to top of loop
            continue;
          }
        }
        for(int i = 0; i < 100; i++){
          free(arguments[i]);
        }
        free(arguments);
      }
      break;
    }
    else
    {                   // not in batchmode
      printf("wish> "); // display prompt
      // fflush(stdout);
      // char *arguments[256];
      arguments = malloc(sizeof(char*) * 100);
      for(int i = 0; i < 100; i++){
        arguments[i] = malloc(sizeof(char) * 256);
      }
      int process_command = read_command(arguments, stdin);
      // fflush(stdin);
      // printf("%s\n",arguments[0]);
      if (process_command == -1)
      {
        for(int i = 0; i < 100; i++){
          free(arguments[i]);
        }
        free(arguments);
        // some error has occured when we processed the command
        break;
      }
      else if (process_command == 1)
      { // if we called in a built in command then continue the loop (exit, path,  etc.)
        continue;
      }
      else
      { // need to execute a bin executable such as ls
        // printf("reached else case\n");
        int run_bin_cmd = bin_cmd_helper(arguments);
        // printf("%d\n", run_bin_cmd);
        if (run_bin_cmd == -1)
        { // some error occured so go back to top of loop
          continue;
        }
      }
      for(int i = 0; i < 100; i++){
        free(arguments[i]);
      }
      free(arguments);
    }
    if(batch_finished == 1){
      break;
    }
  }
  return 0;
}
