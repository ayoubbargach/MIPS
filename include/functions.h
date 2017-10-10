/**
 * @file functions.h
 * @author François Portet <francois.portet@imag.fr>
 * @brief Collections management.
 */

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

/* Each function is detailed in functions.c */


chain make_collection( void );
chain add_chain_next( chain );
chain add_chain_newline( chain );

lex make_lex( unsigned int, char * );
void add_lex( chain , lex );

lex read_lex( chain );


/* Free functions */

void del_lex( lex );
void del_chain( chain );

#endif /* _FUNCTIONS_H */