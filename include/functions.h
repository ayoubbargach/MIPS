/**
 * @file functions.h
 * @author François Portet <francois.portet@imag.fr>
 * @brief Collections management.
 */

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_


/* Each function is detailed in functions.c */


chain make_collection( void );
chain add_chain_next( chain, int );
chain add_chain_newline( chain, int );

digit make_digit( void );
int OctalToDecimal(int octalNumber);
lex make_lex( unsigned int, char *, int );
void add_lex( chain , lex );


lex read_lex( chain );
chain read_line( chain );
chain next_line( chain );


/* Free functions */

void del_lex( lex );
void del_chain( chain );

/* Hash function */

int hash( char *, int );

/* String function */
void majuscule(char *);


#endif /* _FUNCTIONS_H */
