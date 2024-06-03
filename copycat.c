/*
 * Reverse-engineer of [cat] function
 * author: Quoc Bui
 * v.0.0.1
 * 0.0.1
 * 1. implement -b, -n, -v, -e, -t, -s
 * 2. implement multiple text files together
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // string manipulation
#include <sys/stat.h> // check if it is a file
#include <stdbool.h> // if boolean or not

#define BLOCK 1024 // optimal

/* declare option functions, might move this to a header file*/ 
void number_non_blank_lines(); // -b
void number_lines(); // -n
void non_print(); // -v
void non_print_char_dollar(); // -e
void non_print_tab(); // -t
void squeeze_empty_lines(); // -s
void advisory_lock(); // -l
void disable_out_buffer(); // -u
 
/* global option variables */
int SOMENUM = 0 // -b
int ALLNUM = 0 // -n
int ALLCHAR = 0 // -v
int DOLLAR = 0 // -e
int TAB = 0 // -t
int SQUEEZE = 0 // -s
int LOCK = 0 // -l
int DISABLE = 0 // -u

// error print message
void invalidOption(char c){
	fprintf(stderr, "cat: illegal option -- %c\n", c);
	fprintf(stderr, "usage: cat [-belnstuv] [file ...]\n");
	exit(1);
}

// check if this is a valid option
int isValidOption(char *str){
	for (int i = 0; i < strlen(str); i++){
		if (str[i] != 'b' || str[i] != 'n' || str[i] != 'v'
		|| str[i] != 'e' || str[i] != 't' || str[i] != 's'
		|| str[i] != 'l' || str[i] != 'u'){
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
			case 'b': SOMENUM 	= 1; break;	
			case 'n': ALLNUM 	= 1; break;
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
bool file_exists(const char *filename){
	struct stat buffer;
	return stat(filename, &buffer) == 0 ? true : false;
}

int main(int argc, char *argv[]){
	for (int i = 1; i < argc; i++){
		if (*argv[i] == '-'){
			isValidOption(argv[i] + 1); // error checking
			option(argv[i] + 1); // ex: "-bnv" -> "bnv"
		}
		else{
			if (file_exists(argv[i]))
				performOperation(argv[i]); // execute the files
			else
				fprintf(stderr, "copycat: %s: No such file or directory\n", argv[i]);
		}
	}
	return 0;
}


void argc_eq_1(){	
	char buffer[BLOCK];

}

/* ./copycat <file> */
void argc_eq_2(FILE *fp){

	// file size of fp
	fseek(fp, 0, SEEK_END); // go to end
	int file_size = ftell(fp);
	rewind(fp); // put the pointer back

	// allocate memory for string
	char *buffer = malloc(sizeof(char) * (file_size + 1));

	// read fp into buffer
	while ( fread(buffer, 1, BLOCK, fp) > 0 ){}

	buffer[file_size] = 0; // add null-terminate
	printf("%s", buffer);

	free(buffer);

}

void argc_eq_3(FILE *fp){

}

int main (int argc, char **argv){
	if (argc <= 1){
		argc_eq_1();
	}

	if (argc != 2){
		fprintf(stderr, "Usage: ./copycat <file>\n");
		return 1;
	}

	FILE *fp = fopen(*(argv + 1), "rb");
	if (fp == NULL){
		return 1;
	}

	argc_eq_2(fp);

	fclose(fp);
	return 0;
}
