
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
#include <functions.h>

#include <eval.h>
#include <syn.h>

/**
 * @param l lexeme to eval
 * @return An int that results from eval.
 * @brief This routine analyze the lexeme.
 *
 */
 
unsigned int eval( lex l, int typeRel, chain * r, chain symTab) {
 	
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
 				addSymbol( l->this.value , symTab);
 				/* addRel( r, addr, type, l->this.value, TODO Need some change in addsymbol function */
 			}
 			else {
 				addRel(* r, addr, typeRel, l->this.value, sym, &line);
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
 
 
/* ##### Symbol functions ##### */


/**
 * @param value String value of symbol
 * @param symTab Table to complete
 * @param section Section identified
 * @param addr Address in section
 * @return nothing
 * @brief Add the symbol to the table of symbols. Two path for resolution :
 * - if section is not defined, just add
 * - if section defined, add section and addr 
 */

void addSymbol( char * value, chain symTab ) {
	chain element = symTab;
	symbol temp;
	
	
	if ( section != UNDEFINED ) { /* In case of no section/addr filled, we just add the symbol to the tab */
	
		temp = findSymbol( value, symTab );
		
		/* If we have a match we update the symbol : */
		if (temp != NULL) {
			temp->section = section;
			temp->addr = addr;
			
		}
		else {
			/* Create a new element of the chain and add the symbol */
			while ( read_next(element)  != NULL ) 
				element = read_next(element);
				
			element = add_chain_next( element );
			element->this.sym = createSymbol( value );
			
		}
	}
	else {
		ERROR_MSG("Decode error : no section defined yet, symbol can not be added");
	}
	
	return;
}

/**
 * @param value String value of symbol
 * @param symTab Table of symbols
 * @return the symbol if found, NULL if not.
 * @brief Find a symbol. Usefull when an operand is decoded for instance.
 */

symbol findSymbol( char * value, chain symTab ) {
	chain element = symTab;
	symbol temp;
	
	do {
		temp = readSymbol( element );
		if ( temp != NULL && !strcmp( temp->value, value ) ) {
			return temp;
		}
	} while ( (element = read_next( element )) != NULL );
	
	return NULL;
}

/**
 * @param symTab Element of symTab.
 * @return readed symbol, NULL if nothing to read.
 * @brief Read a symbol from a chain element.
 */

symbol readSymbol( chain symElem ) {
	if ( symElem->this.sym != NULL ) {
		return symElem->this.sym;
	}
	return NULL;
}

/**
 * @param section Symbol section, if UNDEFINED, symbol not defined.
 * @param addr If section defined, this value have a meaning.
 * @param value Symbol value
 * @return symbol.
 * @brief Create a symbol.
 */

symbol createSymbol(char * value ) {
	symbol sym = malloc ( sizeof( *sym ) );
	
	strcpy(sym->value , value);
	sym->section = section;
	sym->addr = addr;
	sym->line = line;
	
	return sym;
}


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
	r = add_chain_next( r );
	
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
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
