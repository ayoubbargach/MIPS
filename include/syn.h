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

int decodeInstruction( chain, inst *);
int decodeDirective( chain );

void addSymbol( chain, chain );

char* registerToBinary( char * );


#endif /* _SYN_H_ */
