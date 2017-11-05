
/**
 * @file eval.c
 * @author François Portet <francois.portet@imag.fr>
 * @brief MIPS assembly evaluation.
 *
 * These routines perform the evalution of operands. It translate the register/signed value/hexa .. to a int value that can be used either by decodeInstruction function of decodeDirective.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <syn.h>
#include <functions.h>

/**
 * @param l lexeme to eval
 * @return An int that results from eval.
 * @brief This routine analyze the lexeme.
 *
 */
 
 int eval( lex l ) {
 	return 0;
 }
