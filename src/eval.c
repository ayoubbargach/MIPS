
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


/**
 * @param l lexeme to eval
 * @return An int that results from eval.
 * @brief This routine analyze the lexeme.
 *
 */
 
 unsigned int eval( lex l ) {
 
 	/* We start by analysing lexeme type, in this function, we only treat digits */
 	switch (l->type) {
 		case DECIMAL_ZERO :
 		case DECIMAL :
 		case OCTO :
 		case HEXA :
 			/* Lexeme is a digit */
 			return l->this.digit->value;
 		break;
 		
 		case SYMBOL :
 			/* If we are in symbol case : try to find symbol in symTab. If found, add a complete relocation. If not, add an entry in symTab and another in relocation table. */
 		break;
 		
 		default:
 			ERROR_MSG("Decode error : failed while trying to decode an immediate value.");
 		break;
 	}
 	
 	return 0;
 }
