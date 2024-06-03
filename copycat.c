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

#define BLOCK 1024 // optimal

void number_non_blank_lines(); // -b

void number_lines(); // -n

void non_print(); // -v

void non_print_char_dollar(); // -e

void non_print_tab(); // -t

void squeeze_empty_lines(); // -s

void advisory_lock();



 


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
