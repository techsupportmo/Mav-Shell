
/*
    Name: Mohammed Ahmed
    ID:   1001655176

*/

// The MIT License (MIT)
//
// Copyright (c) 2016, 2017, 2021 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// 7f704d5f-9811-4b91-a918-57c1bb646b70
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n" // We want to split our command line up into tokens 
                           // so we need to define what delimits our tokens.   
                           // In this case  white space                        
                           // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11 // Mav shell can support up to 10 command line parameters

#define MAX_HISTORY_ARGUMENTS 15 //Mav shell will store up to 15 previous commands entered

#define MAX_PIDS 15 //Max number of pids stored in history

int main()
{

    int i;

    char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);

    //pid
    int pid_index = 0;
    int pid_count = 0;


    //initialize history array,index, and count
    char *history[MAX_HISTORY_ARGUMENTS];
    int history_index = 0;
    int history_count = 0;
    int bang = 0;
    

    //allocate memory for each index
    for(i = 0; i < MAX_HISTORY_ARGUMENTS; i++)
    {
      history[i] = ( char * ) malloc(  MAX_COMMAND_SIZE );
      
    }

    


  while (1)
  {

    

    // Print out the msh prompt
    printf("msh> ");


    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin));
    


    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;

    char *working_str = strdup(cmd_str);

   


    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    bang = 0;

    //checks if first element in command is '!'
    if(working_str[0] == '!')
    {
      int history_num = atoi(&working_str[1]);


      //checking if valid command
      if(history_num >= 0 && history_num < history_count)
      {
        strcpy(working_str, history[history_num]);
      }
      else
      {
        printf("Command not in history\n");

        //increments if command not found
        bang++;
        
      }

    }

    // Tokenize the input strings with whitespace used as the delimiter
    while (((argument_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    
    // My Shell Functionality


    //checks for null input
    // ex. if a user enters blank line into msh>
    if(token[0] != NULL)
    {

      char * line = strdup(cmd_str);

    
    
    history[history_index] = line;

    //incrementing each time element
    //is added to the history
    history_index++;
    history_count++;

    //resets history_index
    if(history_index > MAX_HISTORY_ARGUMENTS - 1)
    {
      history_index = 0;
    }

    //max pid displayed is 15 (MAX_HISTORY_ARGUMENTS)
    if(history_count > MAX_HISTORY_ARGUMENTS)
    {
      history_count = MAX_HISTORY_ARGUMENTS;
    }
    
    



    //check if the user wants to exit shell
    if(strcmp(token[0],"exit") == 0 || strcmp(token[0],"quit") == 0)
    {
      exit(0);
    }

    pid_t pid_list[15];



    //fork process
    pid_t pid = fork();

    

    //store pid in pid list
    pid_list[pid_index] = pid;

    //increment index and count
    pid_index++;
    pid_count++;


    //resets pid_index
    // pid_index is where the new pid will be stored
    if(pid_index > MAX_PIDS - 1)
    {
      pid_index = 0;
    }

    //max pid displayed is 15 (MAX_PIDS)
    if(pid_count > MAX_PIDS)
    {
      pid_count = MAX_PIDS;
    }

    

    

    //parent
    if(pid != 0)
    {
      //checks if user entered cd command
      if(strcmp(token[0],"cd") == 0)
      {
        //change directory based on second argument (ex. cd ..)
        chdir(token[1]);

        
        
      }
    }
    


    //child
    if (pid == 0)
    {
      

      //calling exec() to execute command
      int ret = execvp(token[0], &token[0]);



      if (ret == -1)
      {

        //output if the command cannot be executed
        if(strcmp(token[0],"listpids") != 0 && strcmp(token[0],"history") != 0 
        && strcmp(token[0],"cd") != 0 && bang != 1)
        {
          printf("%s: Command not found.\n",token[0]);
          

        }
        
        
        break;
        
        

      }
      
    }
    else
    {
      int status;
      wait(&status);
      
    }

    

    //output PID list if user types "listpids"
    if(strcmp(token[0],"listpids") == 0)
    {
      printf("\n");
      printf("List of PIDS:\n");

      //prints how many pids there are (up until MAX_PIDS)
      for(i = 0; i < pid_count; i++)
      {
        printf("%d.  %d\n",i+1,pid_list[i]);
      }

      printf("\n");

    }

    //output history if the user types "history"
    if(strcmp(token[0],"history") == 0)
    {

      printf("\n");
      printf("HISTORY:\n");

      //prints how many commands in history (up until MAX_HISTORY_ARGUMENTS)
      for(i = 0; i < history_count; i++)
      {
        printf("%d.  %s",i,history[i]);
      }


      printf("\n");

    }
    
      
    }
    
    


    free(working_root);
    
  }

  //free memory for history
  for(i = 0; i < MAX_HISTORY_ARGUMENTS; i++)
  {
    free(history[i]);
    
  }

  
  return 0;
}
