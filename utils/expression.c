#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define POS 300
#define INT 400
#define OPR 500
#define ENT 600
#define ERR 700
typedef struct pos {
	int row;
	int col;
}pos;
typedef struct form {
	int type;
	union formula{
		pos position;
		int num;
		char op;
	} formuladata;
}formulatoken;
int number(char *string) {
	int ret = 0, i;
	if(strlen(string) == 1) {
		ret = string[0] - 'A' + 1;
	}	
	else {
		i = 0;
		while(string[i + 1]) {
			ret += 26 + string[i] - 'A';
			i++;
		}
		ret += string[i] - 'A' + 1;
	}
	return ret;
}

#define LETTER 1001
#define NUMBER 1000
#define START 301
#define ERROR 401
#define POSI  501
#define OPRTR 601
#define EOFST 701
#define OPRND 801
#define SPACE 901
formulatoken getnexttoken(const char *formula, int *restart) {
	static int currstate = START;
	int nextstate;
	static int prevstate = START;
	static int i = 0;
	static int posiindex = 0;
	static char posi[10];
	formulatoken ret;
	static int sum = 0;
	static char currchar, currop;
	if(*restart == 1) {
		currstate = START;
		*restart = 0;
		i = 0;
	}
	while(1) {
		currchar = formula[i];
		switch(currstate) {
			case START :
				switch(currchar) {
					case 'A' : case 'B' : case 'C' : case 'D' : case 'E': case 'F' : case 'G' : case 'H' : case 'I' :
					case 'J' : case 'K' : case 'L' : case 'M' : case 'N': case 'O' : case 'P' : case 'Q' : case 'R' :
					case 'S' : case 'T' : case 'U' : case 'W' : case 'X': case 'Y' : case 'Z' :
						posiindex = 0;
						posi[posiindex++] = currchar;
						prevstate = LETTER;
						ret.formuladata.position.row = ret.formuladata.position.col = INT_MIN;
						nextstate = POSI;
						break;
					case '0' : case '1' : case '2' : case '3' : case '4' : 
					case '5' : case '6' : case '7' : case '8' : case '9' :
						sum = currchar - '0';
						nextstate = OPRND;
						break;
					case '+' : case '-' : case '*' : case '/' : case '(' : case ')' :
						nextstate = OPRTR;
						currop = currchar;
						break;
					case ' ' : case '\t' :
						nextstate = SPACE;
						break;
					case '\0':
						nextstate = EOFST;
						break;
					default  :
						nextstate = ERROR;
						break;
				}
				break;
			case OPRND :
				switch(currchar) {
					case 'A' : case 'B' : case 'C' : case 'D' : case 'E': case 'F' : case 'G' : case 'H' : case 'I' :
					case 'J' : case 'K' : case 'L' : case 'M' : case 'N': case 'O' : case 'P' : case 'Q' : case 'R' :
					case 'S' : case 'T' : case 'U' : case 'W' : case 'X': case 'Y' : case 'Z' :
						ret.formuladata.num = sum;
						ret.type = INT;
						posiindex = 0;
						posi[posiindex++] = currchar;
						prevstate = LETTER;
						ret.formuladata.position.row = ret.formuladata.position.col = INT_MIN;
						nextstate = POSI;
						currstate = nextstate;
						i++;
						return ret;
						break;
					case '0' : case '1' : case '2' : case '3' : case '4' : 
					case '5' : case '6' : case '7' : case '8' : case '9' :
						sum = sum * 10 + currchar - '0';
						nextstate = OPRND;
						break;
					case '+' : case '-' : case '*' : case '/' : case '(' : case ')' :
						ret.formuladata.num = sum;
						ret.type = INT;
						nextstate = OPRTR;
						currop = currchar;
						currstate = nextstate;
						i++;
						return ret;
						break;
					case ' ' : case '\t' :
						nextstate = SPACE;
						ret.formuladata.num = sum;
						ret.type = INT;
						currstate = nextstate;
						i++;
						return ret;
						break;
					case '\0':
						ret.formuladata.num = sum;
						ret.type = INT;
						nextstate = EOFST;
						currstate = nextstate;
						i++;
						return ret;
						break;
					default  :
						ret.formuladata.num = sum;
						ret.type = INT;
						nextstate = ERROR;
						currstate = nextstate;
						i++;
						return ret;
						break;
				}
				break;
			case POSI  :
				switch(currchar) {
					case 'A' : case 'B' : case 'C' : case 'D' : case 'E': case 'F' : case 'G' : case 'H' : case 'I' :
					case 'J' : case 'K' : case 'L' : case 'M' : case 'N': case 'O' : case 'P' : case 'Q' : case 'R' :
					case 'S' : case 'T' : case 'U' : case 'W' : case 'X': case 'Y' : case 'Z' :
						posi[posiindex++] = currchar;
						nextstate = POSI;
						prevstate = LETTER;
						break;
					case '0' : case '1' : case '2' : case '3' : case '4' : 
					case '5' : case '6' : case '7' : case '8' : case '9' :
						if(prevstate == LETTER) {
							posi[posiindex] = '\0';
							ret.formuladata.position.col = number(posi);
							sum = currchar - '0';
						}
						else
							sum = sum * 10 + currchar - '0';
						nextstate = POSI;
						prevstate = NUMBER;
						break;
					case '+' : case '-' : case '*' : case '/' : case '(' : case ')' :
						ret.formuladata.position.row = sum;
						if(prevstate == NUMBER) {
							ret.type = POS;
							nextstate = OPRTR;
						}
						else {
							ret.type = ERR;
							nextstate = ERROR;
						}
						nextstate = OPRTR;
						currop = currchar;
						currstate = nextstate;
						i++;
						posiindex = 0;
						return ret;
						break;
					case ' ' : case '\t' :
						ret.formuladata.position.row = sum;
						if(prevstate == NUMBER) {
							ret.type = POS;
							nextstate = SPACE;
						}
						else {
							ret.type = ERR;
							nextstate = ERROR;
						}
						currstate = nextstate;
						i++;
						posiindex = 0;
						return ret;
						break;
					case '\0':
						ret.formuladata.position.row = sum;
						if(prevstate == NUMBER) {
							ret.type = POS;
							nextstate = EOFST;
						}
						else {
							ret.type = ERR;
							nextstate = ERROR;
						}
						currstate = nextstate;
						i++;
						posiindex = 0;
						return ret;
						break;
					default  :
						ret.type = ERR;
						nextstate = ERROR;
						currstate = nextstate;
						i++;
						return ret;
						break;
				}
				break;
			case OPRTR :
				switch(currchar) {
					case 'A' : case 'B' : case 'C' : case 'D' : case 'E': case 'F' : case 'G' : case 'H' : case 'I' :
					case 'J' : case 'K' : case 'L' : case 'M' : case 'N': case 'O' : case 'P' : case 'Q' : case 'R' :
					case 'S' : case 'T' : case 'U' : case 'W' : case 'X': case 'Y' : case 'Z' :
						posiindex = 0;
						posi[posiindex++] = currchar;
						ret.formuladata.position.row = ret.formuladata.position.col = INT_MIN;
						nextstate = POSI;
						prevstate = LETTER;
						ret.formuladata.op = currop;
						ret.type = OPR;
						currstate = nextstate;
						i++;
						return ret;
						break;
					case '0' : case '1' : case '2' : case '3' : case '4' : 
					case '5' : case '6' : case '7' : case '8' : case '9' :
						sum = currchar - '0';
						nextstate = OPRND;
						ret.formuladata.op = currop;
						ret.type = OPR;
						currstate = nextstate;
						i++;
						return ret;
						break;
					case '+' : case '-' : case '*' : case '/' : case '(' : case ')' :
						nextstate = OPRTR;
						currop = currchar;
						ret.formuladata.op = currop;
						ret.type = OPR;
						currstate = nextstate;
						i++;
						return ret;
						break;
					case ' ' : case '\t' :
						nextstate = SPACE;
						ret.formuladata.op = currop;
						ret.type = OPR;
						currstate = nextstate;
						i++;
						return ret;
						break;
					case '\0':
						nextstate = EOFST;
						ret.formuladata.op = currop;
						ret.type = OPR;
						currstate = nextstate;
						i++;
						return ret;
						break;
					default  :
						nextstate = ERROR;
						ret.formuladata.op = currop;
						ret.type = OPR;
						currstate = nextstate;
						i++;
						return ret;
						break;
				}
				break;
			case SPACE :
				switch(currchar) {
					case 'A' : case 'B' : case 'C' : case 'D' : case 'E': case 'F' : case 'G' : case 'H' : case 'I' :
					case 'J' : case 'K' : case 'L' : case 'M' : case 'N': case 'O' : case 'P' : case 'Q' : case 'R' :
					case 'S' : case 'T' : case 'U' : case 'W' : case 'X': case 'Y' : case 'Z' :
						posiindex = 0;
						posi[posiindex++] = currchar;
						prevstate = LETTER;
						ret.formuladata.position.row = ret.formuladata.position.col = INT_MIN;
						nextstate = POSI;
						break;
					case '0' : case '1' : case '2' : case '3' : case '4' : 
					case '5' : case '6' : case '7' : case '8' : case '9' :
						sum = currchar - '0';
						nextstate = OPRND;
						break;
					case '+' : case '-' : case '*' : case '/' : case '(' : case ')' :
						nextstate = OPRTR;
						currop = currchar;
						break;
					case ' ' : case '\t' :
						nextstate = SPACE;
						break;
					case '\0':
						nextstate = EOFST;
						break;
					default  :
						nextstate = ERROR;
						break;
				}
				break;
			case EOFST :
				ret.type = ENT;
				return ret;
				break;
			case ERROR :
				ret.type = ERR;
				return ret;
				break;
		}
		currstate = nextstate;
		i++;
	}
}
#define DDMMYYYY 50
#define DD1MM1YYYY 53
#define DMONTHYYYY 54
char *formatdate(const char *str, int format) {
	char *input = (char *)malloc(strlen(str) + 1);
	strcpy(input, str);
	char tmp[5];
	char *tmp2;
	char months[12][5] = {"Jan.", "Feb.", "Mar.", "Apr.", "May.", "Jun.", "Jul.", "Aug.", "Sep.", "Oct.", "Nov.", "Dec."};
	int i, j, sum, currformat;
	for(i = 0; i < strlen(input); i++) {
		if(input[i] == ' ') {
			for(j = i + 1; j <= strlen(input); j++)
				input[j - 1] = input[j];
		}
	}
	puts(input);
	if(strlen(input) < 10) {
		free(input);
		return NULL;
	}
	sum = (input[0] - '0') * 10 + (input[1] - '0');
	if(sum < 1 || sum > 31) {
		printf("Returning cuz date invalid\n");
		free(input);
		return NULL;
	}
	if(input[2] == '/' || input[2] == '-') {
		if(input[2] == '/')
			currformat = DDMMYYYY;
		else
			currformat = DD1MM1YYYY;
		sum = (input[3] - '0') * 10 + (input[4] - '0');
		if(sum < 1 || sum > 12) {
			free(input);
			printf("Returning cus month invalid\n");
			return NULL;
		}
	}
	else {
		currformat = DMONTHYYYY;
		for(i = 0; i < 4; i++)
			tmp[i] = input[i + 2];
		puts(tmp);
		for(i = 0; i < 12; i++) {
			if(strcasecmp(tmp, months[i]) == 0)
				break;
		}
		if(i == 12) {
			free(input);
			printf("Returning cuz i = 12\n");
			return NULL;
		}
		else
			sum = i;
	}
	for(i = 6; i < 10; i++) {
		if(input[i] < '0' || input[i] > '9') {
			free(input);
			return NULL;
		}
	}
	/*
	 * So the string is now nicely formatted
	 */
	switch(format) {
		case DDMMYYYY: 
			if(currformat == DDMMYYYY)
				 return input;
			else if(currformat == DD1MM1YYYY) {
				input[2] = '/';
				input[5] = '/';
				return input;
			}
			else {
				printf("%d\n", sum);
				tmp2 = (char *)malloc(10);
				tmp2[0] = input[0];
				tmp2[1] = input[1];
				tmp2[2] = '/';
				tmp2[3] = '0' + ((sum + 1) / 10);
				tmp2[4] = '0' + ((sum + 1) % 10);
				tmp2[5] = '/';
				tmp2[6] = input[6];
				tmp2[7] = input[7];
				tmp2[8] = input[8];
				tmp2[9] = input[9];
				tmp2[10] = '\0';
				free(input);
				return tmp2;
			}
			break;
		case DD1MM1YYYY:
			if(currformat == DDMMYYYY) {
				input[2] = '-';
				input[5] = '-';
				return input;
			}
			else if(currformat == DD1MM1YYYY)
				return input;
			else {
				tmp2 = (char *)malloc(10);
				tmp2[0] = input[0];
				tmp2[1] = input[1];
				tmp2[2] = '-';
				tmp2[3] = '0' + ((sum + 1) / 10);
				tmp2[4] = '0' + ((sum + 1) % 10);
				tmp2[5] = '-';
				tmp2[6] = input[6];
				tmp2[7] = input[7];
				tmp2[8] = input[8];
				tmp2[9] = input[9];
				tmp2[10] = '\0';
				free(input);
				return tmp2;
			}
		case DMONTHYYYY:
			if(currformat == DDMMYYYY) {
				tmp2 = (char *)malloc(12);
				tmp2[0] = input[0];
				tmp2[1] = input[1];
				tmp2[2] = ' ';
				tmp2[3] = '\0';
				strcat(tmp2, months[sum - 1]);
				tmp2[7] = ' ';
				tmp2[8] = input[6];
				tmp2[9] = input[7];
				tmp2[10] = input[8];
				tmp2[11] = input[9];
				tmp2[12] = '\0';
				free(input);
				return tmp2;
			}
			else if(currformat == DD1MM1YYYY) {
				tmp2 = (char *)malloc(12);
				tmp2[0] = input[0];
				tmp2[1] = input[1];
				tmp2[2] = ' ';
				tmp2[3] = '\0';
				strcat(tmp2, months[sum - 1]);
				tmp2[7] = ' ';
				tmp2[8] = input[6];
				tmp2[9] = input[7];
				tmp2[10] = input[8];
				tmp2[11] = input[9];
				tmp2[12] = '\0';
				free(input);
				return tmp2;
			}
			else {
				return input;
			}
		default:
			return NULL;
	}
}
