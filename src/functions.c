
/**
 * @file functions.c
 * @author Ayoub Bargach <ayoub.bargach@phelma.grenoble-inp.fr>
 * @brief Some functions to manage collections ...
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include <global.h>
#include <notify.h>
#include <functions.h>


/* ##### chain functions ##### */
 
/**
 * @return The first chain of a collection.
 * @brief Make a collection of chain elements. Each chain contain "chain list of lexemes".
 * Only useful lines are took into account. This means that we remove comments and commas from the list of lexemes.
 * The different kinds of lexemes are indexed in enum in global.h
 *
 */
chain make_collection( void ) {
	chain ch = malloc( sizeof( *ch ));

	/* Error Management */
	if (ch == NULL) {
		ERROR_MSG("Memory error : Malloc failed.");
    }

	/* Init */
	ch->line = 0;
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
chain add_chain_next( chain parent ) {
	chain ch = malloc( sizeof( *ch ));

	/* Error Management */
	if (ch == NULL) {
		ERROR_MSG("Memory error : Malloc failed.");
    }
    

	/* Init */
	parent->next = ch;
	ch->line = line;
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
 
chain add_chain_bottom( chain parent ) {
	chain ch = malloc( sizeof( *ch ));

	/* Error Management */
	if (ch == NULL) {
		ERROR_MSG("Memory error : Malloc failed.");
    }
    

	/* Init */

	parent->this.bottom = ch;
	ch->line = line;
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
 * @param octalNumber From atoi function
 * @return The converted octal number
 * @brief This function convert an octal number to an int. An octal written as int type is smaller than an int.
 * No Segfault will occur.
 *
 */

int OctalToDecimal(int octalNumber)
{
    int decimalNumber = 0, i = 0;

    while(octalNumber != 0)
    {
        decimalNumber += (octalNumber%10) << (3*i);
        ++i;
        octalNumber/=10;
    }

    i = 1;

    return decimalNumber;
}

 
/**
 * @param type Explicit type of lexeme.
 * @return The lex structure ( a pointer !)
 * @brief Useful to create fastly a lexeme of a kind of type. All the type are explicited in global.h by enum.
 * If there is an immediate value, it will be directly evaluated.
 *
 */

lex make_lex( unsigned int type, char * value, int sign ) {
	lex l = malloc( sizeof( *l ) );
	
	/* Error Management */
	if (l == NULL) {
		ERROR_MSG("Memory error : Malloc failed.");
    }
    
	l->type = type;
	
	if (sign == SIGNED) { /* We eat '-', but not mandatory because strtol manages '-' in fact */
		value++;
	}
	
	
	digit dig = make_digit();

	switch (type) {
		case DECIMAL_ZERO:
			dig->type = DECIMAL_ZERO;
			dig->sign = sign;
			dig->this.integer = 0;
			
			dig->value = 0;
			
			l->this.digit = dig;
			
			break; 
			
		case BIT:
			dig->type = BIT;
			dig->sign = sign;
			dig->this.integer = binaryToInt( value );
			
			if( sign == SIGNED )
				dig->value = ~ dig->this.integer + 1; /* NOT(a) + 1 // Complement of 2 */
			else dig->value = dig->this.integer;
			
			l->this.digit = dig;
			
			break; 
			
		case DECIMAL:
			dig->type = DECIMAL;
			dig->sign = sign;
			dig->this.integer = atoi( value );
			
			if( sign == SIGNED )
				dig->value = ~ dig->this.integer + 1; /* NOT(a) + 1 // Complement of 2 */
			else dig->value = dig->this.integer;
			
			l->this.digit = dig;
			
			break;  
				  			
		case OCTO:
			dig->type = OCTO;
			dig->sign = sign;
			dig->this.octo = atoi( value );
			
			if( sign == SIGNED )
				dig->value = ~ OctalToDecimal(dig->this.integer) + 1; /* NOT(a) + 1 // Complement of 2 */
			else dig->value = OctalToDecimal(dig->this.integer);
			
			l->this.digit = dig;
			
			break; 
				  			
		case HEXA:
			dig->type = HEXA;
			dig->sign = sign;
			dig->this.hexa = value;
			
			if( sign == SIGNED )
				dig->value = ~ (int)strtol(value, NULL, 0) + 1; /* NOT(a) + 1 // Complement of 2 */
			else dig->value = (int)strtol(value, NULL, 0);
			
			l->this.digit = dig;
			
			break;
			
		case REGISTER:
			dig->type = REGISTER;
			dig->sign = sign;
			
			dig->value = registerToInt(value);
			
			
			l->this.digit = dig;
			
			break;
				 			
		default :
			strncpy ( l->this.value, value, sizeof(l->this.value) );
			break;
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

/**
 * @return unsigned int to be construct to build the decode code.
 * @brief this routine convert a char of a specified register to his int equivalent.
 *
 */

unsigned int registerToInt( char *input ) {
	
	unsigned int t = 0;
	
	if ( input[0] == '$' ) {
		if ( !strcmp( input+1, "zero" ) ) {
			t = 0;
		}
		else if ( !strcmp( input+1, "at" ) ) {
			t = 1;
		}
		else if ( !strcmp( input+1, "gp" ) ) {
			t = 28;
		}
		else if ( !strcmp( input+1, "sp" ) ) {
			t = 29;
		}
		else if ( !strcmp( input+1, "fp" ) ) {
			t = 30;
		}
		else if ( !strcmp( input+1, "ra" ) ) {
			t = 31;
		}
		else {
			switch (input[1]) {
				case 'v':
					t = atoi(input+2) + 2;
				break;
	
				case 'a':
					t = atoi(input+2) + 4;
				break;
	
				case 't':
					if (atoi(input+2) < 8) t = atoi(input+2) + 8;
					else t = atoi(input+2) + 16;
			
				break;
	
				case 's':
					t = atoi(input+2) + 16;
				break;
	
				case 'k':
					t = atoi(input+2) + 16;
				break;
	
				default:
					t = atoi( input+1);
				break;
			}
		}
		
	}
	else {
		t = binaryToInt( input );
	}
	
	
	if (t > 31) {
		ERROR_MSG("Decode error : one register is badly written. Please check source code or instSet.txt");
	}
	
	return t;
	
}

/**
 * @param s The binary string to analyse
 * @return an unsigned int as a result
 * @brief Convert a binary string to an unsigned integer
 *
 */


unsigned int binaryToInt( char* start ) {
	unsigned int total = 0;
	
	while (*start)
	{
		total <<= 1;
   		if (*start++ == '1') total^=1;
	}
	
	return total;
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
 
chain read_next( chain parent ) {
	
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
 
chain read_bottom( chain parent ) {
	
	if( parent->this.bottom != NULL ) {
		return parent->this.bottom;
	}
	
	return NULL ;
	
}

/**
 * @return An int
 * @brief Hash function used to generate a number knowing a string.
 *
 */
int hash( char * s, int modulo ) {
	int i =0;
	int result = 0;
	
	while ( s[i] != '\0' ) {
		result = result + ( s[i] << i);
		i++;
	}
	
	return result % modulo;
}

/**
 * @return A string with upper chars
 * @brief Is used to recognize some symbols. Example : addi -> ADDI. Took from openclassroom forum.
 *
 */

void majuscule(char *chaine)
{
    int i = 0;
    for (i = 0 ; chaine[i] != '\0' ; i++)
    {
        chaine[i] = toupper(chaine[i]);
    }
}


 
 
 
 
 
 
 
 
 
 
 
 
 

