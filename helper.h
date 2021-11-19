//  helper.h
//  Compilers_2
//
//  Created by Sam
//  Copyright © 2020 Sam. All rights reserved.
//

#ifndef helper_h
#define helper_h

#define BUFSIZE 100
FILE *fp;
FILE *output;
char cache[BUFSIZE];
int cache_i = 0;
char nextchar;
enum N{
NT_S,       // Statement
NT_E,       // Expression
NT_A,       // Assignment
NT_T,      // Term
NT_F,      // Factor
T_PLUS,     // +
T_MINUS,    // -
T_MULTI,   // *
T_DIV,    // /
T_L_PARENS, // (
T_R_PARENS, // )
T_ID,       // identifier
T_NUM,      // integer number
T_EMPTY,    // empty (epsilon)
T_COMMA,    // ,
T_KEYWORD // int, float, bool, char
};
#endif /* helper_h */
