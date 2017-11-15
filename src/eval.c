
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

/* ##### Relocation functions ##### */

/**
 * @param addr Address of relocation
 * @param type of relocation
 * @return
 * @brief 
 */

rel createRel( unsigned int addr, int type, char * value, symbol sym ) {
	rel r = malloc ( sizeof( *r ) );
	
	/* Error Management */
	if (r == NULL) {
		ERROR_MSG("Memory error : Malloc failed");
    }
    
    r->addr = addr;
    r->type = type;
    r->sym = sym;
    strcpy( r->value, value);
    
	return r;
}

/**
 * @param
 * @return
 * @brief 
 */

void addRel( chain r,  unsigned int addr, int type, char * value, symbol sym, unsigned int *nline  ) {
	/* We add a new element in the chain code */
	r = add_chain_next( r, *nline );
	
	r->this.r = createRel(addr, type, value, sym);
	
	
	return;
}

/**
 * @param
 * @return
 * @brief 
 */

rel readRel( chain r ) {
	if ( r->this.r != NULL ) {
		return r->this.r;
	}
	return NULL;
}


/**
 * @param l lexeme to eval
 * @return An int that results from eval.
 * @brief This routine analyze the lexeme.
 *
 */
 
 unsigned int eval( lex l, int type, chain r, chain symTab, int section, unsigned int addr, unsigned int *nline) {
 	
 	symbol sym;
 	
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
 			sym = findSymbol( l->this.value, symTab );
 			
 			if (sym == NULL) { /* If symbol is not yet defined */
 				addSymbol( l->this.value , symTab, section, addr, nline);
 				/* addRel( r, addr, type, l->this.value, TODO Need some change in addsymbol function */
 			}
 			else {
 				addRel( r, addr, type, l->this.value, sym, nline);
 			}
 			
 			
 		break;
 		
 		default:
 			ERROR_MSG("Decode error : failed while trying to decode an immediate value.");
 		break;
 	}
 	
 	return 0;
 }
 
 /**
 * @param 
 * @return Nothing.
 * @brief The aim is to solve in the end all the possible relocations. It means that sometimes, some LABELs will still be not defined. 
 * According to type, the relocation will be solved using the symbol table. It is mandatory to have a well working symTable.
 *
 */

void solve( chain symTab, chain chCode, chain r ) {
	while (r != NULL){
		switch (readRel(r)->type) {
			case R_MIPS_32 :
			break;
			
			case R_MIPS_26 :
			break;
			
			case R_MIPS_HI16 :
			break;
			
			case R_MIPS_LO16 :
			break;
			
			default :
			break;
		}
	}
	return;
}
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
