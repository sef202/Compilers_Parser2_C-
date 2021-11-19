#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h> // for isalpha() and isdigit()
#define BUFSIZE 100
FILE *fp; // file pointer to input source code file
FILE *outputPtr; // file pointer to output text file
char buffer[BUFSIZE];
void printToken(FILE *, const char *, const char *);
void printCharToken(FILE *, const char *, char);
bool isSeparator(char);
bool isOperator(char);
bool isEmpty(char);
bool isKeyword(const char *);
char nextChar;
int buf_i = 0;
char str[15];

enum Symbols {
	NT_S,       // Statement
	NT_E,       // Expression
	NT_A,       // Assignment
	NT_T,       // Term
	NT_F,       // Factor
	T_PLUS,     // +
	T_MINUS,    // -
	T_MULTI,    // *
	T_DIV,      // /
	T_L_PARENS, // (
	T_R_PARENS, // )
	T_ID,       // identifier
	T_NUM,      // integer number
	T_EMPTY,    // empty (epsilon)
	T_COMMA,    // ,
	T_KEYWORD,  // int, float, bool, char
};

enum FsmState { START, ID_START, IN_ID, ID_END, KEYWORD_END,
								NUM_START, IN_NUM, IN_REAL, INT_END, REAL_END,
								COMMENT_START, IN_COMMENT, END_COMMENT,
							 	OPERATOR, OPERATOR_END, SEPARATOR };

void printToken(FILE* fp, const char* token, const char* lexeme) { //print to output.txt
	fprintf(fp, "\nToken: %s \tLexeme: %s \n", token, lexeme);
}

void printCharToken(FILE* fp, const char* token, char lexeme) { // for printing single char separator/operator
	fprintf(fp, "\nToken: %s \tLexeme: %c \n ", token, lexeme);
}

void printRule(FILE* fp, const char* rule) {
	fprintf(fp, "%s\n", rule);
}
bool isOperator(char ch) {
	int match;
  switch (ch) {
    case '=': case '<': case '>': case '+': case '-':
    case '/': case '%': case '*': match = 1; break;
    default: match = 0; break;
  }
	return match;
}

bool isSeparator(char ch) {
	int match;
  switch (ch) {
    case '(': case ')': case '{': case '}': case '[': case ']':
    case ',': case '.': case ':': case ';': case '!': match = 1; break;
    default: match = 0; break;
  }
	return match;
}

bool isKeyword(const char* buf) {
	const char* keywords[] = { "int", "float", "bool", "if", "else", "then", "endif", "while", "whileend",
						  	  					 "do", "doend", "for", "forend", "input", "output", "and", "or" , "function" };
	int match = 0;

	for (int i = 0; i < 18; i++) {
		if (strcmp(keywords[i], buf) == 0) {
			match = 1;
			break;
		}
	}
	return match;
}

bool isEmpty(char ch) {
  int match;
  switch (ch) {
    case ' ': case '\t': case '\n': match = 1; break;
    default: match = 0; break;
  }
  return match;
}

enum Symbols lexer_main (char ch) {
  char symBuf = ch;
  char tokBuf[100];
  int i = 0;
  enum FsmState currentState = START;

  if (isSeparator(ch) || isOperator(ch)) {
    switch(ch) {
      case '+':
  			printf("\nToken: Operator\tLexeme: %c\n", ch);
				printCharToken(outputPtr, "OPERATOR", ch);
  			return T_PLUS;
  			break;
  		case '-':
  			printf("\nToken: Operator\tLexeme: %c\n", ch);
				printCharToken(outputPtr, "OPERATOR", ch);
  			return T_MINUS;
  			break;
  		case '*':
  			printf("\nToken: Operator\tLexeme: %c\n", ch);
				printCharToken(outputPtr, "OPERATOR", ch);
  			return T_MULTI;
  			break;
  		case '/':
  			printf("\nToken: Operator\tLexeme: %c\n", ch);
				printCharToken(outputPtr, "OPERATOR", ch);
  			return T_DIV;
  			break;
  		case '(':
  			printf("\nToken: Separator\tLexeme: %c\n", ch);
				printCharToken(outputPtr, "SEPARATOR", ch);
  			return T_L_PARENS;
  			break;
  		case ')':
  			printf("\nToken: Separator\tLexeme: %c\n", ch);
				printCharToken(outputPtr, "SEPARATOR", ch);
  			return T_R_PARENS;
  			break;
  		case '=':
  			printf("\nToken: Operator\tLexeme: %c\n", ch);
				printCharToken(outputPtr, "OPERATOR", ch);
  			break;
  		case ';':
  			printf("\nToken: Separator\tLexeme: %c\n", ch);
				printCharToken(outputPtr, "SEPARATOR", ch);
  			printf("e -> epsilon\n");
				printRule(outputPtr, "e -> epsilon");
  			return T_EMPTY;
  			break;
			default:
				printf("e -> epsilon\n");
				printRule(outputPtr, "e -> epsilon");
				return T_EMPTY;
				break;
    }
  }
  else {
    while (!(isSeparator(ch)) && !(isOperator(ch)) && !(isEmpty(ch)) && ch != '\0') {
      switch(currentState) {
        case START:   // 0 - initial identifier state
          if (ch == '\r' || ch == ' ' || ch == '\n' || ch == '\t') {
            break;		// skips any white space in beginning of tokBuf
          } else {
            tokBuf[i++] = ch; // add current character to
            if(isalpha(ch) > 0) {
              currentState = ID_START; // if alphabetical character, go to state 1 identifier start
            }
            else if (isdigit(ch) > 0) { // if digit character, go to state 5 number start
              currentState = NUM_START;
            }
            else if (ch == '!') { // "!" signifies beginning/end of a comment, go to state 10 comment start
              currentState = COMMENT_START;
            }
          }
          break;
        case ID_START: // 1 - initial identifier state - can only go to 2nd state
  				if (isSeparator(ch) || isOperator(ch)) {
  					symBuf = ch; // for encountering separators/operators at ends of identifiers with no space separator ( i.e. x=a )
  					currentState = ID_END;
  				}
  				else {
  					tokBuf[i++] = ch;
  					currentState = IN_ID;
  				}
  				break;

  			case IN_ID: // state 2 - can go back to 2 or to state 3 ID end accepting state
  				if (isalpha(ch) > 0 || isdigit(ch) > 0 || ch == '$') {
  					tokBuf[i++] = ch;		//legal identifiers start with alphabetical character and can include any number or '$' character
  					currentState = IN_ID;
  				}
  			 	else if (isSeparator(ch) || isOperator(ch)) {
  					symBuf = ch; // for encountering separators/operators at ends of identifiers with no space separator ( i.e. function(); )
  					currentState = ID_END;
  				}
  				else { currentState = ID_END; }
  				break;

  			case NUM_START: // 5 - initial number state - can go to 6th state inside number
  				if (isSeparator(ch) || isOperator(ch)) {
  					currentState = INT_END;
  					symBuf = ch;
  				} else {
  					tokBuf[i++] = ch;
  					currentState = IN_NUM;
  			 	}
  				break;

  			case IN_NUM: // state 6 - can go back to state 6, or state 7 in real number, or state 8 in int end
  				if (isdigit(ch) > 0) {
  					tokBuf[i++] = ch;
  					currentState = IN_NUM;
  				}
  				else if(ch == '.') {
  			 		tokBuf[i++] = ch;
  					currentState = IN_REAL;
  				}
  				else if (isSeparator(ch) || isOperator(ch)) {
  					symBuf = ch;
  					currentState = INT_END;
  				}
  				else { currentState = INT_END; }
  				break;

  			case IN_REAL: // state 7 - can go back to state 7 or state 9 real number end
  				if (isdigit(ch) > 0) {
  					tokBuf[i++] = ch;
  					currentState = IN_REAL;
  				}
  				else { currentState = REAL_END; }
  				break;

  			case COMMENT_START: // state 10 - can only go to state 11 inside comment
  				tokBuf[i++] = ch;
  				currentState = IN_COMMENT;
  				break;

  			case IN_COMMENT: // state 11 - state continues to loop back to state 11 inside comment until end comment '!' is reached
  				tokBuf[i++] = ch;
  				if (ch == '!') {
  					currentState = END_COMMENT;
  				}
  				break;
        default:
  				printf("Error: invalid state.\n");
  				break;
      }
      //ch = str[++str_i];
			// ch = buffer[buf_i++];
			// nextChar = ch;
			//ch = buffer[++buf_i];
			//nextChar = ch;
			if (isSeparator(ch) || isOperator(ch) || isEmpty(ch) || ch == '\0') {
				switch(currentState) {
					case IN_ID: case ID_START:
						currentState = ID_END;
						break;
					case NUM_START: case IN_NUM:
						currentState = INT_END;
						break;
					default: break;
				}
      }

      if (currentState == ID_END || currentState == INT_END || currentState == REAL_END || currentState == END_COMMENT) {  // final accepting states
					tokBuf[i] = '\0';
					if (currentState == ID_END) {
						if(isKeyword(tokBuf)) { //check if string is a keyword or identifier
							printf("\nToken: Keyword\tLexeme: %s\n", tokBuf); // print to stdout
							printToken(outputPtr, "KEYWORD", tokBuf);         // and print to output.txt
							return T_KEYWORD;
						} else {
							printf("\nToken: Identifier\tLexeme: %s\n", tokBuf);
							printToken(outputPtr, "IDENTIFIER", tokBuf);
							return T_ID;
						}
					}
					else if (currentState == INT_END) {
            printf("\nToken: Integer\tLexeme: %s\n", tokBuf);
						printToken(outputPtr, "INTEGER", tokBuf);
						return T_NUM;
					}
					else if (currentState == REAL_END) {
            printf("\nToken: Real Number\tLexeme: %s\n", tokBuf);
						printToken(outputPtr, "REAL NUMBER", tokBuf);
						return T_NUM;
					}
					else if (currentState == END_COMMENT) {
            printf("\nToken: Comment\tLexeme: %s\n", tokBuf);
						printToken(outputPtr, "COMMENT", tokBuf);
						return T_EMPTY;
          }
      }
			else {
				ch = buffer[++buf_i];
				nextChar = ch;
			}

    } // while bracket close
  } // else bracket close
} // function bracket close
