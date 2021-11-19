#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

void F();
void TP();
void T();
void EP();
void E();
void A();
void S();
void parser();

enum Symbols lexer(char ch) {
	switch(ch) {
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

void openFile(char *fileName) {
	fp = fopen(fileName, "r");
	if (fp == NULL) {
		printf("Could not open file.\n");
		exit(1);
	} else { printf("Opened %s successfully.\n\n", fileName); }
}

void closeFile() { // closes both input and output files
	fclose(fp);
}

/******************************************** GRAMMAR RULES ********************************************
1ST ITERATION:
S -> A                    Statement       ->  Assign
A -> id = E;              Assign          ->  id = Expression;
E -> TE'                  Expression      ->  Term ExpressionPrime
E' -> +TE' | -TE' | e     ExpressionPrime ->  + Term ExpressionPrime | - Term ExpressionPrime | empty
T -> FT'                  Term            ->  Factor TermPrime
T' -> *FT' | /FT' | e     TermPrime       ->  * Factor TermPrime | / Factor TermPrime | empty
F -> ( E ) | id           Factor          ->  ( Expression ) | id
2ND ITERATION:
S -> D                    Statement       ->  Declarative
D -> T id Mid             Declarative     ->  Type id MoreIds
T -> int | float | bool   Type            ->  int | float | bool
Mid -> , id Mid           MoreIds         ->  , id MoreIds
********************************************************************************************************/


bool whiteSpace(char ch) {
	int match;
  switch (ch) {
    case ' ': case '\t': case '\n': match = 1; break;
    default: match = 0; break;
  }
	return match;
}
void next() {
	nextChar = buffer[buf_i++];
	if(whiteSpace(nextChar)) { // compiler ignores whitespaces between lexemes
		while((whiteSpace(nextChar))) {
			nextChar = buffer[buf_i++];
		}
	}
}

void match(char ch) {
	if (ch == nextChar) { next(); } // if matches, go to next
	else {
		printf("\nError matching rule \n\n");
		printRule(outputPtr, "\nError matching rule \n");
		exit(2);
	}
}

// F -> ( E ) | id
void F() {
	if (nextChar == '(') {
		nextChar = lexer_main(nextChar);
		match(T_L_PARENS);
		E(); 								// need to implement E procedure
		match(T_R_PARENS);
		printf("F -> ( E ) | id\n");
		printRule(outputPtr, "F -> ( E ) | id");
	}
	else if(isalpha(nextChar) > 0) {
		nextChar = lexer_main(nextChar);
		match(T_ID);
		printf("F -> ( E ) | id\n");
		printRule(outputPtr, "F -> ( E ) | id");
	}
	else {
		printf("\nSyntax Error. Expected token in First F.");
		printRule(outputPtr, "\nSyntax Error. Expected token in First F.");
	}
}

// T' -> *FT' | /FT' | e
void TP() { //TPrime
	if (nextChar == '*') {
		nextChar = lexer_main(nextChar);
		printf("T' -> *FT' | /FT' | e\n");
		printRule(outputPtr, "T' -> *FT' | /FT' | e");
		match(T_MULTI);
		F();
		TP();
	}
	else if (nextChar == '/') {
		nextChar = lexer_main(nextChar);
		printf("T' -> *FT' | /FT' | e\n");
		printRule(outputPtr, "T' -> *FT' | /FT' | e");
		match(T_DIV);
		F();
		TP();
	}
	else if(nextChar == ';' || nextChar == '\0'|| whiteSpace(nextChar)) {
		nextChar = lexer_main(nextChar);
		printf("T' -> *FT' | /FT' | e\n");
		printRule(outputPtr, "T' -> *FT' | /FT' | e");
		match(T_EMPTY);
	}
	else {
		printf("\nSyntax Error. Expected token in First T'.\n");
		printRule(outputPtr, "\nSyntax Error. Expected token in First T'.");
	}
}

//T -> FT'
void T() {
	if(nextChar == '(') {
		nextChar = lexer_main(nextChar);
		printf("T -> FT'\n");
		printRule(outputPtr, "T -> FT'");
		match(T_L_PARENS);
		TP();
	}
	else if(isalpha(nextChar) > 0) {
		nextChar = lexer_main(nextChar);
		printf("T -> FT'\n");
		printRule(outputPtr, "T -> FT'");
		match(T_ID);
		TP();
	}
	else {
		printf("\nSyntax Error. Expected token in First T.\n");
		printRule(outputPtr, "\nSyntax Error. Expected token in First T.");
	}
}

//E' -> +TE' | -TE' | e
void EP() {
	if(nextChar == '+') {
		nextChar = lexer_main(nextChar);
		printf("E' -> +TE' | -TE' | e\n");
		printRule(outputPtr, "E' -> +TE' | -TE' | e");
		match(T_PLUS);
		T();
		EP();
	}
	else if(nextChar == '-') {
		nextChar = lexer_main(nextChar);
		printf("E' -> +TE' | -TE' | e\n");
		printRule(outputPtr, "E' -> +TE' | -TE' | e");
		match(T_MINUS);
		T();
		EP();
	}
	else if(nextChar == ';' || nextChar == '\0' || whiteSpace(nextChar)) {
		nextChar = lexer(nextChar);
		printf("E' -> +TE' | -TE' | e\n");
		printRule(outputPtr, "E' -> +TE' | -TE' | e");
		match(T_EMPTY);
	}
	else {
		printf("\nSyntax Error. Expected token in First E'.\n");
		printRule(outputPtr, "\nSyntax Error. Expected token in First E'.");
	}
}

//E -> TE'
void E() {
	if(nextChar == '(') {
		nextChar = lexer_main(nextChar);
		printf("E -> TE'\n");
		printRule(outputPtr, "E -> TE'");
		match(T_L_PARENS);
		EP();
	}
	else if(isalpha(nextChar) > 0) {
		nextChar = lexer_main(nextChar);
		printf("E -> TE'\n");
		printRule(outputPtr, "E -> TE'");
		match(T_ID);
		EP();
	}
	else {
		printf("\nSyntax Error. Expected token in First E.\n");
		printRule(outputPtr, "\nSyntax Error. Expected token in First E.");
	}
}

//A -> id = E;
void A() {
	if(isalpha(nextChar) > 0) {
		printf("A -> id = E\n");
		printRule(outputPtr, "A -> id = E");
		next();
		if (nextChar == '=') {
			lexer_main(nextChar);
			match('=');
			E();
		}
		else {
			printf("\nSyntax Error. Mismatch in Rule A.\n");
			printRule(outputPtr, "\nSyntax Error. Mismatch in Rule A.");
		}
	}
	else {
		printf("\nSyntax Error. Expected token in First A.\n");
		printRule(outputPtr, "\nSyntax Error. Expected token in First A.");
	}
}

//S -> A
void S() {
	if(isalpha(nextChar) > 0 || whiteSpace(nextChar)) {
		nextChar = lexer_main(nextChar);
		printf("S -> A\n");
		printRule(outputPtr, "S -> A");
		match(T_ID);
		A();
	}
	else {
		printf("\nSyntax Error. Expected token in First S.\n");
		printRule(outputPtr, "\nSyntax Error. Expected token in First S.");
	}
}

// Mid -> , id Mid
void Mid() {
	if(nextChar == ',') {
		nextChar = lexer(nextChar);
		printf("Mid -> , id Mid\n");
		match(T_COMMA);
		if (isalpha(nextChar) > 0) {
			nextChar = lexer(nextChar);
			printf("Mid -> , id Mid\n");
			match(T_ID);
			Mid();
		} else { printf("\nSyntax Error. Mismatch in Rule A.\n"); }
	}
	else { printf("\nSyntax Error. Expected token in First Mid.\n"); }
}

//Type -> int | float | bool
void Type() {
	if(isalpha(nextChar) > 0) {
		nextChar = lexer(nextChar);
		printf("Type -> int | float | bool\n");
	}
	/* if(isalpha(nextChar) > 0) {
		 	nextChar = lexer_main(nextChar) will increment parser's buffer[i++] for every state transitioned
					and will print whole keyword
		 		if(nextChar == T_KEYWORD) {
					printf("Type -> int | float | bool\n");
		 		}
		}
		else { printf("Syntax Error. Expected token in First Type.\n"); }
	*/
}

void parser() {
	S();
}

// S -> D
// D -> T id Mid


int main (int argc, char *argv[]) {
	char ch;
	int i = 0;
	outputPtr = fopen("output.txt", "w"); // open output file with write permissions

	printf("Run the program with no command line arguments in order to parse a\n");
	printf("string via stdin. Otherwise, run the program with a source code\n");
  printf("filename as argv[1] to parse through the input source code file.\n\n");

	if(argc == 1) {                         // run ./test with no args to
		printf("Enter a string: ");           // parse user input via stdin
		fgets(buffer, sizeof(buffer), stdin);
  	buffer[strcspn(buffer, "\n")] = '\0';
	}
	else {
		openFile(argv[1]);                   // else input source code filename
		while ((ch = fgetc(fp)) != EOF) {     // to parse as command line argv[1]
				buffer[i++] = ch;
	  }
		printf("buffer: %s", buffer);
	}

	nextChar = buffer[0];
	//printf("nextChar = %c\n", nextChar);

	parser();

	if (nextChar == '\0') {
		printf("\nParsing completed successfully.\n");
	} else { printf("Syntax Error. Parsing not completed. \n"); }

	printf("\nOutput recorded in output.txt.\n");

	if (argc != 1) {
		closeFile();
	}

	fclose(outputPtr);
  return 0;
}
