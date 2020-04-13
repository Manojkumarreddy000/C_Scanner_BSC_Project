#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>
#include<stdbool.h>
#include<ctype.h>

#define temp_file			"temp.dat"
#define comments_file		"Single_line_comments.txt"
#define header_address 		"/usr/include/"
#define Comments_file 		"Multi_line_comments.txt"
#define Macros_file 		"Macros.txt"
#define if_file 			"If.txt"
#define for_file 			"For.txt"
#define while_file 			"While.txt"
#define variables_file 		"Variables.txt"
#define functions_file		"Functions.txt"
#define keywords_file 		"keywords.dat"
#define datatypes_file 		"datatypes.dat"


#define MAX_SIZE 			300		// Maximum size of a token
#define ARG_NUM 			5		// Maximum number of arguments 
#define ARG_SIZE 			10		// Maximum size of an argument of a macro or a function
#define MAX_NUM 			30		// Maximum number of functions which can be called 


enum Status { unknown, declared, defined }  ;

// Structure definitions for macros

typedef struct macro
{
    char identifier[MAX_SIZE];
    char arg[ARG_NUM][ARG_SIZE];
    int arg_cnt;
    char replacing_string[MAX_SIZE];
    int line_no;
} Macro;

typedef struct macro_list
{
    struct macro* data;
    struct macro_list *next;
}   Macro_list;

typedef Macro_list Macro_element;

// Structure definitions for functions

typedef struct function
{
	char identifier[MAX_SIZE];
	int arg_cnt;
	char type[ARG_NUM][MAX_SIZE];
	char return_type[MAX_SIZE];
	char called_fn[MAX_NUM][MAX_SIZE];
	int called_fn_count;
	enum Status status;
} 	Function;

typedef struct function_element
{
	struct function* data;
	struct function_element *next;
}	Function_element;

typedef Function_element* Function_list;

// Stack of strings
typedef struct node
{
	char str[MAX_SIZE];
	struct node* next;
}	Node;

typedef Node* Stack;

// Stack fns
Stack	CreateStack();
Stack	Push(Stack S,char*);
Stack	Pop(Stack S);
bool	Find(Stack,char*);
Stack	DeleteStack(Stack);

// Function declarations of preprocessor
void		remove_single_line_comments(char *);
void		remove_multiline_comments(char *filename);
void 		remove_define(char *filename);
void 		string_constants(char *filename);
Macro_list* get_macros(char *file_name);
Macro_list* add_macro(Macro_list *head, Macro *to_be_inserted);
Macro_list* create_macro_list();
void 		substitute_macro(char *file_name,Macro *M);

void 		replace_substr(char *,char *,char *);
void		substitute_all_macros(char *file_name,Macro_list *L);
void 		remove_define(char *filename);

// Scanner
void		token(char *filename);
void 		string_constants(char *file_name );
void		character_constants(char *file_name );

// Functions for identifying various blocks
void		print_while(char *file_name,FILE **ifp);
void 		print_statement(FILE **ifp, FILE **ofp);
void 		print_if(char *file_name,FILE **ifp);
void 		print_for(char *file_name,FILE **ifp);

// Function for identifying the variables
void 		Variables(char* filename);

// Function hierarchy
Function* 		create_function();
void			delete_function(Function*);
Function_list	get_functions(char *file_name);
void     	    build_hierarchy(Function_list );
Function_list	add_function(Function_list,Function*);
void			print_function_hierarchy(Function_list);
void			print_hierarchy(Function_element*,Function_list,Stack*);


 int if_function(char a[],Function_list F1);
void find_hierarchy(Function_list F1,char *filename);
 int if_samedata(Function_list F1,char a[],int data_cnt);
// Misc fns
bool			isKeyword(char*);
bool			isDataType(char*);
bool 			isIdentifier(char*);
bool			 isNumber(char*);
