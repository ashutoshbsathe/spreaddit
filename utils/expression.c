#include <limits.h>
#include <string.h>
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
