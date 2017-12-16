/**
 * @file eval.h
 * @author François Cayre <cayre@yiking.(null)>
 * @date Thu Mar 22 17:22:55 2012
 * @brief Evaluation.
 *
 * Used for evaluation analysis.
 */
 
 #ifndef _EVAL_H_
#define _EVAL_H_


#include <stdio.h>
#include <global.h>

unsigned int eval( lex, int, chain *, chain );
void solve( chain symTab, chain chCode, chain r );

void addSymbol( char * , chain );
symbol findSymbol( char * , chain );
symbol readSymbol( chain );
symbol createSymbol(char * );

rel createRel( unsigned int addr, int type, char * value, symbol sym );
void addRel( chain r,  unsigned int addr, int type, char * value, symbol sym, unsigned int *nline );
rel readRel( chain );



#endif /* _EVAL_H_ */
