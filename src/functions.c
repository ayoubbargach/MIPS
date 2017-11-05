
/**
 * @file functions.c
 * @author François Portet <francois.portet@imag.fr> from François Cayre
 * @brief Some functions to manage collections ...
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <global.h>
#include <notify.h>
#include <functions.h>


/**
 * @param 
 * @return
 * @brief
 *
 */

/* ##### chain functions ##### */
 
/**
 * @return The first chain of a collection.
 * @brief Make a collection of chain elements. Each chain contain "chain list of lexemes".
 * Only useful lines are took into account. This means that we remove comments and commas from the list of lexemes.
 * The different kinds of lexemes are indexed in enum in global.h
 *
 */
 
chain make_collection ( void ) {
	chain ch = malloc( sizeof( *ch ));

	/* Error Management */
	if (ch == NULL) {
		ERROR_MSG("Memory error : Malloc failed.");
    }

	/* Init */

	ch->next = NULL;
	ch->this.bottom = NULL;

	return ch;
}
 
/**
 * @param parent The parent element. 
 * @return The next element of the chain.
 * @brief This is used to create a list of chain elements in order to use it to organize lexemes.
 * For each element we add using this function, we are managing to add a lexeme. (It symbolize a line)
 *
 */
 
chain add_chain_next( chain parent, int nline ) {
	chain ch = malloc( sizeof( *ch ));

	/* Error Management */
	if (ch == NULL) {
		ERROR_MSG("Memory error : Malloc failed.");
    }
    

	/* Init */

	parent->next = ch;
	ch->line = nline;
	ch->next = NULL;
	ch->this.bottom = NULL;

	return ch;
}

/**
 * @param parent The parent element. 
 * @return The bottom element of the chain.
 * @brief This is used to add a new line to the chain.
 *
 */
 
chain add_chain_newline( chain parent, int nline ) {
	chain ch = malloc( sizeof( *ch ));

	/* Error Management */
	if (ch == NULL) {
		ERROR_MSG("Memory error : Malloc failed.");
    }
    

	/* Init */

	parent->this.bottom = ch;
	ch->line = nline;
	ch->next = NULL;
	ch->this.bottom = NULL;

	return ch;
}






/* ##### LEX functions ##### */

/**
 * @return A digit pointer.
 * @brief Simple function to make a digit.
 *
 */
digit make_digit( void ) {
	digit dig = malloc( sizeof( *dig ) );
	
	/* Error Management */
	if (dig == NULL) {
		ERROR_MSG("Memory error : Malloc failed.");
    }
    
	return dig;
}
 
/**
 * @param type Explicit type of lexeme.
 * @return The lex structure ( a pointer !)
 * @brief Useful to create fastly a lexeme of a kind of type. All the type are explicited in global.h by enum.
 * After that, we fill the structure with the value of lexeme.
 *
 */

lex make_lex( unsigned int type, char * value, int sign ) {
	lex l = malloc( sizeof( *l ) );
	
	/* Error Management */
	if (l == NULL) {
		ERROR_MSG("Memory error : Malloc failed.");
    }
    
	l->type = type;
	
	if ( type == DECIMAL && type == OCTO && type == HEXA ) {
		digit dig = make_digit();
	
		switch (type) {
				case DECIMAL:
					dig->type = DECIMAL;
					dig->sign = sign;
					dig->this.integer = atoi( value );
					
					l->this.digit = dig;
					
					break;  
						  			
				case OCTO:
					dig->type = OCTO;
					dig->sign = sign;
					dig->this.octo = atoi( value );
					
					l->this.digit = dig;
					
					break; 
						  			
				case HEXA:
					dig->type = OCTO;
					dig->sign = sign;
					dig->this.hexa = value;
					
					l->this.digit = dig;
					
					break;
						 			
				default :
					break;
		}
	}
	else {
		strncpy ( l->this.value, value, sizeof(l->this.value) );
	}
	
	return l;
}

/**
 * @param chain Chain to link.
 * @param lex Lexeme to add.
 * @return Nothing
 * @brief Add a lexeme to the bottom of a chain element.
 *
 */
 
void add_lex( chain parent, lex l ) {
	parent->this.bottom_lex = l;
	
	return;
}



/* ##### Chain management functions ##### */

/**
 * @param chain to print.
 * @return the lexeme contained in the chain element, if any error, returns NULL.
 * @brief Is use to read the lexeme contained in a chain element.
 *
 */
 
lex read_lex( chain parent ) {
	
	/* We start by verifying if we are in the case of a lexeme : */
	if ( parent->this.bottom_lex != NULL ) {
		return parent->this.bottom_lex;
	}
	
	return NULL ;

}

/**
 * @param chain Chain to read.
 * @return Give next chain to read. Returns NULL if the chain is completed.
 * @brief Read trough a given chain line. Each time it used, it gives the next chain in the same line.
 *
 */
 
chain read_line( chain parent ) {
	
	if( parent->next != NULL ) {
		return parent->next;
	}
	
	return NULL ;
	
}

/**
 * @param chain Chain to read.
 * @return Give next line to read. Returns NULL if the chain is completed OR if the chain is connected to a lexeme intead of another chain.
 * @brief Keep in mind the structure, with this function, you can load the next line.
 *
 */
 
chain next_line( chain parent ) {
	
	if( parent->next != NULL ) {
		return parent->this.bottom;
	}
	
	return NULL ;
	
}




 
 
 
 
 
 
 
 
 
 
 
 
 

