
/**
 * @file lex.c
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexical analysis routines for MIPS assembly syntax.
 *
 * These routines perform the analysis of the lexeme of an assembly source code file.
 */

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <functions.h>

/**
 * @param line String of the line of source code to be analysed.
 * @param nline the line number in the source code.
 * @return should return the collection of lexemes that represent the input line of source code.
 * @brief This function performs lexical analysis of one standardized line.
 *
 */
void lex_read_line( char *line, int nline, chain newline) {

	int i;
	int sign;
	
	/* We first use newline as an initial affectation */
	chain element = add_chain_next( newline, nline );

	
	/* Useful when a token is defined as a comment, all the following tokens are also undertood as comments */
	int comment = 0;
	
	/* After standarizing, we only need ' ' as sep */
    char *seps = " ";
    char *token = NULL;
    char save[STRLEN];

    /* copy the input line so that we can do anything with it without impacting outside world*/
    memcpy( save, line, STRLEN );
    

    /* get each token */
    for( token = strtok( save, seps ); NULL != token; token = strtok( NULL, seps )) {
    	
 		/* Re-initialisation of FSM */
    	int state = INIT;
    	sign = UNSIGNED;
    	
        for ( i= 0; i< strlen(token); i++ ) {
        	
        	switch (state) {
        		case INIT :
        			if ( token[i] == '-' ) {
        				sign = SIGNED;
        			}
        			else if ( token[i] == '#' || comment) {
        				comment = 1;
        				state = COMMENT;
        			}
        			else if ( token[i] == '0' ) {
        				state = DECIMAL_ZERO;
        			}
        			else if ( isdigit(token[i]) ) {
        				state = DECIMAL;
        			}
        			else if ( token[i] == '.' ) {
        				state = DIRECTIVE;
        			}
        			else if ( token[i] == '$' ) {
        				state = REGISTER;
        			}
        			else if ( token[i] == ',' || token[i] == '(' || token[i] == ')' ) {
        				state = PUNCTUATION;
        			}
        			else {
        				state = SYMBOL;
        			}
        			
        			
        			
        			break;
        		
        		case DECIMAL_ZERO :
    				if ( token[i] < '8' && token[i] >= '0' ) {
    					state = OCTO;
    				}
    				else if ( token[i] == 'x' ) {
    					state = HEXA;
    				}
    				else if ( token[i] == '8' || token[i] == '9' ) {
    					state = DECIMAL;
    				}
    				else {
    					/* In all other cases, there is an error. */
    					state = ERROR;
    				}
        			
        			break;
        		
        		case DECIMAL :
					state = ( isdigit( token[i] ) ) ? DECIMAL : ERROR;
					break;  
					      			
        		case OCTO:
        			state = ( isdigit( token[i] ) && token[i] < '8' ) ? OCTO : ERROR;
					
					break; 
					      			
        		case HEXA:
        			state = ( isxdigit( token[i] ) ) ? HEXA : ERROR;
					
					break;
					
				
				/* All other states are managed by default : We do nothing ! */			
        		default :
					
					break;
        			
        		
        	
        	
        	}
        	
        	
        }
        
        /* verifying if the last character is ":" */
        if ( token[strlen(token)-1] == ':' && state == SYMBOL) {
        	state = LABEL;
        }
        
        
        
        /* We use the final value of state to determine the lexeme 
        DEBUG_MSG("[%s] %s", state_to_string(state), token); */
        
		/* /!\ If there is any error, we raise it and stop the program /!\ */
		
		if ( state == ERROR ) {
			ERROR_MSG("Lexical error. One of lexemes failed. Example : Be sure that you are not writing an hexa without 0x");
		}
        
        /* /!\ Punctuations and comments are not added to the collection (skip if) /!\ */
        
        if ( !( state == COMMENT || state == PUNCTUATION || state == INIT ) ) {
			/* Create lexeme and add value */
			lex lexeme = make_lex( state, token, sign );
		
			/* Add lexeme to the element */
			add_lex(element, lexeme);
		
			/* TEST Code
			if (read_lex(element) != NULL ) {
				WARNING_MSG("%s", state_to_string( read_lex(element)->type ) );
			} */
			
		
			/* Create a new chain element to store the next lexeme */
			element = add_chain_next( element, nline );
		}
        
    }

    return;
}

/**
 * @param file Assembly source code file name.
 * @param nlines Pointer to the number of lines in the file.
 * @return should return the collection of lexemes
 * @brief This function loads an assembly code from a file into memory.
 *
 */
void lex_load_file( char *file, unsigned int *nlines, chain ch ) {

    FILE        *fp   = NULL;
    char         line[STRLEN]; /* original source line */
    char         res[2*STRLEN]; /* standardised source line, can be longeur due to some possible added spaces*/



    fp = fopen( file, "r" );
    if ( NULL == fp ) {
        ERROR_MSG("Error while trying to open %s file --- Aborts",file);
    }

    *nlines = 0;
    
    
    chain newline = ch;

    while(!feof(fp)) {

        /*read source code line-by-line */
        if ( NULL != fgets( line, STRLEN-1, fp ) ) {

            line[strlen(line)-1] = '\0';  /* eat final '\n' */
            (*nlines)++;

            if ( 0 != strlen(line) ) {
                lex_standardise( line, res );
                lex_read_line( res, *nlines, newline );
            }
        }
        
        /* We add a newline in our collection, the condition helps to avoid possible "blank" lines in the collection */
        
        if (read_line( newline ) != NULL) {
    		newline = add_chain_newline( newline, *nlines );
    	}
    }
    
    

    fclose(fp);
    return;
}



/**
 * @param in Input line of source code (possibly very badly written).
 * @param out Line of source code in a suitable form for further analysis.
 * @return nothing
 * @brief This function will prepare a line of source code for further analysis.
 */

/* Note that MIPS assembly supports distinctions between lower and upper case */

void lex_standardise( char* in, char* out ) {

    unsigned int i = 0, j = 0, k = 0;
    
	/* Important note : k means that a space should be added. But the decision is taken by the next char. */
	
    while ( i < strlen(in) ) {

        /* translate all spaces (i.e., tab) into simple spaces, we use k to delete futher spaces */
		
		if (isblank((int) in[i])) {
		    k = 1;
        }
        else{
        	switch (in[i]) {
		    	case ',' :
		    		
		    		/* Purpose : Have a space before and after comma in all cases */
		    		 
		    		out[j] = ' ';
		    		out[j+1]=in[i];
		    		j += 2;
		    		
		    		k = 1;
		    	break;
		    	
		    	case '#' :
		    		
		    		/* Purpose : Have a space before and after # in all cases (Same as comma !) */
		    		 
		    		out[j] = ' ';
		    		out[j+1]=in[i];
		    		j += 2;
		    		
		    		k = 1;
		    	break;
		    	
		    	case ')' :
		    		
		    		/* Purpose : Have a space before and after # in all cases (Same as comma !) */
		    		 
		    		out[j] = ' ';
		    		out[j+1]=in[i];
		    		j += 2;
		    		
		    		k = 1;
		    	break;
		    	
		    	case '(' :
		    		
		    		/* Purpose : Have a space before and after # in all cases (Same as comma !) */
		    		 
		    		out[j] = ' ';
		    		out[j+1]=in[i];
		    		j += 2;
		    		
		    		k = 1;
		    	break;
		    	
		    	case ':' :
		    		
		    		/* Purpose : Have no space before and only one space after */
		    		
		    		out[j]=in[i];
		    		j++;
		    		
		    		k = 1;
		    	break;
		    	
		    	case '-' :
		    		/* Purpose : Have no space after and only one space before */
		    		
		    		out[j] = ' ';
		    		out[j+1]=in[i];
		    		j += 2;
		    		
		    		/* If the character after is blank, we increment i ! If not, we do nothing. */
		    		
		    		while ( isblank((int) in[i+1]) || i+1 == strlen(in) ) {
		    			i++;
		    		}
		    		
		    		k = 0;
		    		
		    	break;
		    	
		    	default :
					if (k) {
						out[j]=' ';
						out[j+1]=in[i];
						j = j + 2;
						k=0;
					}
					else {
						out[j]=in[i];
						j++;
					}
		    	break;
        	}
        }
        
        i++;
		
    }
    out[j]='\0';

	/* ---- TEST 1 ---- */

    /* To compare in and out : */
    
    if (testID == 1) {
		WARNING_MSG("In  : %s", in);
		WARNING_MSG("Out : %s", out);
    }
}

/**
 * @param state The state enum (FSM).
 * @return should return a string with the same name as enum case.
 * @brief This function is useful for debug trace only.
 *
 */
 
char* state_to_string(int state) {
	switch (state) {
    		case COMMENT :
    			return "COMMENT";
    			break;
    		
    		case DECIMAL_ZERO :
    			return "DECIMAL_ZERO";
    			break;
    		
    		case DECIMAL :
    			return "DECIMAL";
				break;  
				      			
    		case OCTO:
    			return "OCTO";
				break; 
				      			
    		case HEXA:
    			return "HEXA";
				break;
				     			
    		case DIRECTIVE:
    			return "DIRECTIVE";
				break; 
				     			
    		case REGISTER:
    			return "REGISTER";
				break; 
				     			
    		case SYMBOL:
    			return "SYMBOL";
				break;
				
			case PUNCTUATION:
    			return "PUNCTUATION";
				break; 
			
			case LABEL:
    			return "LABEL";
				break; 
				     			
    		default :
    			return "ERROR";
				break;
	}
}

















