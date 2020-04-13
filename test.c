#include"header.h"

int scanner_parser(char argv[] )
{
	int i;
	char return_type[MAX_SIZE],identifier[MAX_SIZE];
    Macro_list *List;
    Macro_element *p;
	Function_list L=NULL;
	Function_element *E;
	char in_file[MAX_SIZE],
			file[MAX_SIZE];

    FILE *infp,*opfp;
    int j;
    int len;
    char ch,str[MAX_SIZE];
    len=strlen(argv);
    if(argv[len-1]!='c' || argv[len-2]!='.')
    {
        fprintf(stderr,"\nInvalid argument\nExpected a .c file as argument\n");
        return 1;
    }
	strcpy(in_file,argv);
	in_file[len-2]='\0';
/*	for(i=0;i<strlen(in_file);i++)
		if(in_file[i]=='.')
		{	in_file[i]='\0';
			break;
		}

	for(i=strlen(in_file);i>0;i--)
		if(in_file[i]=='.')
		{	in_file[i]='\0';
			break;
		}
*/
    strcpy(str,argv);
    argv[len-1]='o';
    infp=fopen(str,"r");
	if(infp==NULL)
	{	fprintf(stderr,"Unable to open file \"%s\"\n",str);
		return 1;
	}
    opfp=fopen(argv,"w");
    ch=fgetc(infp);

    while(!feof(infp))
    {
        fputc(ch,opfp);
        ch=fgetc(infp);
    }  
 
    fclose(infp);

    fclose(opfp);

	remove_comments(argv);

	//Finding the macro definitions in the input code 
    List=get_macros(argv);


	// Remove the macro definitions in the input file
    remove_define(argv);

	// Substitute the macro definitions
    substitute_all_macros(argv,List);

	// Scanner
    token(argv);
 	string_constants(argv);
 	character_constants(argv);

	// Identifies the variables declared
   Variables(argv);

	// If construct
  	infp=fopen(argv,"r");
	if(infp==NULL)
	{	fprintf(stderr,"Error opening file \"%s\"\n");
		return 1;
	}
	strcpy(file,in_file);
	strcat(file,"_");
	strcat(file,if_file);
    print_if(file,&infp);
	fclose(infp);

  	infp=fopen(argv,"r");
	if(infp==NULL)
	{	fprintf(stderr,"Error opening file \"%s\"\n");
		return 1;
	}

	strcpy(file,in_file);
	strcat(file,"_");
	strcat(file,for_file);
    print_for(file,&infp);
	fclose(infp);

  	infp=fopen(argv,"r");
	if(infp==NULL)
	{	fprintf(stderr,"Error opening file \"%s\"\n");
		return 1;
	}

	strcpy(file,in_file);
	strcat(file,"_");
	strcat(file,while_file);
    print_while(file,&infp);
	fclose(infp);

    L=get_functions(argv);

	find_hierarchy(L,argv);

	strcpy(file,in_file);
	strcat(file,"_");
	strcat(file,functions_file);
	opfp=fopen(file,"w");
	if(opfp==NULL)
	{	fprintf(stderr,"Unable to open file \"%s\"\n",functions_file);
		return 1;
	}


	for( E=L; E!=NULL; E=E->next )
	{
		strcpy(return_type,E->data->return_type);
		if(return_type[ strlen(return_type)-1 ]=='\n')
			return_type[ strlen(return_type)-1 ]='\0';

		strcpy(identifier,E->data->identifier);

		if(identifier[ strlen(identifier)-1 ]=='\n')
			identifier[ strlen(identifier)-1 ]='\0';

		fprintf(opfp,"\n%s %s",return_type,identifier);
		fprintf(opfp,"(");

		if(E->data->arg_cnt)
		{	
			for(i=0;i<E->data->arg_cnt-1;i++)
				fprintf(opfp,"%s,",E->data->type[i]);

			fprintf(opfp,"%s",E->data->type[i]);
		}

		fprintf(opfp,")\n");

		if(E->data->called_fn_count!=0)
		{	fprintf(opfp,"Functions called by %s",E->data->identifier);
			for(i=0;i<E->data->called_fn_count;i++)
				fprintf(opfp,"\t%s",E->data->called_fn[i]);
		}

		if(E->data->status==declared)
			fprintf(opfp,"Declared\n\n");
		
		if(E->data->status==defined)
			fprintf(opfp,"Defined\n\n");
		
		fprintf(opfp,"\n");
	}

	printf("\n\n\nFunction hierarchy of %s.c\n",in_file);
	// for finding the function hierarchy
	function_hierarchy(L);

    return 0;
}





// starting fo the main function
int main(int argc,char *argv[])
{
	int i;

    if(argc==1)
    {   printf(" Usage : test  filename [ filename2 [ filename3 .. ] ] \n");
        return 1;
    }
			
	for(i=1;i<argc;i++)
		scanner_parser(argv[i]);
	return 0;
}
