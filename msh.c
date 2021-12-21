//Kevin Phan

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
// 
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

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
// so we need to define what delimits our tokens.
// In this case  white space
// will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports five arguments


//max number of command that can be held in history array
#define MAX_HISTORY_SIZE 15


//max number of pids that can be held in pid array
#define PID_HISTORY_SIZE 15


/* Parse input */
char *token[MAX_NUM_ARGUMENTS];

//array that holds the last 15 commands
char HistoryArr[MAX_HISTORY_SIZE][MAX_COMMAND_SIZE];
//--counter to keep track of commands
int HistoryCounter = 0;

//array to hold last 15 pids
pid_t PidHistory[PID_HISTORY_SIZE];
//counter to keep track of pid HistoryCounter
int PidCounter = 0;

int main(void)
{
	
	char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
	
	
	while( 1 )
	{
		// Print out the msh prompt
		printf ("msh> ");
		
		// Read the command from the commandline.  The
		// maximum command that will be read is MAX_COMMAND_SIZE
		// This while command will wait here until the user
		// inputs something since fgets returns NULL when there
		// is no input
		while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
		
		int   token_count = 0;                                 
		
		// Pointer to point to the token
		// parsed by strsep
		char *argument_ptr;                                         
		
		char *working_str  = strdup( cmd_str );                
		
		//if the ! command is entered
		if(working_str[0] == '!')
		{
			//convert number that comes after ! to int and store in bang_index
			int bang_index = atoi(&working_str[1]);
			
			//use int converted bang value to find command from history array
			//and store in working string for use now
			if(bang_index < HistoryCounter)
			{
				strcpy(working_str, HistoryArr[bang_index]);
			}	
		}
		
		// we are going to move the working_str pointer so
		// keep track of its original value so we can deallocate
		// the correct amount at the end
		char *working_root = working_str;
		
		
		// Tokenize the input strings with whitespace used as the delimiter
		while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
		(token_count<MAX_NUM_ARGUMENTS))
		{
			token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
			if( strlen( token[token_count] ) == 0 )
			{
				token[token_count] = NULL;
			}
			token_count++;
		}
		
		
		//if the user does not type anything then print next msh prompt
		if(token[0] == NULL)
		{
			continue;
		}
		
		//if user enters "quit" or "exit" then exit the program
		else if(strcmp(token[0], "quit") == 0 || strcmp(token[0], "exit") == 0)
		{
			exit(0);    
		}
		
		//if user enters "cd" then change directory
		else if(strcmp(token[0], "cd") == 0)
		{
			//if user types something after cd then change directory
			//to requested directory, otherwise print invalid dir
			if(token[1] != NULL)
			{
				chdir(token[1]);
			}
			
			else
			{
				printf("Invalid directory\n");
			}
		}
		
		
		//if user enters "history"
		else if(strcmp(token[0], "history") == 0)
		{
			//add "history to the history array so it could be displayed right after"
			//if there's less than 15 in the history array then add command to end
			if(HistoryCounter < MAX_HISTORY_SIZE)
			{
				strcpy(HistoryArr[HistoryCounter], working_root);
				HistoryCounter++;
			}
			//if history array currently holds 15 commands then shift it and add new to end
			else
			{
				int i;
				for(i = 0; i < MAX_HISTORY_SIZE - 1; i++)
				{
					strcpy(HistoryArr[i], HistoryArr[i+1]);
				}
				strcpy(HistoryArr[MAX_HISTORY_SIZE - 1], working_root);
			}
			
			//display history array
			int i;
			for(i = 0; i < HistoryCounter; i++)
			{
				printf("%d %s\n",i, HistoryArr[i]);
			}
		}
		
		
		//if user enters "listpids"
		else if(strcmp(token[0],"listpids") == 0)
		{
			//display pid array
			int i;
			for(i = 0; i < PidCounter; i++)
			{
				printf("%d %d\n",i, PidHistory[i]);
			}	
		}
		
		pid_t pid = fork();
		
		//if the pid array stores less than the max allowed then store 
		//new pid id inside array and increment to next spot
		if(PidCounter < PID_HISTORY_SIZE)
		{
			PidHistory[PidCounter] = pid; 
			PidCounter++;
		}
		//if pid array currently stores max amount then shift all values in 
		//array 1 spot back and add new pid id to end of array
		else
		{
			int i;
			for(i = 0; i < PID_HISTORY_SIZE - 1; i++)
			{
				PidHistory[i] = PidHistory[i+1];
			}
			
			PidHistory[PID_HISTORY_SIZE - 1] = pid;
		}
		
		
		//if history array stores less than max allowed then store current 
		//command in array and increment index
		if(HistoryCounter < MAX_HISTORY_SIZE)
		{
			if(strcmp(token[0],"history") == 0)
			{
				
			}
			else
			{
				strcpy(HistoryArr[HistoryCounter], working_root);
				HistoryCounter++;
			}
		}
		//if history array currently holds 15 commands then shift it and add new to end
		else
		{
			//if the history command was just entered then don't add it to array
			//because it was already added earlier
			if(strcmp(token[0],"history") == 0)
			{
				
			}
			//otherwise add command to history array
			else
			{
				int i;
				for(i = 0; i < MAX_HISTORY_SIZE - 1; i++)
				{
					strcpy(HistoryArr[i], HistoryArr[i+1]);
				}
				strcpy(HistoryArr[MAX_HISTORY_SIZE - 1], working_root);
			}
		}
		
		//if in the child process
		if(pid == 0)
		{
			//execute user command
			int ret = execvp(token[0], &token[0]);
			
			//if user command is invalid print command not found
			if(ret == -1)
			{
				//history, listpids, and cd will cuase command not found to print 
				//this if statement eliminates the invalid print for these cmds
				if((strcmp(working_root,"history") == 0) || (strcmp(working_root,"listpids") == 0) ||
				(strcmp(working_root,"cd") == 0))
				{
				
				}
			
				else
				{
					printf("%s: Command not found\n", token[0]);
				}
			
			}
			
			exit(0);
		}
		
		//parent process waits for child to finish
		else
		{
			int status;
			wait(&status);
		}

		free( working_root );
		
	}
	return 0;
}
			
									