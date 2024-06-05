/*
 * Reverse-engineer of [cat] function
 * author: Quoc Bui
 * v.0.0.1
 * 1. implement -b, -n, -v, -e, -t, -s
 *    Have implemented -n and when user input nothing
 *    Have implemented -b -s properly
 *    Implemented -u disable_lock
 * 	  Implemented -v -t -e for displaying special characters
 * 2. implement multiple text files together - Done
 * 3. implement copycat - - - | Done
 * 4. implemented ./copycat -benv 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 	// string manipulation
#include <sys/stat.h> 	// check if it is a file
#include <stdbool.h> 	// if boolean or not
#include <unistd.h> 	// for 'dup' and 'fileno'

#define BLOCK 1024 // 1KB
 
/* global option variables */
int SOMENUM 	= 0; 	// -b
int ALLNUM 		= 0;  	// -n
int ALLCHAR 	= 0;  	// -v
int DOLLAR 		= 0;  	// -e
int TAB 		= 0;  	// -t
int SQUEEZE 	= 0;	// -s
int LOCK 		= 0; 	// -l
int DISABLE 	= 0; 	// -u


// return true if file exists | https://www.learnc.net/c-tutorial/c-file-exists/
bool fileExists(const char *filename){
	struct stat buffer;
	return stat(filename, &buffer) == 0 ? true : false;
}

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

// number the lines function -n
int printNumLines(int lineNo, char prevChar){
	if (ALLNUM != 1) return lineNo;
	
	if (prevChar == '\n')
		fprintf(stdout, "%6d  ", lineNo++);
	
	return lineNo;
}

//  number the non blank lines function -b
int printNumNonBlank(int lineNo, char curChar, char prevChar){
	if (SOMENUM != 1) return lineNo;
	
	if (prevChar == '\n' && curChar != '\n')
		fprintf(stdout, "%6d  ", lineNo++);
	
	return lineNo;
}

// squeeze empty lines -s 
int squeezeEmptyLines(char ch, int *consecutiveNewlines){
	if (SQUEEZE != 1) return 1;
	
	if (ch == '\n') {
		(*consecutiveNewlines)++;
			if (*consecutiveNewlines > 2)  return 0; // Skip printings multi-lines
	} 
	else 
		*consecutiveNewlines = 0;
    
	return 1;
}

// display non-printing chars -v
void makeVisibleChar(int ch){

	if (ch == '\n') // get rid of case ^J as \n = 10 
		putchar(ch); 
	else if (ch == 9 && TAB == 1) // -t | display tab if user selected the option
		fprintf(stdout,"^%c", ch + 64); // because I = 73
	else if (ch >= 1 && ch<= 26 && ch != 9)
		fprintf(stdout, "^%c", ch + 64); // because A = 65
	else if(ch == 127)
		fprintf(stdout, "^?");
	else if(ch >= 128)
		fprintf(stdout, "M-%c", ch & 0x7F); // characters of low 7 bits
	else
		putchar(ch);
		
}

// replace putchar() in processFile(), added with options
void modifyPrintChar(int ch){
	if (ALLCHAR == 1 || TAB == 1 || DOLLAR == 1){
		if (ch == '\n' && DOLLAR == 1){ // -e option, only for end of line
			putchar('$');
			putchar(ch);
			return;
		}
		makeVisibleChar(ch);
	}
	else
		putchar(ch);
}

// print lines
void processFile(FILE *fp){
	int ch;
	int lineNo = 1;
	int prevChar = EOF;
	int consecutiveNewlines;

	// Check if the file is empty
	if ((ch = fgetc(fp)) == EOF) return;

	// for '-b' and '-n'
	if (ALLNUM == 1 || (SOMENUM == 1 && ch != '\n')){
		fprintf(stdout, "%6d  ", lineNo++);
	}

	// Process the first char
	putchar(ch);
	prevChar = ch;

	// Use fgetc to read the file
	while ( (ch = fgetc(fp)) != EOF){
	
		// -s
		if ( !squeezeEmptyLines(ch, &consecutiveNewlines) ){
			continue; // just simply skip this iteration
		}

		// -n
		lineNo = printNumLines(lineNo, prevChar);	
		
		// -b
		lineNo = printNumNonBlank(lineNo, ch, prevChar);
		
		prevChar = ch; // to deal with EOF cases
		modifyPrintChar(ch); // replacement for putchar()
	}

}

// open and close files
void performOperation(const char *filename){
	
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL){
		fprintf(stderr, "Error opening file\n");
		exit(1);
	}	
	
	processFile(fp);

    fclose(fp); // Close the file
}

/* simplyEcho() function
void simplyEcho(){
	char buffer[BLOCK];
	while ( fgets(buffer, BLOCK, stdin) != NULL ){ 
		fprintf(stdout, "%s", buffer);
	}
}*/

// simpleyEcho() version 2
void simplyEcho(){
	processFile(stdin);
}

// test function pretty when printed so I will not remove it ...
/* check which options user selected
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
}*/

int main(int argc, char *argv[]){
	if (argc == 1){
		simplyEcho();
		return 0;
	}

	// case for copycat -options <no files>
	if (argc == 2){
		if (*argv[1] == '-'){
			isValidOption(argv[1] + 1); // error checking
			option(argv[1] + 1); // ex: "-bnv" -> "bnv"
		}
		simplyEcho();
		return 0;
	}

	// -u disable output buffering, placed here so we can call it once
	if (DISABLE == 1)
		setvbuf(stdout, NULL, _IONBF, 0);

	for (int i = 1; i < argc; i++){
		if (*argv[i] == '-'){ // make this more dynamic
			if (strlen(argv[i]) == 1){
				simplyEcho();
			}
			else{
				isValidOption(argv[i] + 1); // error checking
				option(argv[i] + 1); // ex: "-bnv" -> "bnv"
			}
		}
		else{
			if (fileExists(argv[i]))
				performOperation(argv[i]); // execute the files
			else
				fprintf(stderr, "copycat: %s: No such file or directory\n", argv[i]);
		}
		clearerr(stdin); // clear EOF flag after ever call  ...
	}

	return 0;
}
