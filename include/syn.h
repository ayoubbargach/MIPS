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

void decodeInstruction( chain ** , inst *);
chain get_special( char *, chain, chain );
void decodeDirective( chain ** );

void fetch( chain**, inst * );
lex get_lex( chain * );

void addCode( chain *, unsigned int );
code createCode(unsigned int, unsigned int );
code getCode( chain );

#endif /* _SYN_H_ */


