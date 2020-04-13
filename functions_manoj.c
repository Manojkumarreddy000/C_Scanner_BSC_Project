// this file  contains all t:he functions created by Manoj for scanning and parsing

#include"header.h"




// this function will take the file as input and separate it into individual tokens
// on 25-01-16
void token(char *filename)
{
 char ch,ch2;

 FILE *inputfp,		 // this pointer points to the file from  which we are taking the input
 		*outputfp;  // this pointer points to the file in which we are putting the tokens

 // trying to link to the pointer
 outputfp=fopen("temp","w");
 inputfp=fopen(filename,"r");


 // checking if we can link to the files
 if(outputfp==NULL || inputfp==NULL)
  {
   fprintf(stderr,"%s","not able to create or open the file in function token");
   exit(1);
  }

 ch=fgetc(inputfp);


 // 
 while(!feof(inputfp))
  { 
   ch2=fgetc(inputfp);
   
   if(isalnum(ch) || ch=='_')
    fputc(ch,outputfp);
   else
    if(ch==' ')//  && ( isalnum(ch2) || ch2=='_' ) )
     fputc('\n',outputfp);
    else
     if(ch=='\n')
      {
       fputc('\n',outputfp); 
      }
     else
      {
       if(!isalnum(ch2) && ch!='_')
        {
         fputc('\n',outputfp); 
         fputc(ch,outputfp); 
        }
       else
        {
         fputc('\n',outputfp); 
         fputc(ch,outputfp); 
         fputc('\n',outputfp); 
        }
  
      // fputc('\n',outputfp); 
      }
 
   ch=ch2;

  }


 fclose(outputfp);  
 fclose(inputfp);  

 rename("temp",filename);

 return;
}





// this function is to remove the #define line and #include
// on 27-1
void remove_define(char *filename)
{
 char ch,str[30];
 int i=0,check,check2=1;
 FILE *inputfp,*outputfp;

 outputfp=fopen("temp","w");
 inputfp=fopen(filename,"r");

 if(outputfp==NULL || inputfp==NULL)
  {
   fprintf(stderr,"%s","not able to create or open the file in function remove_define");
   exit(1);
  }

 ch=fgetc(inputfp);
 
 
 while(!feof(inputfp))
  { 
   if(ch=='#')
    {
     ch=fgetc(inputfp);
     
     i=0;
     while(!feof(inputfp))
      {
       str[i++]=ch; 
       str[i]='\0';
    //   printf("\n\"%s\"\n",str);
        if(ch==' ' || ch=='\"' || ch=='<')
        {
         str[i-1]='\0';
         check=strcmp("define",str) && strcmp("include",str) ; 
  //        printf("\n%d",check);
         if(check==0)
          {
           ch=fgetc(inputfp);
           while(ch!='\n' && !feof(inputfp))
            ch=fgetc(inputfp);
           break;
          }
         else
          {
           fputc('#',outputfp);
           fprintf(outputfp,"%s",str);
           ch=fgetc(inputfp);
           break;
          }
       
        }
       ch=fgetc(inputfp);
      }
    }
   fputc(ch,outputfp);
   ch=fgetc(inputfp);
  
  }
   fclose(outputfp);
   fclose(inputfp);

  rename("temp",filename);
  return;
}





// this function will find the Varibles used in the program

void Variables(char* filename)
{
	FILE *ipf,*opf;
	int i,flag=1,status,line;
	char strl[100]="",str[50];
	char in_file[MAX_SIZE];

	strcpy(in_file,filename);
	in_file[ strlen(in_file)-2 ]='\0';
	int sc,qf,bracess,dt;
	char stcl[][10]={"auto\n","extern\n","static\n","register\n"};
	char Q_ers[][10]={"long\n","short\n","signed\n","unsigned\n"};
	char data[][10]={"int\n","char\n","float\n","double\n","long\n"};
	
	strcat(in_file,"_");
	strcat(in_file,variables_file);
	ipf=fopen(filename,"r");
	opf=fopen(in_file,"w");
	

	if(ipf==NULL || opf== NULL)
	{
		fprintf(stderr,"not able to create the file %s","temp");
		exit(1);
	}
	
	fgets(str,49,ipf);

	status=1;
	line=1;
	rewind(ipf);
	while(!feof(ipf))
	{
		if(!strcmp(str,"\n") )
			line++;


		if(!strcmp(str,"(\n") )
			while(!feof(ipf) && strcmp(str,")\n") )
			{
				fgets(str,49,ipf);
				if(!strcmp(str,"\n") )
					line++;
				status=1;
			}
			


		switch(status)
		{
			case 1:
			    if(strl[0]!='\0')
					strl[0]='\0';
					
					flag=1;
					sc=0;
					qf=2;

					for(i=0;i<4;i++)
						if(!strcmp(str,stcl[i]) )
						{
							status=2;

							sc=i;
							break;
						}
						
					for(i=0;i<4;i++)
						if(!strcmp(str,Q_ers[i]) )
						{
								status=7;
								
								qf=i;
//							printf("Qualifier found %d\n",qf);
								break;
						}
					for(i=0;i<5;i++)
						if(!strcmp(str,data[i]) )
						{
							status=3;
							strcat(strl,"\n");
							strcat(strl," ");
							strcat(strl,stcl[sc]);
							strcat(strl," ");
							strcat(strl,Q_ers[qf]);
							strcat(strl," ");
							strcat(strl,data[i]);
							strcat(strl," ");
							dt=i;
//							printf("Data type with sc : %d, qf %d\n",sc,qf);
							break;
						}
						
				break;
			case 2:
					for(i=0;i<4;i++)
						if(!strcmp(str,Q_ers[i]) )
						{
							status=7;

								qf=i;
//							printf("Qualifier found\n");
								break;
						}
					if(i!=4)
						break;
					for(i=0;i<5;i++)
						if(!strcmp(str,data[i]))
						{
							
							 status=3;
							strcat(strl,"\n");
							strcat(strl," ");
							strcat(strl,stcl[sc]);
							strcat(strl," ");
							strcat(strl,Q_ers[qf]);
							strcat(strl," ");
							strcat(strl,data[i]);
							strcat(strl," ");
							 dt=i;
//							printf("Data type with sc : %d, qf %d\n",sc,qf);
							 break;
						}

					if(i==5)
					{
							fprintf(stderr,"Expected data type before %s\n",str);
							status=1;
							break;
					}
					break;

			case 7:
					for(i=0;i<5;i++)
						if(!strcmp(str,data[i]) )
						{		
							status=3;
							strcat(strl,"\n");
							strcat(strl," ");
							strcat(strl,stcl[sc]);
							strcat(strl," ");
							strcat(strl,Q_ers[qf]);
							strcat(strl," ");
							strcat(strl,data[i]);
							strcat(strl," ");
							dt=i;
//							printf("Data type with sc : %d, qf %d\n",sc,qf);
							break;
						}
					if(i==5)
					{
							str[strlen(str)-1]='\0';
							fprintf(stderr,"Expected data type before %s\n",str);
							status=1;
							break;
					}

						break;
			case 3:
					if(isIdentifier(str) )
					{
						status=5;
						if(!flag)
							strcat(strl," ");
						 str[strlen(str)-1]='\0';
							strcat(strl,str);

						if(flag==1)
							flag=0;
						break;
					}
					if(!strcmp(str,"*\n") )
					{
							flag=1;
					 str[strlen(str)-1]='\0';
						strcat(strl,str);
						
						break;
					}
					break;
			case 5:
					if(!strcmp(str,"[\n") )
					{
						status=6;
					 str[strlen(str)-1]='\0';
						strcat(strl,str);

						break;
					}
					else
					if(!strcmp(str,",\n") )
					{
						status=3;
					 str[strlen(str)-1]='\0';
						strcat(strl,str);
						break;
					}
					else
					if(!strcmp(str,";\n") )
					{
					 status=1;
					 str[strlen(str)-1]='\0';
						strcat(strl,str);
						fprintf(opf,"%s\n",strl);

					 break;
					}
					else
					if(!strcmp(str,"=\n") )
					{
							status=9;
					 str[strlen(str)-1]='\0';
							strcat(strl,str);
						break;
					}
					else
					{
						str[strlen(str)-1]='\0';
						fprintf(stderr,"Expected ';' before \"%s\"\n",str);
					}

					break;

			case 6:
					if(!strcmp(str,"]\n") )
					{

						status=5;
					 str[strlen(str)-1]='\0';
						strcat(strl,str);
						break;
					}
					if(isNumber(str) )
					{
					 str[strlen(str)-1]='\0';
						strcat(strl,str);
					}
					break;
			case 9:
					 str[strlen(str)-1]='\0';
					strcat(strl,str);
					status=8;
					break;
			case 8:
					if(!strcmp(str,",\n") )
					{
						status=3;
					 str[strlen(str)-1]='\0';
						strcat(strl,str);
						break;
					}
					else
					if(!strcmp(str,";\n") )
					{
					 status=1;
					 str[strlen(str)-1]='\0';
						strcat(strl,str);
						fprintf(opf,"%s\n",strl);

					 break;
					}
					else
					 str[strlen(str)-1]='\0';
					strcat(strl,str);
					break;

			
	
		}



		fgets(str,49,ipf);
		

	}

	fclose(ipf);
	fclose(opf);


	return;
}



// this function will find the for block's  in theb program

void print_for(char *file_name,FILE **ifp)
{	
	int bracket_cnt=0,semicolon_cnt=0;
	char str[MAX_SIZE];
	FILE *ofp=fopen(file_name,"w");
	if(ofp==NULL)
	{	fprintf(stderr,"Error opening file \"%s\"\n",file_name);
		return;
	}
	fseek(*ifp,0,SEEK_SET);
	fgets(str,MAX_SIZE-1,*ifp);
	str[strlen(str)-1]='\0';

	while(!feof(*ifp))
	{	if(!strcmp(str,"for"))  // || !strcmp(str,"if\n") )
		{	
			fgets(str,MAX_SIZE-1,*ifp);
			str[strlen(str)-1]='\0';

			if( !strcmp(str,"("))      // || !strcmp("(\n",str) )
			{	bracket_cnt=1;

				fputs("for",ofp);
				semicolon_cnt=0;


				while(!feof(*ifp) && bracket_cnt!=0 )
				{	
					if(strcmp(str,"\n") && str[ strlen(str)-1 ]=='\n' )
						str[ strlen(str)-1 ]=' ';
					fputs(str,ofp);

					fgets(str,MAX_SIZE-1,*ifp);
					str[strlen(str)-1]='\0';


							if(!strcmp(str,"("))
								bracket_cnt++;

							if(!strcmp(str,")"))
								bracket_cnt--;

							if(!strcmp(str,";"))
								semicolon_cnt++;

							if(semicolon_cnt>2)
							{
								fprintf(stderr,"Wrong syntax in for statement (Too many expressions)\n");
								return;
							}



				}

				if(bracket_cnt==0)
				{
					if(semicolon_cnt<2)
					{
						fprintf(stderr,"Too less expressions in for statement\n");
						return;
					}

					fputs(str,ofp);
					print_statement(ifp,&ofp);
				}
				else
				{
					fprintf(stderr,"Expected ) before the end of input\n");
					return;
				}





			}
			else
			{	fgets(str,MAX_SIZE-1,*ifp);
				fprintf(stderr,"Invalid use of for loop\n");
		 	}
		}
		else
		{
			fgets(str,MAX_SIZE-1,*ifp);
			str[strlen(str)-1]='\0';
		}
//	    printf("\"%s\" obtained\n",str);
	}
return;
}


// this function will find the while blocks in the program

void print_while(char *file_name,FILE **ifp)
{	
	int bracket_cnt;
	char str[MAX_SIZE];
	FILE *ofp=fopen(file_name,"w");
	if(ofp==NULL)
	{	fprintf(stderr,"Error opening file \"%s\"\n",file_name);
		return;
	}
	fseek(*ifp,0,SEEK_SET);
	fgets(str,MAX_SIZE-1,*ifp);
	str[strlen(str)-1]='\0';
//	printf("\"%s\" obtained\n",str);
	while(!feof(*ifp))
	{	if(!strcmp(str,"while")) // || !strcmp(str,"if\n") )
		{	
			fgets(str,MAX_SIZE-1,*ifp);
			str[strlen(str)-1]='\0';

			if( !strcmp(str,"("))  // || !strcmp("(\n",str) )
			{	
				fputs("while",ofp);
				bracket_cnt=1;

				while(!feof(*ifp) && bracket_cnt!=0 )
				{		
					if(strcmp(str,"\n") && str[ strlen(str)-1 ]=='\n' )
						str[ strlen(str)-1 ]=' ';
					fputs(str,ofp);

					fgets(str,MAX_SIZE-1,*ifp);
					str[strlen(str)-1]='\0';


							if(!strcmp(str,"("))
								bracket_cnt++;

							if(!strcmp(str,")"))
								bracket_cnt--;
				}

				if(bracket_cnt==0)
				{
					fputs(str,ofp);
					print_statement(ifp,&ofp);
				}
				else
				{
					fprintf(stderr,"\nBracket at the end of the while statement is missing(not closed)\n");
					return;
				}

			}
			else
			{	
				fgets(str,MAX_SIZE-1,*ifp);
				fprintf(stderr,"Invalid use of while loop\n");
		 	}
		}
		else
		{
			fgets(str,MAX_SIZE-1,*ifp);
			str[strlen(str)-1]='\0';
		}
	}




return;				
}

 int if_function(char a[],Function_list F1)
{

	Function *f;
	if(!isIdentifier(a))
		return 0;
	
	while(F1!=NULL)
	{
		f=F1->data;
		if(!strcmp(a,f->identifier) )
			return 1;
        F1=F1->next;
	}
	if(F1==NULL)
		return 0;
}




//this function will check if finction is defined correctly

 int if_samedata(Function_list F1,char a[],int data_cnt)
{
	Function *f;
	
	while(F1!=NULL)
	{
		f=F1->data;
		if(!strcmp(a,f->identifier) )
			if(f->arg_cnt==data_cnt)
				return 1;
			else
				return 0;
		
		F1=F1->next;
	}
	if(F1==NULL)
		return 0;
}




// this function will find all the other functions defined in a perticular function

void find_hierarchy(Function_list F1,char *filename)
{
	Function* f;
	Function_list F2=F1;
	int status=1,data_count,bracket_cnt,first_time=0,sq_bracket,pos;
	FILE *ptr,*iptr,*temp;
	char str[50],cpy[50];

	ptr=fopen(filename,"r");

	if(ptr==NULL)
	{
			fprintf(stderr,"\n\nUnable to open file %s\n",filename);
			return;
	}

	while(!feof(ptr))
	{
		fgets(str,49,ptr);

		if(status==1)
		{
			data_count=0;
			bracket_cnt=0;
		}

		switch(status)
		{
			case 1:
					if(if_function(str,F1) )
					{
						status=2;
						strcpy(cpy,str);
						first_time=0;
						F2=F1;
						
						while(strcmp(F2->data->identifier,str) && F2!=NULL)
							F2=F2->next;
						F2->data->called_fn_count=0;
						
					}
					break;
			case 2:
					while(!strcmp(str,"\\") || !strcmp(str,"\n") || !strcmp(str,"\t"))
					{
						fgets(str,49,iptr);
						if(strcmp(str,"\n"))
 							str[strlen(str)-1]='\0';
					}

					if(!strcmp(str,"(\n") )
						status=3;
					else
					{
						str[strlen(str)-1]='\0';
						fprintf(stderr,"Invalid use of identifier\"%s\"",cpy);
						status=1;
					}
					break;

			case 3:
					if(!strcmp(str,")\n") )
						status=4;
					else
						if(isDataType(str) )
						{
							data_count++;
							status=7;
						}
						else
						{
							str[strlen(str)-1]='\0';
							fprintf(stderr,"Invalid identifier \"%s\" in definition of function %s",str,cpy);
							status=1;
						}
						
						break;
			case 7:
					if(isIdentifier(str))
					{
							sq_bracket=0;
							status=8;
					}

							break;
			case 8:
					if(sq_bracket!=0 && isNumber(str) )
			          break;					  ;
					if(!strcmp(str,"[\n"))
							sq_bracket++;
					else
						if(!strcmp(str,"]\n"))
								sq_bracket--;
						else
							if(!strcmp(str,",\n"))
							{
									if(sq_bracket<0)
									{
											fprintf(stderr,"Unexpected ]\n");
											status=1;
									}
									else
									if(sq_bracket<0)
									{
											fprintf(stderr,"Expected ]\n");
											status=1;
									}
									else
									status=3;
							}
							else
								if(!strcmp(str,")\n") )
									status=4;
								else
									if(!strcmp(str,"\n") || !strcmp(str,"\t\n") )
									{}
									else
									{
											fprintf(stderr,"Error in variable declaration\n");
											status=1;
									}



					break;

									
									


			case 4:
					if(if_samedata(F1,cpy,data_count) );
					else
					{
						cpy[ strlen(cpy)-1 ]='\0';
						fprintf(stderr,"Mismatch of number of arguments of function \"%s\"",cpy);
						status=1;
						break;
					}

					
					while(!strcmp(str,"\\\n") || !strcmp(str,"\n") || !strcmp(str,"\t\n"))
					{
						fgets(str,49,ptr);
					}

					if(!strcmp(str,";\n") )
						status=1;
					else
						if(!strcmp(str,"{\n") )
						{
								bracket_cnt=1;
								status=6;
						}
						else
						{
							fprintf(stderr,"Expected \';\' after declaration\"%s\"",str);
							status=1;
						}

						break;

					
			case 6:
					
					if(!strcmp(str,"{\n") )
						bracket_cnt++;
					if(!strcmp(str,"}\n") )
						bracket_cnt--;
                    if(bracket_cnt==0)
					{
						status=1;
//						fputs("}",stdout);
					}

					if(if_function(str,F1) )
					{
//						fprintf(temp,"%s : %d : %d",str,if_function(str,F1),first_time);
						pos=F2->data->called_fn_count;
						strcpy((F2->data->called_fn)[pos],str);
						F2->data->called_fn_count++;
						/*
						if(first_time==0)
						{
							fputs("\n\n\n",stdout);
							fputs(cpy,stdout);
							fputs("{",stdout);

							first_time=1;
						}
							fputs("\n",stdout);
							fputs(str,stdout);
						*/
					}
					break;
		}
	}


					
	if(bracket_cnt!=0)
	 fprintf(stderr,"Expected bracket at the end of the input\n");
	
	
	
	

	fclose(ptr);
//	fclose(iptr);
//	fclose(temp);
	return;
}



