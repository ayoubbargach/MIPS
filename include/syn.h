/**
 * @file notify.h
 * @author François Cayre <cayre@yiking.(null)>
 * @date Thu Mar 22 17:22:55 2012
 * @brief Syntactic analysis.
 *
 * Used for the syntactic analysis.
 */
 
 #ifndef _SYN_H_
#define _SYN_H_


#include <stdio.h>
#include <global.h>

/* For the chain structure, we use the same structure "chain" */

void instructionSet( inst* );
inst makeInst( char*, char*, char*, char*, char*  );

unsigned int decodeInstruction( chain, inst *);
unsigned int decodeDirective( chain );

char* registerToBinary( char * );

void fetch( chain, chain, chain, int*, int*, inst *  ); 

void addSymbol( int, int, chain, chain );


#endif /* _SYN_H_ */
