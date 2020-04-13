
#include"functions.h"

//Function for initializing the project.
void initialize(char* h_path)
{
 system("clear");
 
 system("cat init");

 getcwd(h_path,30);
 strcat(h_path,"/bash_history");				//For getting the path of the history file.
}

//Function to execute command.
void execute_cmd(char **argv, int argc)
{
 int flag;

 if( !strcmp(argv[0],"cd") )								 //Checking for 'cd' command.
 {
  if( flag=chdir(argv[1]) )
   printf("cd : no such directory or permission Denied!\n");
 }
 else
 {
  if( !strcmp(argv[0],"ls") )								//Checking for 'ls' command.
  {
   argv[argc]="--color=auto";
   argv[argc+1]=NULL;
  }

  if( !strcmp(argv[0],"vi") )								//Checking for 'vi' command.
   argv[0]="vim";
 
  if( (flag=fork())<0 )
  {
   printf("Fork failed!\n");
   return;
  }
  
  if(!flag)												    //Executing command.
  {
   execvp(argv[0],argv);
   printf("command not found!\n");
   exit(-1);
  }
  else
  wait();
   
 }
 return;
}
  

//Function for obtaining the present working directory.
char* getpwd(char* s,char* buf,int size)
{
 int i=0;
	
 s=getcwd(s,size);
		
 while(*s)								//For getting the directory (not the entire path) the user is in.
 {
  if((*s)=='/')
   i=0;
 
  i++;
  s++;
 }

 return (s-i+1); 
}

/*Function for parsing the string entered by the user, i.e, commands.
  -str is the string entered by the user.
  -arr stores the end of a particular set of commands. arr[][0] stores '1' for pipe and '2' for redirection.
   arr[][1] stores the end of the particular command.
  -p_count contains the number of pipes or redirections in the program.
*/
int parse(char *str, char** argv, int* p_count, int* argc, int arr[][2])
{
 int i=strlen(str)-1, j, f_pipe=-1, l;
 
 (*p_count)=(*argc)=0;

 while(str[i]==' ' || str[i]=='\t')						//For removing the blank spaces at the end.
  str[i--]='\0';
				
 while( ((*str) == ' ') || ((*str) == '\t') )			//For removing the blank spaces at the begining.
  str++;

 argv[0]=str;

 for( i=1; (*str); i++, (*argc)++ )
 {
  while( ( (*str)!= ' ') && ( (*str)!='\t') && ((*str)!='\0') && ((*str)!='|') && ((*str)!='>') )
   str++;
    
  if( (*str) == '|' || (*str) == '>')
  {
   if( (*str) == '|')
    arr[(*p_count)][0]=1;
   else
    arr[(*p_count)][0]=2;

   if( ((*(str-1)) =='\0') || ((*(str-1)) ==' ') )
   {
    str++;
     
	while( (*str)==' ')
     str++;
	
	argv[i-1]=NULL;
	argv[i]=str;
	(*argc)--;
    
	arr[(*p_count)++][1]=i;
   }
   else
   {
    if( (*str) == '|')
	 arr[ (*p_count) ][0]=1;
	else
	 arr[ (*p_count) ][0]=2;
	 
    (*str++)='\0';
    
	while( (*str)==' ')
	 str++;
     
	argv[i+1]=str;
   
    argv[i]=NULL;
   
    arr[(*p_count)++][1]=i+1;

	i++;
   }

   f_pipe=1; 
  }
  else
   if((*str))
   {
    (*str)='\0';
    
	while( ((*str)==' ') || ((*str)=='\t') || ( !(*str)) )
	 str++;

    argv[i]=str;
      
   }
 }

 argv[i-1]=NULL;

 return f_pipe;
}

//Function to check for commands, other than the ones in /bin.
int check_cmd(char* argv)
{
 if(!strcmp(argv, "exit"))
   return 0;
  else
   if(!strcmp(argv, "history"))
	return 1;
   else
    if(!strcmp(argv, "help"))
	 return 2;
						
 return -1;
}


//Function to update history.
void update_hist(char* str, char* path)
{
 FILE* hist;
 hist=fopen(path,"a+");
 
 if(hist==NULL)
 {
  printf("Error!\n");
  exit(-1);
 }

 fputs(str,hist);
 fputc('\n',hist);
 fclose(hist);
}

//Function to display history.
void history(char* path)
{
 FILE* hist;
 char* str,buf[cmd_len];

 hist=fopen(path,"r");

 if(hist==NULL)
 {
  printf("Error!\n");
  return;
 }

 while( fscanf(hist,"%[^\n]s",buf)!=EOF )
 {
  puts(buf);
  fseek(hist,1,1);
 }

 fclose(hist);
}

//Function for piping.
void piping( char ** argv, int argc, int p_count, int arr[][2] )
{
 int pipefd[2], c_pid, pid, status;
 
 if( (pipe(pipefd)) < 0 )					//Piping...
 {
  printf("Piping failed!\n");
  return;
 }
 
 if( (c_pid=fork()) < 0 )					//Forking...
 {
  printf("Fork failed!\n");
  return;
 }

 if(!c_pid)
  child(pipefd, argv, argc, p_count, arr);
 else
 {
  close(pipefd[0]);
  close(pipefd[1]);

  while( (pid=wait(&status)) > 0 )
   if( (pid == c_pid) && status)
    printf("An error occured!\n");
 }

 return;
}

//Function executes the commands in the pipe.
//Static int flag is to signal it is the first of commands in the piping.
void child(int *pipefd, char **argv, int argc, int p_count, int arr[][2])
{
 int c_pid, pid, status, i ,j, s;
 static int flag=0;

 if(p_count)
 {
  p_count--;
 
  if( ( c_pid=fork()) < 0 )
  {
   printf("Fork failed!\n");
   exit(-1);
  }

  if(!c_pid)
  {
   /* The following routine (until line 283), is for shifting the
	* second command into the place of the first set of commands.
	* The corresponding counters are incremented, decremented or 
	* updated in the case of arr[][2]. */

   for(i=0, j=arr[0][1], s=0; s<argc ; i++, j++)
   {
    argv[i]=argv[j];

	if(argv[j])									
	 s++;
   }

   argc=argc-(arr[0][1]-1);

   for(i=0, j=arr[0][1]; i<p_count; i++)
   {
    arr[i][0]=arr[i+1][0];
	arr[i][1]=arr[i+1][1]-j;
   }
    
   flag++;
   
   child(pipefd, argv, argc, p_count, arr);
  }
  else
  {
   /* Closing the appropriate file descriptors */
   if( !flag)
   { 
    close(pipefd[0]);
	close(1);
	dup(pipefd[1]);
	close(pipefd[1]);

	flag++;
   }
   else
   {
    close(0);
	dup(pipefd[0]);
	close(pipefd[0]);
	
	if(p_count)
	{
	 close(1);
	 dup(pipefd[1]);
	 close(pipefd[1]);
	}
	else
	 close(pipefd[0]);
   }
   
   execvp(argv[0], argv);
   exit(-1);
  }
 }
 else
 {
  flag=0;
  
  /* Closing the appropriate file descriptors */
  close(pipefd[1]);
  close(0);
  dup(pipefd[0]);
  close(pipefd[0]);
  
  execvp(argv[0], argv);
  exit(-1);
 }
}

