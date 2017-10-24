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
void decodeDirective( chain, char * );

char* registerToBinary( char * );

/* enum sections */
enum {UNDEFINED, TEXT, DATA, BSS};

void fetch( chain, chain, chain, int*, unsigned int*, inst *  ); 

void addSymbol( char * , chain, int  , unsigned int  );
symbol findSymbol( char * , chain);
symbol readSymbol( chain );
symbol createSymbol(char * ,  int, unsigned int );

code createCode(unsigned int, unsigned int, unsigned int );
void addCode( chain, unsigned int, unsigned int, unsigned int  );


#endif /* _SYN_H_ */
