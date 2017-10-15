
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
				
			case COMMA:
    			return "COMMA";
				break; 
			
			case LABEL:
    			return "LABEL";
				break; 
				     			
    		default :
    			return "ERROR";
				break;
	}
}


/**
 * @param line String of the line of source code to be analysed.
 * @param nline the line number in the source code.
 * @return should return the collection of lexemes that represent the input line of source code.
 * @brief This function performs lexical analysis of one standardized line.
 *
 */
void lex_read_line( char *line, int nline, chain newline) {

	int i;
	chain element = add_chain_next( newline );

	
	/* Useful when a token is defined as a comment, all the following tokens are also undertood in the same way */
	int comment = 0;
	
	/* After standirising, we only need ' ' as sep */
    char *seps = " ";
    char *token = NULL;
    char save[STRLEN];

    /* copy the input line so that we can do anything with it without impacting outside world*/
    memcpy( save, line, STRLEN );
    

    /* get each token */
    for( token = strtok( save, seps ); NULL != token; token = strtok( NULL, seps )) {
    	
 		/* Re-initialisation of FSM */
    	int state = INIT;
    	
        for ( i= 0; i< strlen(token); i++ ) {
        	
        	switch (state) {
        		case INIT :
        			if ( token[i] == '#' || comment != 0) {
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
        			else if ( token[i] == ',' ) {
        				state = COMMA;
        			}
        			else {
        				state = SYMBOL;
        			}
        			
        			
        			
        			break;
        		
        		case DECIMAL_ZERO :
        			if ( isdigit(token[i]) ) {
        				if ( token[i] < '8' ) {
        					state = OCTO;
        				}
        				else if ( token[i] == 'x' ) {
        					state = HEXA;
        				}
        				else state = ERROR;
        			}
        			
        			break;
        		
        		case DECIMAL :
					state = (isdigit( token[i] )) ? DECIMAL : ERROR;
					break;  
					      			
        		case OCTO:
        			state = ( isdigit( token[i] ) || token[i] < '8' ) ? OCTO : ERROR;
					
					break; 
					      			
        		case HEXA:
        			state = ( isxdigit( token[i] ) ) ? HEXA : ERROR;
					
					break;
					
				
				/* All other states are managed by default */			
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
        
        /* /!\ The comma and comments are not added to the collection (skip if) /!\ */
        
        if ( !( state == COMMENT || state == COMMA || state == INIT ) ) {
			/* Create lexeme and add value */
			lex lexeme = make_lex( state, token );
		
			/* Add lexeme to the element */
			add_lex(element, lexeme);
		
			/* TEST Code
			if (read_lex(element) != NULL ) {
				WARNING_MSG("%s", state_to_string( read_lex(element)->type ) );
			} */
			
		
			/* Create a new chain element to store the next lexeme */
			element = add_chain_next( element );
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
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
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
    		newline = add_chain_newline( newline );
    	}
    }
    
    /* TEST code */
    
    chain chcopy = ch;
    chain in;
    
    WARNING_MSG("%d", (*nlines));
    
    while (  chcopy != NULL ) {
    	in = chcopy;
    	in = read_line( in );
    	while ( in != NULL ) {
    		
    		
    		if (read_lex( in ) != NULL ) {
				WARNING_MSG("%s", state_to_string( read_lex(in)->type ) );
			}
			
    		in = read_line( in );
    	}
    	
    	DEBUG_MSG("[NL]");
    	chcopy = next_line( chcopy );
    	
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

/* note that MIPS assembly supports distinctions between lower and upper case*/
void lex_standardise( char* in, char* out ) {

    unsigned int i, j, k;
    
    k = 0;

    for ( i= 0, j= 0; i< strlen(in); i++ ) {

        /* translate all spaces (i.e., tab) into simple spaces, we use k to delete futher spaces */
		
		if (isblank((int) in[i])) {
		    k = 1;
        }
        else{
        	if (in[i] == ',' )
        	{
        		out[j] = ' ';
        		out[j+1]=in[i];
        		
        		
        		/* If the character after is blank, we do nothing */
        		
        		if ( isblank((int) in[i+1]) || i+1 == strlen(in) ) {
        			j = j + 2;
        		}
        		else {
        		    out[j+2]=' ';
        			j = j + 3;
        		}
        		
        		k=0;
        	}
        	else if ( in[i] == '#' )
        	{
        		/* If we have a space before, we add one. If not, nothing to do. */
        		if (k) {
        			out[j]=' ';
        			j++;
        		}
        		
        		out[j]=in[i];
        		
        		
        		/* If the character after is blank, we do nothing */
        		
        		if ( isblank((int) in[i+1]) || i+1 == strlen(in) ) {
        			j++;
        		}
        		else {
        		    out[j+1]=' ';
        			j = j + 2;
        		}
        		
        		k=0;
        	}
        	else if ( in[i] == ':' )
        	{
        		out[j]=in[i];
        		
        		
        		/* If the character after is blank, we do nothing */
        		
        		if ( isblank((int) in[i+1]) || i+1 == strlen(in) ) {
        			j++;
        		}
        		else {
        		    out[j+1]=' ';
        			j = j + 2;
        		}
        		
        		k=0;
        	}
        	else if (k)
        	{
        		out[j]=' ';
        		out[j+1]=in[i];
        		j = j + 2;
        		k=0;
        	}
        	else {
		    	out[j]=in[i];
		    	j++;
        	}
        	
        }
		
    }
    out[j]='\0';
    
    /* To compare in and out :
    WARNING_MSG("%s", in);
    WARNING_MSG("%s", out); */
}






















