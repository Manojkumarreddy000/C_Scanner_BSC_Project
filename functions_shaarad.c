#include"header.h"


void remove_comments(char *file_name)
{
	unsigned int state=1, line_no=1;
    char ch;
	char single_line_comments_file[MAX_SIZE];
	char multi_line_comments_file[MAX_SIZE];


	strcpy(single_line_comments_file,file_name);
	single_line_comments_file[ strlen(single_line_comments_file)-2 ]='\0';
	strcat(single_line_comments_file,"_");
	strcat(single_line_comments_file,comments_file);

	strcpy(multi_line_comments_file,file_name);
	multi_line_comments_file[ strlen(multi_line_comments_file)-2 ]='\0';
	strcat(multi_line_comments_file,"_");
	strcat(multi_line_comments_file,Comments_file);


    FILE *ifp,
			  *sofp, // Single line comments
	          *mofp, // Multi line comments
			  *tmp_fp;
    ifp=fopen(file_name,"r");
    if(ifp==NULL)
    {   fprintf(stderr,"Unable to open file \"%s\"\n",file_name);
        exit(1);
    }
    sofp=fopen(single_line_comments_file,"w");
    if(sofp==NULL)
    {   fprintf(stderr,"Unable to open file \"%s\"\n",single_line_comments_file);
        exit(1);
    }
    mofp=fopen(multi_line_comments_file,"w");
    if(mofp==NULL)
    {   fprintf(stderr,"Unable to open file \"%s\"\n",multi_line_comments_file);
        exit(1);
    }
    tmp_fp=fopen(temp_file,"w");	// Temporary file into which all the contents of 'file_name'
    if(tmp_fp==NULL)				// except single line comments are copied
    {   fprintf(stderr,"Unable to open file \"%s\"\n",temp_file);
        exit(1);
    }
	ch=fgetc(ifp);
	while( !feof(ifp) )
	{
		if(ch=='\n')
			line_no++;
		switch(state)
		{
			case 1: // Starting state
						switch(ch)
						{
							case '\"' : 
										fputc(ch,tmp_fp);
										state=2;
										break;

							case '/'  :
										state=4;
										break;

							case '*' :
										fputc(ch,tmp_fp);
										state=11;
										break;

							case '\'': 
										fputc(ch,tmp_fp);
										state=8;
										break;

							default	:
										fputc(ch,tmp_fp);

						}
						break;

			case 2: // In quotes
						switch(ch)
						{
							case '\\' :
										fputc(ch,tmp_fp);
										state=3;
										break;

							case '\"' :
										fputc(ch,tmp_fp);
										state=1;
										break;

							default :
										fputc(ch,tmp_fp);
						}
						break;

			case 3: // Escaping quotes in quotes
						fputc(ch,tmp_fp);
						state=2;
						break;

			case 4: // Found '/'
						switch(ch)
						{
							case '*' :
										state=5;
										break;

							case '/' :
										state=7;
										break;
							default :
										fputc('/',tmp_fp);
										fputc(ch,tmp_fp);
										state=1;
										break;
						}
						break;

			case 5: // In multi line comments
						switch(ch)
						{
							case '*' :
										state=6;
										break;
							default :
										fputc(ch,mofp);
										break;
						}
						if(ch=='\n')
							fputc(ch,tmp_fp);
						break;

			case 6: // Found a '*' in multi line comments
						switch(ch)
						{
							case '*' :
										fputc('*',mofp);
										break;
							case '/' :
										state=1;
										break;
							default :
										fputc('*',mofp);
										fputc(ch,mofp);
										state=5;
										break;
						}
						if(ch=='\n')
							fputc(ch,tmp_fp);
						break;

			case 7: // In single line comments
						switch(ch)
						{
							case '\n' :
										fputc('\n',sofp);
										state=1;
										break;

							case '\\' :
										fputc(ch,sofp);
										state=12;
										break;

							default :
										fputc(ch,sofp);
										if(ch=='\n')
											fputc(ch,tmp_fp);
										break;
						}

						break;
							
			case 12:	// Escaping new line in single line comment
						fputc(ch,sofp);
						state=7;
						break;

			case 8: // In '\''
						switch(ch)
						{
							case '\\' :
										fputc(ch,tmp_fp);
										state=9;
										break;
							default :
										fputc(ch,tmp_fp);
										state=10;
										break;
						}
						break;

			case 9: // Escaping in ''s
						fputc(ch,tmp_fp);
						state=10;
						break;

			case 10:
						switch(ch)
						{
							case '\'' :
										fputc(ch,tmp_fp);
										state=1;
										break;
							default :
										fprintf(stderr,"Expected \'\'\' in line %d\n",line_no);
										exit(1);
										break;
						}
						break;
			case 11: // Found a stray '*'
						switch(ch)
						{
							case '*' :
										fputc(ch,tmp_fp);
										break;
							case '/' :
										fprintf(stderr,"Unexpected pattern */ in line no. %d\n",line_no);
										exit(1);
										break;
							default :
										fputc(ch,tmp_fp);
										state=1;
										break;
						}
						break;

		}
		ch=fgetc(ifp);
	}
		fclose(ifp);
		fclose(sofp);
		fclose(mofp);
		fclose(tmp_fp);
		rename(temp_file,file_name);
		return;
}





// Adds the macro *M to the linked list of macros pointed to by head
Macro_list* add_macro(Macro_list *head,Macro *M)
{
    Macro_element *E=(Macro_element*)malloc(sizeof(Macro_element));
    if(E==NULL)
    {   fprintf(stderr,"Memory unavailable\n");
        return NULL;
    }
    E->data=M;
    E->next=head;
    return E;
}

Macro_list* create_macro_list()
{
    return NULL;
}

// Searches for valid macro definitions in 'file_name' and returns a linked list of all the macros found
Macro_list* get_macros(char *file_name)
{
    char ch,str[MAX_SIZE],
         arg[ARG_NUM];
    bool done;
    Macro_list *List=create_macro_list();
    Macro *Mac;
    int i,j,
        n_of_args,
        arg_top=-1,
        line_no=1;
    FILE *ifp;
    ifp=fopen(file_name,"r");
    if(ifp==NULL)
    {   fprintf(stderr,"Unable to open file \"%s\"\n",file_name);
        exit(1);
    }

    ch=fgetc(ifp);
    i=0;
    while(!feof(ifp))
    {   
        if(ch=='#' || isalpha(ch) )
            str[i++]=ch;
        else
        {   if(ch!=' ')
            {  
                if(ch=='\n')
                    line_no++;
                ch=fgetc(ifp);
                i=0;
                continue;
            }
            str[i]='\0';
            i=0;
            if(!strcmp(str,"#define"))
            { 
                // Move till start of macro identifier
                while(!feof(ifp) && ch==' ' && ch!='\n')
                    ch=fgetc(ifp);
                if(ch=='\n' || feof(ifp))
                {   fprintf(stderr,"Error! Incomplete definition of macro in line : %d in file %s\n",
                                   line_no,file_name);
                    i=0;
                    continue;
                }

                // Obtaining macro identifier
                str[0]=ch;
                i=1;
                while(!feof(ifp)) 
                {   ch=fgetc(ifp);
                    if(isalnum(ch))
                        str[i++]=ch;
                    else
                    {   str[i]='\0';
                        break;
                    }
                }
                if(ch=='\n' || feof(ifp))
                {   fprintf(stderr,"Error! Incomplete definition of macro in line : %d in file %s\n",
                                   line_no,file_name);
                    i=0;
                    continue;
                }
                // Moving till start of replacing string or list of paramenters
                while(!feof(ifp) && ch==' ' )
                    ch=fgetc(ifp);
                if(ch=='\n' || feof(ifp))
                {   fprintf(stderr,"Error! Incomplete definition of macro in line : %d in file %s\n",
                                   line_no,file_name);
                    i=0;
                    continue;
                }
                Mac=(Macro*)calloc(1,sizeof(Macro));
                if(Mac==NULL)
                {   fprintf(stderr,"Memory full.n");
                    exit(1);
                }
                arg_top=-1;
                strcpy(Mac->identifier,str);
                // Checking and storing arguments
                if(ch=='(')
                    do
                    {   done=false;
                        ch=fgetc(ifp);
                        i=0;
                        while(!feof(ifp) && ch!=',' && ch!=')' && ch!='\n')
                        {   str[i++]=ch;
                            ch=fgetc(ifp);
                        }
                        if(ch=='\n')
                            line_no++;
                        if(feof(ifp))
                        {   fprintf(stderr,"Error! Incomplete definition of macro in line : %d in file %s\n",
                                           line_no,file_name);
                            i=0;
                            continue;
                        }
                        if(ch==')')
                        {   ch=' ';
                            done=true;
                        }
                        str[i]='\0';
                        i=0;
                        arg_top++;
                        strcpy(Mac->arg[arg_top],str);
                    } while(!done);                
                Mac->arg_cnt=arg_top+1;
                Mac->line_no=line_no;
                // Moving till start of replacing string
                while(!feof(ifp) && ch==' ' && ch!='\n')
                    ch=fgetc(ifp);
                if(ch=='\n' || feof(ifp))
                {   fprintf(stderr,"Error! Incomplete definition of macro in line : %d in file %s\n",
                                   line_no,file_name);
                    i=0;
                    continue;
                }
                // Obtaining replacing string
                str[0]=ch;
                i=1;
                while(!feof(ifp)) 
                {   ch=fgetc(ifp);
                    if(ch!='\n')
                        str[i++]=ch;
                    else
                    {   str[i]='\0';
                        i=0;
                        if(ch=='\n')
                            line_no++;
                        break;
                    }
                }
                strcpy(Mac->replacing_string,str);
                List=add_macro(List,Mac);
            }
        }
        ch=fgetc(ifp);
    }
    fclose(ifp); 
    return List;
}

// Fuctions substitutes all macros called in the source file "file_name"
void substitute_all_macros(char *file_name,Macro_list *L)
{
   char ch;
    FILE *ifp,*ofp,*tmp_fp;
    Macro_element *E=L;
    while(E!=NULL)
    {   substitute_macro(file_name,E->data);
        E=E->next;
    }
} 

// Substitutes the macro *M called in the file "file_name"
void substitute_macro(char *file_name,Macro *M)
{
    FILE *tmp_fp,
         *location,
         *ifp;
    ifp=fopen(file_name,"r");
    if(ifp==NULL)
    {   fprintf(stderr,"Unable to open file \"%s\"\n",file_name);
        exit(1);
    }
    tmp_fp=fopen(temp_file,"w");
    if(tmp_fp==NULL)
    {   fprintf(stderr,"File %s cannot be opened\n",temp_file);
        exit(1);
    }
    bool found/*a macro*/;
    int i,j;
    long pos;
    char ch,str[ARG_NUM][ARG_SIZE],replacing_string[MAX_SIZE];
    if(ifp==NULL)
    {   fprintf(stderr,"Error opening %s\n",file_name);
        exit(1);
    }
    ch=fgetc(ifp);
    while(!feof(ifp))
    { 
        if(ch==EOF)
            break;
        if(ch!=M->identifier[0])
        {   fputc(ch,tmp_fp);
            ch=fgetc(ifp);
            continue;
        }
        else
        {   location=ifp;
            pos=ftell(ifp);
            i=0;
            found=true;
            // Check the identifier fully
            while( !feof(ifp) && i<strlen(M->identifier) )
            {   
                if(ch!=M->identifier[i])
                {   found=false; /*False positive*/
                    break;
                }
                i++;
                ch=fgetc(ifp); 
            } 
            if(!found)
            { 
                fseek(ifp,pos,SEEK_SET);
                fputc(M->identifier[0],tmp_fp);
                ch=fgetc(ifp);
                continue;
            }
            else  /* Macro identifier found to be used */
            { 
                if(!M->arg_cnt)
                {   fputs(M->replacing_string,tmp_fp);
                }
                else
                {   while( !feof(ifp) && ch==' ')
                        ch=fgetc(ifp);
                    if(ch!='(')
                    {   fputc(M->identifier[0],tmp_fp);
                        fseek(ifp,pos,SEEK_SET);
                        ch=fgetc(ifp);
                        continue;
                    }
                    i=0; j=0;
                    ch=fgetc(ifp);
                    while(!feof(ifp) && ch!=')')
                    {   str[j][i++]=ch;
                        ch=fgetc(ifp);
                        if(ch==',' || ch==')')
                        {   str[j++][i]='\0';
                            i=0;
                            if(ch==',')
                                ch=fgetc(ifp);
                        }

                    }
                    for(i=0;i<j;i++)
                    if(j!=M->arg_cnt)
                    {   fprintf(stderr,"Invalid number of arguments in macro call\n");
                        fseek(ifp,pos,SEEK_SET);
                        fputc(M->identifier[0],tmp_fp);
                        ch=fgetc(ifp);
                        continue;
                    }
                    else
                    {   strcpy(replacing_string,M->replacing_string);
                        for(i=0;i<M->arg_cnt;i++)
                            replace_substr(replacing_string,M->arg[i],str[i]);
                        fputs(replacing_string,tmp_fp);
                        ch=fgetc(ifp);
                    }
                }     
            }
        }
    }
    rename(temp_file,file_name);
    fclose(ifp);
    fclose(tmp_fp);    
} 


// Searches for the use of 'if' construct in the file pointed to by *ifp
// and puts it to a file 'file_name'
void print_if(char *file_name,FILE **ifp)
{	
	int cnt;
	char str[MAX_SIZE];
	FILE *ofp=fopen(file_name,"w");
	if(ofp==NULL)
	{	fprintf(stderr,"Error opening file \"%s\"\n",file_name);
		return;
	}
	fseek(*ifp,0,SEEK_SET);
	fgets(str,MAX_SIZE-1,*ifp);
	while(!feof(*ifp))
	{	if(!strcmp(str,"if") || !strcmp(str,"if\n") )
		{	// "if" keyword found
			fgets(str,MAX_SIZE-1,*ifp);
			if( !strcmp(str,"(") || !strcmp("(\n",str) )
			{	cnt=1;
				fprintf(ofp,"if ");
				// Parse till close of parantheses
				while(!feof(*ifp) && cnt>0 )
				{	
					if(strcmp(str,"\n") && str[ strlen(str)-1 ]=='\n' )
						str[ strlen(str)-1 ]=' ';
					fputs(str,ofp);
					fgets(str,MAX_SIZE-1,*ifp);
					if(!strcmp(str,")\n"))
						cnt--;
					else if(!strcmp(str,"(\n"))
						cnt++;
				}
				fprintf(ofp," )");
				// Print till end of 'if' block
				print_statement(ifp,&ofp);
				fgets(str,MAX_SIZE-1,*ifp);
				while( !feof(*ifp) && (!strcmp(str,"\n") || !strcmp(str,"\t\n") ) )
					fgets(str,MAX_SIZE-1,*ifp);
				// Check for 'else'
				if(!strcmp(str,"else\n"))
				{	fprintf(ofp,"else  ");
					// Print till end of 'else'
					print_statement(ifp,&ofp);
				}

			}
			else	// No opening paranthesis after 'if'
			{	fgets(str,MAX_SIZE-1,*ifp);
				fprintf(stderr,"Invalid use of if construct\n");
		 	}
		}
		else
			fgets(str,MAX_SIZE-1,*ifp);
	}
}

// Prints one statement or a block of statements
void print_statement(FILE **ifp, FILE **ofp)
{
	static int depth=0;
	++depth;
	char construct[][10]= {  "for",
							"if",
							"while",
							"do"
						 } ;

	int cnt,i,index,pos,length,bracket_count=0,semicolon_count=0;
	bool found;// a construct
	char str[MAX_SIZE];
	fgets(str,MAX_SIZE-1,*ifp);
	while(!feof(*ifp) && ( !strcmp(str,"\n") || !strcmp(str,"\t\n") || !strcmp(str,"\n\n") ) )
	{
		if( strcmp(str,"\n") && str[ strlen(str)-1 ]=='\n' )
			str[ strlen(str)-1 ]=' ';
		fputs(str,*ofp);
		fgets(str,MAX_SIZE-1,*ifp);
	}
	if(strcmp(str,"\n") && str[ strlen(str)-1 ]=='\n' )
		str[ strlen(str)-1 ]='\0';
	if(!strcmp(str,";"))
	{
		fputs(str,*ofp);
		return;
	}

	// If an opening brace is found, print till a closing brace
	if(!strcmp(str,"{"))
	{	cnt=1;
		while(!feof(*ifp) && cnt>0)
		{	if(strcmp(str,"\n") && str[ strlen(str)-1 ]=='\n' )
				str[ strlen(str)-1 ]=' ';
			fputs(str,*ofp);
			fgets(str,MAX_SIZE-1,*ifp);
			if(!strcmp(str,"{\n"))
				cnt++;
			else if(!strcmp(str,"}\n"))
					cnt--;
		}
		fprintf(*ofp,"}\n");
		depth--;
		return;
	}

	// If an opening brace is not found, check if we are at the start
	// of a construct such as 'if' etc.
	else
	for(i=0,found=false;i<4;i++)
		if(!strcmp(str,construct[i]))
		{	found=true;
			index=i;
		}

	if(found)
	{
		switch(index)
		{
			case 1 :    // if construct found
						// parse till close of parantheses and call 'print_statement'
						fputs("if",*ofp);
						fgets(str,MAX_SIZE-1,*ifp);
						while( !feof(*ifp) && ( !strcmp(str,"\n") || !strcmp(str,"\t\n") ) )
						{	if(strcmp(str,"\n"))
								str[ strlen(str)-1 ]='\0';
							fputs(str,*ofp);
							fgets(str,MAX_SIZE-1,*ifp);
						}
						if(strcmp(str,"(\n"))
					 	{	found=0;
							fprintf(stderr,"Invalis use of \"if\" construct\n");
							break;
						}
						else
						{	
							cnt=1;
							while( !feof(*ifp) && cnt>0 )
							{	
								if( strcmp(str,"\n") && str[ strlen(str)-1 ]=='\n')
									str[ strlen(str)-1 ]=' ';
								fputs(str,*ofp);
								fgets(str,MAX_SIZE-1,*ifp);
								if( !strcmp(str,"(\n") )
									cnt++;
								if( !strcmp(str,")\n") )
									cnt--;
							}
							fputs(" )",*ofp);
							print_statement(ifp,ofp);
							pos=ftell(*ifp);
							fgets(str,MAX_SIZE-1,*ifp);
							while( !feof(*ifp) && ( !strcmp(str,"\n") || !strcmp(str,"\t\n")  ) )
							{	if(strcmp(str,"\n"))
									str[ strlen(str) -1]='\0';
								fputs(str,*ofp);
								fgets(str,MAX_SIZE-1,*ifp);
							}
							if(!strcmp(str,"else\n"))
							{
								fputs("else ",*ofp);
								print_statement(ifp,ofp);
							}
							else
								fseek(*ifp,pos,SEEK_SET);

						}
						break;
			case 0: 
					//for construct found     
					// Parse till end of parantheses checking the number of semi colons
					// and call 'print_statement'
					fputs("for",*ofp);
					fgets(str,MAX_SIZE-1,*ifp);
					if(strcmp(str,"\n"))
						str[strlen(str)-1]='\0';

					if(!strcmp(str,"("))
					{
						bracket_count=1;
					    fputs(str,*ofp);

						fgets(str,MAX_SIZE-1,*ifp);
						if(strcmp(str,"\n"))
							str[strlen(str)-1]='\0';

						while(!feof(*ifp) && bracket_count!=0)
						{
							fputs(str,*ofp);


							if(!strcmp(str,"("))
								bracket_count++;

							if(!strcmp(str,")"))
								bracket_count--;

							if(!strcmp(str,";"))
								semicolon_count++;

							if(semicolon_count>2)
							{
								printf("Wrong syntax in for loop");
								return;
							}

						 fgets(str,MAX_SIZE,*ifp);
     					 if(strcmp(str,"\n"))
							 str[strlen(str)-1]='\0';

						}


						if(bracket_count==0)
						{
							if(semicolon_count<2)
							{
								printf("Semicolon missing inside the for statement");
								return;
							}


							fputs(str,*ofp);
							print_statement(ifp,ofp);
						}

						else
						{
							printf("the bracket in the end of for statement is not closed");
							return;
						}
					}
					else
					{
						printf("for loop not used properly");
						return;
					}

					break;

					
			case 2:
					//"while" construct found
	    		    //Parse till close of parantheses and call 'print_statement'
					
					fputs("while",*ofp);
					fgets(str,MAX_SIZE-1,*ifp);
					if(strcmp(str,"\n"))
						str[strlen(str)-1]='\0';


					if(!strcmp(str,"("))
					{
						bracket_count=1;
						fputs(str,*ofp);

						fgets(str,MAX_SIZE-1,*ifp);
						str[strlen(str)-1]='\0';

						while(!feof(*ifp) && bracket_count!=0)
						{
							fputs(str,*ofp);


							if(!strcmp(str,"("))
								bracket_count++;

							if(!strcmp(str,")"))
								bracket_count--;


						 fgets(str,MAX_SIZE,*ifp);
						 if(strcmp(str,"\n"))
							 str[strlen(str)-1]='\0';

						}

						if(bracket_count==0)
						{
							fputs(str,*ofp);
							print_statement(ifp,ofp);
						}

						else
						{
							printf("The bracket in the end of while statement is not closed\n");
							return;
						}
					}
					else
					{
						printf("While loop not used properly\n");
						return;
					}


					break;

					
			case 4: 
					// do  while
					fputs(str,*ofp);
					print_statement(ifp,ofp);


					fgets(str,MAX_SIZE,*ifp);
					if(strcmp(str,"\n"))
						str[strlen(str)-1]='\0';

								while(!feof(*ifp)  && (!strcmp(str,"\t") || !strcmp(str,"\n")) )
								{
									fgets(str,MAX_SIZE,*ifp);
									if(strcmp(str,"\n"))
										str[strlen(str)-1]='\0';
								}

                   
                    if(!strcmp("while",str))
					{

						fputs("while",*ofp);
						fgets(str,MAX_SIZE-1,*ifp);
						if(strcmp(str,"\n"))
							str[strlen(str)-1]='\0';

	
						if(!strcmp(str,"("))
						{
							bracket_count=1;
							fputs(str,*ofp);
	
							fgets(str,MAX_SIZE-1,*ifp);
							str[strlen(str)-1]='\0';
	
							while(!feof(*ifp) && bracket_count!=0)
							{
								fputs(str,*ofp);
	

								if(!strcmp(str,"("))
									bracket_count++;
			
								if(!strcmp(str,")"))
									bracket_count--;
	

							 fgets(str,MAX_SIZE,*ifp);
							 if(strcmp(str,"\n"))
								 str[strlen(str)-1]='\0';
							}
							if(bracket_count==0)
							{  
								fgets(str,MAX_SIZE,*ifp);
								if(strcmp(str,"\n"))
									str[strlen(str)-1]='\0';
								while(!feof(*ifp)  && (!strcmp(str,"\t") || !strcmp(str,"\n")) )
								{
									fgets(str,MAX_SIZE,*ifp);
									if(strcmp(str,"\n"))
										str[strlen(str)-1]='\0';
								}

								if(!strcmp(str,";"))
									fputs(str,*ofp);
								else
								{
									printf("In do while statement semi colon is missing after the while statement\n");
									 return;
								}
							}

								 



							}
							else
							{
									printf("\nImproper use of the do while loop\n");
									return;
							}
						}

						break; // end of case 4 or do while


					
					





		} // end of switch case

	}
	if(!found)
	{	while(!feof(*ifp) && strcmp(str,";\n"))
		{	if( strcmp(str,"\n") && (str[strlen(str)-1]=='\n') )
				str[strlen(str)-1]=' ';
			fputs(str,*ofp);
			fgets(str,MAX_SIZE-1,*ifp);
		}
		fprintf(*ofp,";\n");
		depth--;
		return;
	}
	depth--;
}

// Replaces substring 'c1' in 'ch' by 'c2'
void replace_substr(char *ch, char *c1, char *c2) 
{
  int i=-1,j,k,cnt,
      length=strlen(ch),
      len1=strlen(c1),
      len2=strlen(c2)  ;
  while( ch[++i]!='\0' )     
  {
    for(j=0;j<len1;j++)
      if(ch[i+j]!=c1[j])
        break;
    if(j==len1)
    {
      if(len1<len2)
        for(cnt=0;cnt<len2-len1;cnt++)
        {
          for(j=length+1;j>i;j--)
            ch[j]=ch[j-1];
          length++;
        }
      else
        if(len1>len2)
          for(cnt=0;cnt<len1-len2;cnt++)
          {
            for(j=i;j<length;j++)
              ch[j]=ch[j+1];
            length--;
          }
      for(j=0;j<len2;j++)
        ch[i+j]=c2[j];
      i+=(len2-1);
    }
  }
  return;
}

Function* create_function()
{
	Function* f=(Function*)calloc(1,sizeof(Function));
	if(f==NULL)
	{	fprintf(stderr,"Memory unavailable\n");
		return NULL;
	}
	f->identifier[0]='\0';
	f->return_type[0]='\0';
	f->arg_cnt=0;
	f->called_fn_count=0;
	return f;
}

// Adds a function to the list of functions
Function_list add_function(Function_list L,Function *f)
{
	Function_element *F=L;
	Function_element *E=(Function_element*)calloc(1,sizeof(Function_element));
	if(E==NULL)
	{	fprintf(stderr,"Memory unavailable\n");
		return L;
	}
	while(F!=NULL && strcmp(F->data->identifier,f->identifier) )
		F=F->next;
	if(F==NULL)
	{
		E->data=f;
		E->next=L;
		L=E;
	}
	else
	{	if(F->data->status==defined)
		{	fprintf(stderr,"Multiple definition of fn %s\n",F->data->identifier);
			return L;
		}
		else
		{	if(f->status==defined)
			{
				F->data->status = defined;
			}
			else
			{	fprintf(stderr,"Multiple declaration of fn \"%s\"\n",F->data->identifier);
				return L;
			}
		}

	}
	return L;
}

// Creates a list of all the functions declared or defined in the file "file_name"
Function_list get_functions(char *file_name)
{
	int i;
	bool bool_data_type=false;
	int bracket_cnt=0;
	int arg_cnt,para_cnt,line_no=1;
	char identifier[MAX_SIZE];
	bool end_of_statement;
	char arg[ARG_NUM][ARG_SIZE], *data_type , argument[ARG_SIZE], return_type[MAX_SIZE];
	Function_list L=NULL;
	Function *F;
	bool valid;
	char str1[MAX_SIZE],str2[MAX_SIZE],str3[MAX_SIZE],*str;
	str=(char*)malloc(MAX_SIZE);
	data_type=(char*)malloc(MAX_SIZE);
	FILE *ifp;
	ifp=fopen(file_name,"r");
	if(ifp==NULL)
	{	fprintf(stderr,"File error : Unable to open file \"%s\"\n",file_name);
		return ;
	}
	fgets(str2,MAX_SIZE-1,ifp);
	fgets(str3,MAX_SIZE-1,ifp);
	if(!strcmp(str1,"\n"))
		line_no++;
	if(!strcmp(str2,"\n"))
		line_no++;

	while(!feof(ifp))
	{

		if(bool_data_type==false)
		{
			strcpy(str1,str2);
			if(isDataType(str2))
				bool_data_type=true;
		}
		else
		{	if(isDataType(str2))
				strcat(str1,str2);
			else
			{
				strcpy(str1,str2);
				bool_data_type=false;
			}
		}
		strcpy(str2,str3);
		fgets(str3,MAX_SIZE-1,ifp);
		if(!strcmp(str3,"\n"))
			line_no++;
		if(!strcmp(str3,"(\n"))
			if(!isKeyword(str2) && bool_data_type )
			{	arg_cnt=0;
				F=create_function();
				strcpy(F->return_type,str1);
				strcpy(F->identifier,str2);
				strcpy(data_type,"");
				strcpy(str,"");
				strcpy(data_type,"");
				strcpy(identifier,str2);
				strcpy(return_type,str1);
				fgets(str,MAX_SIZE-1,ifp);
				end_of_statement=false;
				while( !feof(ifp) && !end_of_statement )
				{
					if(!strcmp(str,"\n"))
						line_no++;
					else
					if(!strcmp(str,"[\n"))
						bracket_cnt++;
					else
					if(!strcmp(str,"]\n"))
						bracket_cnt--;
					else
					if(isNumber(str))
					{
						if(bracket_cnt!=1 || !strcmp(argument,""))
						{
							str[strlen(str)-1]='\0';
							fprintf(stderr,"Invalid fn declaration/definition in line %d : Invalid argument \"%s\"\n",line_no,str);
							break;
						}
					}
					else
					if(isDataType(str))
					{	str[ strlen(str)-1 ]=' ';
						if(!strcmp(data_type,""))
							arg_cnt++;
						strcat(data_type,str);
						strcpy(argument,"");
					}
					else
					if( !strcmp(str,"*\n") )
					{	if(!strcmp(data_type,"") )
						{	fprintf(stderr,"Invalid function declaration in line %d : indiscriminate use of *\n",line_no);
							break;
						}
						strcat(data_type,str);
					}
					else
					if(isIdentifier(str))
					{	if(!strcmp(data_type,""))
						{	fprintf(stderr,"Invalid function declaration in line %d : expected data type before argument\n",line_no);
							break;
						}
						str[ strlen(str)-1 ]='\0';
						data_type[ strlen(data_type)-1 ]='\0';
						strcpy(F->type[arg_cnt-1],data_type);
						strcpy(data_type,"");
						fgets(str,MAX_SIZE-1,ifp);
						while(!strcmp(str,"\n"))
							fgets(str,MAX_SIZE-1,ifp);
						if(!strcmp(str,")\n"))
							continue;
						else
						{	label:
							if(!strcmp(str,"[\n"))
							{
								fgets(str,MAX_SIZE-1,ifp);
								if(isNumber(str))
									fgets(str,MAX_SIZE-1,ifp);
								if(strcmp(str,"]\n"))
								{	fprintf(stderr,"Expected ] in line %d\n",line_no);
									break;
								}
								fgets(str,MAX_SIZE-1,ifp);
								goto label;
							}
							else
							{	if(!strcmp(str,"\n"))
								{	fgets(str,MAX_SIZE-1,ifp);
									goto label;
								}
								if(strcmp(str,",\n") && strcmp(str,")\n") )
								{	fprintf(stderr,"Expected , or ) in line %d\n",line_no);
									break;
								}
								if(!strcmp(str,")\n"))
									continue;
							}
						}
					}
					else
					if(!strcmp(str,",\n") || !strcmp(str,")\n") )
					{
						str[ strlen(str)-1 ]='\0';
						if(strcmp(data_type,""))
						{
							data_type[ strlen(data_type)-1 ]='\0';
							strcpy(F->type[arg_cnt-1],data_type);
						}
						strcpy(data_type,"");
						if( !strcmp(str,")") )
						{	end_of_statement=true;
							fgets(str,MAX_SIZE-1,ifp);
							while(!feof(ifp) && ( !strcmp(str,"\n") || !strcmp(str,"\t\n") ) )
								fgets(str,MAX_SIZE-1,ifp);
							if(!strcmp(str,";\n"))
							{	F->status=declared;
								F->arg_cnt=arg_cnt;
								L=add_function(L,F);
							}
							else
							if(!strcmp(str,"{\n"))
							{	F->status=defined;
								F->arg_cnt=arg_cnt;
								L=add_function(L,F);
							}
							break;
						}
						else
						{
							fgets(str,MAX_SIZE-1,ifp);
							continue;
						}
					}
					else
					{
						fprintf(stderr,"Invalid identifier \"%s\" in line %d\n",str,line_no);
						break;
					}
					fgets(str,MAX_SIZE-1,ifp);
				}
				
			}

	}
	fclose(ifp);
	return L;
}

// Checks whether the string 'str' is a keyword
bool isKeyword(char *str)
{
	char keyword[MAX_SIZE];
	FILE *ifp=fopen(keywords_file,"r");
	if(ifp==NULL)
	{	fprintf(stderr,"Unable to open file \"%s\"\n",keywords_file);
		return false;
	}
	fgets(keyword,MAX_SIZE-1,ifp);
	while(!feof(ifp))
	{	if(!strcmp(str,keyword))
		{	fclose(ifp);
			return true;
		}
		keyword[ strlen(keyword)-1 ]='\0';
		if(!strcmp(str,keyword))
		{	fclose(ifp);
			return true;
		}
		fgets(keyword,MAX_SIZE-1,ifp);
	}
	fclose(ifp);
	return false;
}

// Delete function
void delete_function(Function *F)
{
	if(F!=NULL)
		free(F);
}

// Checks whether the string 'str' is a datatype which can be used for defining variables
bool isDataType(char *str)
{
	char datatype[MAX_SIZE];
	FILE *ifp=fopen(datatypes_file,"r");
	if(ifp==NULL)
	{	fprintf(stderr,"Unable to open file \"%s\"\n",datatypes_file);
		return false;
	}
	fgets(datatype,MAX_SIZE-1,ifp);
	while(!feof(ifp))
	{	if(!strcmp(str,datatype))
		{	fclose(ifp);
			return true;
		}
		datatype[ strlen(datatype)-1 ]='\0';
		if(!strcmp(str,datatype))
		{	fclose(ifp);
			return true;
		}
		fgets(datatype,MAX_SIZE-1,ifp);
	}
	fclose(ifp);
	return false;
}
		
// Checks whether the string "str" is a valid identifier		
bool isIdentifier(char *str)
{
	if(isDataType(str))
		return false;
	if(!isalpha(str[0]) && str[0]!='_')
		return false;
	while(*++str)
		if( (*str)!='\n' )
		if(!isalnum(*str) && (*str)!='_')
			return false;
	return true;
}

// In file containing tokens, make each string constant a single token
void string_constants(char *file_name )
{
	char str[MAX_SIZE],prev_str[MAX_SIZE]="";
    FILE *ifp,*ofp;
	bool ignore=false;
    ifp=fopen(file_name,"r");
    if(ifp==NULL)
    {   fprintf(stderr,"Unable to open file \"%s\"\n",file_name);
        exit(1);
    }
    ofp=fopen(temp_file,"w");
    if(ofp==NULL)
    {   fprintf(stderr,"Unable to open file \"%s\"\n",Comments_file);
        exit(1);
    }
	fgets(str,MAX_SIZE-1,ifp);
	while(!feof(ifp))
	{
			str[ strlen(str)-1 ]='\0';
		if( ignore==false && !strcmp(str,"\"") )
			ignore=true;
		else
		if( ignore==true && ( strcmp(prev_str,"\\") && !strcmp(str,"\"" ) ) )
			ignore=false;
		fprintf(ofp,"%s%c",str,( ignore ? ' ' : '\n' ) );
		strcpy(prev_str,str);
		fgets(str,MAX_SIZE-1,ifp);
	}
	rename(temp_file,file_name);
	fclose(ifp);
	fclose(ofp);
	return;
}

// In file containing tokens, make each character constant a single token
void character_constants(char *file_name )
{
	char str[MAX_SIZE],prev_str[MAX_SIZE]="";
    FILE *ifp,*ofp;
	bool ignore=false;
    ifp=fopen(file_name,"r");
    if(ifp==NULL)
    {   fprintf(stderr,"Unable to open file \"%s\"\n",file_name);
        exit(1);
    }
    ofp=fopen(temp_file,"w");
    if(ofp==NULL)
    {   fprintf(stderr,"Unable to open file \"%s\"\n",Comments_file);
        exit(1);
    }
	fgets(str,MAX_SIZE-1,ifp);
	while(!feof(ifp))
	{
		str[ strlen(str)-1 ]='\0';
		if( ignore==false && !strcmp(str,"\'") )
			ignore=true;
		else
		if( ignore==true && ( strcmp(prev_str,"\\") && !strcmp(str,"\'" ) ) )
			ignore=false;
		if(ignore)
			fprintf(ofp,"%s",str);
		else
			fprintf(ofp,"%s%c",str,'\n');
		strcpy(prev_str,str);
		fgets(str,MAX_SIZE-1,ifp);
	}
	rename(temp_file,file_name);
	fclose(ifp);
	fclose(ofp);
	return;
}

bool isNumber(char *str)
{
	int i;
	for(i=0;i<strlen(str)-1;i++)
		if(str[i]<'0' || str[i]>'9')
			return false;
	if(str[i]!='\n' && ( str[i]<'0' || str[i]>'9')  )
		return false;
	return true;
}

// Stack functions
Stack	CreateStack()
{
	return NULL;
}

Stack Push(Stack S,char str[])
{
	Node* t=calloc(1,sizeof(Node));
	if(t==NULL)
	{
		fprintf(stderr,"No space for stack\n");
		exit(1);
	}
	strcpy(t->str,str);
	t->next=S;
	S=t;
	return S;
}

Stack Pop(Stack S)
{
	if(S==NULL)
		return S;
	Node *t=S;
	S=S->next;
	free(t);
	return S;
}

bool Find(Stack S,char str[MAX_SIZE])
{
	Node* t=S;
	while(t!=NULL)
	{
		if(!strcmp(t->str,str))
			return true;
		else
			t=t->next;
	}
	return false;
}


Stack	DeleteStack(Stack S)
{
	while(S!=NULL)
		S=Pop(S);
	return NULL;
}

// Displays overall function hierarchy in the C program
void function_hierarchy(Function_list L)
{
	Stack S=CreateStack();
	Function_element* E=L;
	while(E!=NULL && strcmp(E->data->identifier,"main\n"))
		E=E->next;
	if(E==NULL)
	{	fprintf(stderr,"Expected definition of function \"main\"\n");
		return;
	}
	S=Push(S,"main\n");
	printf("main\n");
	S=Pop(S);
	print_hierarchy(E,L,&S);
	S=DeleteStack(S);
	return;
}

// Displays function hierarchy of calls in the function *E
void print_hierarchy(Function_element *E,Function_list L,Stack *S)
{
	int i,j;
	Function_element *F;
	static int depth=0;
	depth++;
	for(i=0;i<E->data->called_fn_count;i++)
	{	for(j=0;j<depth;j++)
			printf("\t");
		printf("%s",E->data->called_fn[i]);
		F=L;
		while(strcmp(F->data->identifier,E->data->called_fn[i]))
			F=F->next;
		
		if(F->data->status!=defined)
		{	fprintf(stderr,"Undefined reference to function %s\n",F->data->identifier);
			continue;
		}
		
		if(!Find(*S,F->data->identifier))
		{	(*S)=Push(*S,F->data->identifier);
			print_hierarchy(F,L,S);
			(*S)=Pop(*S);
		}
	}
	depth--;
	return;
}

