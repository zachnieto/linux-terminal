
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "svec.h"

// Reads from the current index for len characters, then
// adds to the given svec.
void read_token(svec* vec, char *line, int index, int len) {
	
	char *str = malloc(len + 1);
	memcpy(str, line + index, len);
	str[len] = '\0';
	svec_push_back(vec, str);
}

// Parses the given line for tokens, and adds them to
// the given svec.
void parse(char *line, svec* vec) {

	int length = 0;
	for (int i = 0; i + length <= strlen(line);) {

		if (length != 0 && (line[i + length] == ' ' 
			|| line[i + length] == '\0'
			|| line[i + length] == '\n')) { 

			read_token(vec, line, i, length);				

			i = i + length + 1;	
			length = 0;	
		}
		else if (line[i + length] == '"') {
			
			for (int x = i + length + 1; x+length < strlen(line); length++)
				if (line[x+length] == '"') {
					read_token(vec, line, x, length);
					i = x + length + 1;
					length = 0;
					break;
				}

		}
		else if (line[i + length] == ';'|| line[i + length] == '<'
					|| line[i + length] == '>') {

			if (length != 0)
				read_token(vec, line, i, length);

			if (line[i + length + 1] == '|') {
				read_token(vec, line, i + length, 2);	
				i = i + length + 2;
			}
			else {
				read_token(vec, line, i + length, 1);
				i = i + length + 1;
			}

			length = 0;	
		}
		else if (line[i + length] == '|') {
			if (length != 0)
				read_token(vec, line, i, length);

			if (line[i + length + 1] == '|') {
				read_token(vec, line, i + length, 2);	
				i = i + length + 2;
			}
			else {
				read_token(vec, line, i + length, 1);
				i = i + length + 1;
			}

			length = 0;	
		}
		else if (line[i + length] == '&') {
			if (length != 0)
				read_token(vec, line, i, length);

			if (line[i + length + 1] == '&') {
				read_token(vec, line, i + length, 2);	
				i = i + length + 2;
			}
			else {
				read_token(vec, line, i + length, 1);
				i = i + length + 1;
			}

			length = 0;	
		}
		else if (line[i] != ' ' && line[i] != '\t') {
			length++;
		}
		else
			i++;
	}

}


