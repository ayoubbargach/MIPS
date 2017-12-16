/**
 * @file notify.h
 * @author François Cayre <cayre@yiking.(null)>
 * @date Thu Mar 22 17:22:55 2012
 * @brief Syntactic analysis.
 *
 * Used for the instruction analysis.
 */
 
 #ifndef _INST_H_
#define _INST_H_

#include <stdio.h>
#include <global.h>

void instructionSet( inst* );
inst makeInst( char*, char*, char*, char*, char*  );

#endif /* _INST_H_ */
