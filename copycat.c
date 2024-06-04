/*
 * Reverse-engineer of [cat] function
 * author: Quoc Bui
 * v.0.0.1
 * 0.0.1
 * 1. implement -b, -n, -v, -e, -t, -s
 * 2. implement multiple text files together - Done
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 	// string manipulation
#include <sys/stat.h> 	// check if it is a file
#include <stdbool.h> 	// if boolean or not
#include <unistd.h> 	// for 'dup' and 'fileno'

#define BLOCK 1024 // optimal

/* declare option functions, might move this to a header file*/ 
void number_non_blank_lines(); 	// -b
void number_lines(); 			// -n
void non_print(); 				// -v
void non_print_char_dollar(); 	// -e
void non_print_tab(); 			// -t
void squeeze_empty_lines(); 	// -s
void advisory_lock(); 			// -l
void disable_out_buffer(); 		// -u
 
/* global option variables */
int SOMENUM 	= 0; 	// -b
int ALLNUM 		= 0;  	// -n
int ALLCHAR 	= 0;  	// -v
int DOLLAR 		= 0;  	// -e
int TAB 		= 0;  	// -t
int SQUEEZE 	= 0;	// -s
int LOCK 		= 0; 	// -l
int DISABLE 	= 0; 	// -u

// error print message
void invalidOption(char c){
	fprintf(stderr, "copycat: illegal option -- %c\n", c);
	fprintf(stderr, "usage: ./copycat [-belnstuv] [file ...]\n");
	exit(1);
}

// check if this is a valid option
void isValidOption(char *str){
	for (int i = 0; i < strlen(str); i++){
		if (str[i] != 'b' && str[i] != 'n' && str[i] != 'v'
		&& str[i] != 'e' && str[i] != 't' && str[i] != 's'
		&& str[i] != 'l' && str[i] != 'u'){
			invalidOption(str[i]);
		}
	}
}

// options for user to choose
void option(char *str){
	extern int SOMENUM, ALLNUM, ALLCHAR, DOLLAR, 
					TAB, SQUEEZE, LOCK, DISABLE;

	while (*str != 0){
		switch(*str++){
			case 'b': 
				SOMENUM = 1; 
				if (ALLNUM == 1) ALLNUM = 0; // b or n goes first?
				break;	
			case 'n': 
				ALLNUM = 1; 
				if (SOMENUM == 1) SOMENUM = 0; // ^^
				break;
			case 'v': ALLCHAR 	= 1; break;
			case 'e': DOLLAR 	= 1; break;
			case 't': TAB 		= 1; break;
			case 's': SQUEEZE 	= 1; break;
			case 'l': LOCK 		= 1; break;
			case 'u': DISABLE 	= 1; break;
			default:				 break;
		}
	}
}

// return true if file exists
// https://www.learnc.net/c-tutorial/c-file-exists/
bool file_exists(const char *filename){
	struct stat buffer;
	return stat(filename, &buffer) == 0 ? true : false;
}

// perform simple printing
void performOperation(const char *filename){
	
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL){
		fprintf(stderr, "Error opening file\n");
		exit(1);
	}	
    unsigned char buffer[BLOCK];
    int byte_read;

    while ((byte_read = fread(buffer, 1, BLOCK, fp)) > 0){
        fwrite(buffer, 1, byte_read, stdout);
    }

    fclose(fp); // Close the file
}

// simplyEcho() function
void simplyEcho(){
	char buffer[BLOCK];
	while ( fgets(buffer, BLOCK, stdin) != NULL ){ 
		fprintf(stdout, "%s", buffer);
	}
}

// function for development
void optionSelectionPrint(){
	fprintf(stderr, "-------------------------\n"
					"Selections user did:\n"
					"int SOMENUM 	= %d; 	// -b\n"
					"int ALLNUM 	= %d;  	// -n\n"
					"int ALLCHAR 	= %d;  	// -v\n"
					"int DOLLAR 	= %d;  	// -e\n"
					"int TAB 	= %d;  	// -t\n"
					"int SQUEEZE 	= %d;	// -s\n"
					"int LOCK 	= %d; 	// -l\n"
					"int DISABLE 	= %d;	// -u\n" 
					"-------------------------\n"
					, SOMENUM, ALLNUM, ALLCHAR, DOLLAR, TAB, SQUEEZE, LOCK, DISABLE);
}

int main(int argc, char *argv[]){
	if (argc == 1){
		simplyEcho();
		return 0;
	}

	for (int i = 1; i < argc; i++){
		if (*argv[i] == '-'){
			if (strlen(argv[i]) == 1){
				simplyEcho();
			}
			else{
				isValidOption(argv[i] + 1); // error checking
				option(argv[i] + 1); // ex: "-bnv" -> "bnv"
			}
		}
		else{
			if (file_exists(argv[i]))
				performOperation(argv[i]); // execute the files
			else
				fprintf(stderr, "copycat: %s: No such file or directory\n", argv[i]);
		}
		clearerr(stdin); // clear EOF flag after ever call  ...
	}
	optionSelectionPrint();
	return 0;
}
