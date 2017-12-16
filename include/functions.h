/**
 * @file functions.h
 * @author Ayoub Bargach <ayoub.bargach@phelma.grenoble-inp.fr>
 * @brief Collection management.
 */

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_


/* Each function is detailed in functions.c */


chain make_collection( void );
chain add_chain_next( chain );
chain add_chain_bottom( chain );

digit make_digit( void );
int OctalToDecimal(int octalNumber);
lex make_lex( unsigned int, char *, int );
void add_lex( chain , lex );
unsigned int registerToInt( char * );
unsigned int binaryToInt( char* s );


lex read_lex( chain );
chain read_next( chain );
chain read_bottom( chain );

/* Hash function */
int hash( char *, int );

/* String function */
void majuscule(char *);

/* Free functions */
void del_lex( lex );
void del_chain( chain );


#endif /* _FUNCTIONS_H */



