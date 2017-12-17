
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
 				addSymbol( l->this.value , symTab, 0);
 				sym = findSymbol( l->this.value, symTab );
 			}
 			
 			
 			addRel( r, typeRel, l->this.value, sym);
 			
 			
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

void solve( chain symTab, chain chCode, chain chRel ) {
	rel r;
	chain lastR = chRel;
	code c;
	symbol sym;
	
	chRel = read_next( chRel );
	chCode = read_next( chCode );
	
	while (chRel != NULL){
		r = readRel( chRel );
		
		
		c = findCode( chCode, r->addr );
		sym = r->sym;
		
		if( c != NULL ) {
		
			/* The symbol is already linked into the relocation structure ! There is any more to do except update the code. */
			
			switch (r->type) {
				case R_MIPS_32 :
					c->value = sym->addr;
					lastR = chRel;
				break;
		
				case R_MIPS_26 :
					c->value = c->value + ((sym->addr) >> 2);
				break;
		
				case R_MIPS_HI16 :
					c->value = c->value + ((sym->addr) >> 16);
				break;
		
				case R_MIPS_LO16 :
					c->value = c->value + (((sym->addr) << 16) >> 16);
				break;
		
				case RELATIVE : /* For relative relocations we delete theme from the rel chain ! */
					c->value = c->value + (((((sym->addr - c->addr) << 16) >> 16) >> 2) - 1);
					lastR->next = read_next(chRel); /* We eat this one */
				break;
		
				default :
				break;
			}
			
			switch (r->type) {
				case RELATIVE : /* Do nothing for this one */
				break;
		
				default :
					lastR = chRel;
				break;
			}
		}
		else {
			ERROR_MSG("Internal error : A relocation failed due to unfindable code. Please contact devs.");
		}
		
		chRel = read_next(chRel);
	}
	return;
}
 
 
/* ##### Symbol functions ##### */


/**
 * @param value String value of symbol
 * @param symTab Table to complete
 * @param section Section identified
 * @param addr Address in section
 * @param label Boolean that explicit if we work with a label.
 * @return nothing
 * @brief Add the symbol to the table of symbols. Two path for resolution :
 * - if section is not defined, just add
 * - if section defined, add section and addr 
 */

void addSymbol( char * value, chain symTab, int label ) {
	chain element = symTab;
	chain foundElement = symTab;
	chain lastElement = symTab;
	symbol temp;
	
	
	/* If we are managing a label, we search if there is any previous entry to updae it. If not, add a new one. */
	if (label) {
	
		if ( section != UNDEFINED ) { /* In case of no section/addr filled, we just add the symbol to the tab */
	
			temp = findSymbol( value, symTab );
		
			/* If we have a match we update the symbol by putting it in the good place : */
			if (temp != NULL) {
				/* We start by updating symbol */
				temp->section = section;
				temp->addr = addr;
				temp->line = line;
				
				/* After that, we put the symbol in the right place */ 
				while ( element != NULL && element->this.sym != temp ) {
					lastElement = element;
					element = read_next( element );
				}
				
				foundElement = element;
				
				/* pull it from the chain */
				lastElement->next = read_next( foundElement );
				
				element = symTab;
				
				while ( element != NULL && element->line < line ) {
					lastElement = element;
					element = read_next( element );
				}
				
				lastElement->next = foundElement;
				foundElement->next = element;
				
			
			}
			else {
				/* Create a new element of the chain and add the symbol */
				while ( read_next(element)  != NULL ) 
					element = read_next(element);
				
				element = add_chain_next( element );
				element->this.sym = createSymbol( value, 1 );
			
			}
		}
		else {
			ERROR_MSG("Decode error : no section defined yet, symbol can not be added");
		}
	}
	else {
		/* In this case, we only need to add a simple entry */
		temp = findSymbol( value, symTab );
		
		/* If we have a match we update the symbol : */
		if (temp == NULL) {
			/* Create a new element of the chain and add the symbol */
			while ( read_next(element)  != NULL ) 
				element = read_next(element);
			
			element = add_chain_next( element );
			
			element->this.sym = createSymbol( value, 0 );
		
		}
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
 * @param label Boolean that explicit if we work with a label.
 * @return symbol.
 * @brief Create a symbol.
 */

symbol createSymbol(char * value, int label ) {
	symbol sym = malloc ( sizeof( *sym ) );
	
	strcpy(sym->value , value);
	
	if (label) {
		sym->section = section;
		sym->addr = addr;
	}
	else {
		sym->section = UNDEFINED;
		sym->addr = 0;
	}
	
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

rel createRel(int type, char * value, symbol sym ) {
	rel r = malloc ( sizeof( *r ) );
	
	/* Error Management */
	if (r == NULL) {
		ERROR_MSG("Memory error : Malloc failed");
    }
    
    r->section = section;
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

void addRel( chain * r, int type, char * value, symbol sym  ) {
	/* We add a new element in the chain code */
	*r = add_chain_next( *r );
	
	(*r)->this.r = createRel( type, value, sym);
	
	
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
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
