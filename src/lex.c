
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

/**
 * @param line String of the line of source code to be analysed.
 * @param nline the line number in the source code.
 * @return should return the collection of lexemes that represent the input line of source code.
 * @brief This function performs lexical analysis of one standardized line.
 *
 */
void lex_read_line( char *line, int nline) {

	/* Initialisation of FSM */
	int i;
	int state = INIT;
	
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
        for ( i= 0; i< strlen(token); i++ ) {
        	
        	switch (state) {
        		case INIT :
        			if ( token[i] == '#' || comment != 0) {
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
        			else {
        				state = SYMBOL;
        			}
        			
        			
        			
        			break;
        			
        		case COMMENT :
        			if ( i == strlen(token)-1) {
        			
        				/* For each token, we show the comment for debug : */ 
        				DEBUG_MSG("[COMMENT] %s", token); 
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
					
					break;  
					      			
        		case OCTO:
					
					break; 
					      			
        		case HEXA:
					
					break;
					     			
        		case DIRECTIVE:
					
					break; 
					     			
        		case REGISTER:
					
					break; 
					     			
        		case SYMBOL:
					
					break; 
					     			
        		default :
					
					break;
        			
        		
        	
        	
        	}
        	
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
void lex_load_file( char *file, unsigned int *nlines ) {

    FILE        *fp   = NULL;
    char         line[STRLEN]; /* original source line */
    char         res[2*STRLEN]; /* standardised source line, can be longeur due to some possible added spaces*/



    fp = fopen( file, "r" );
    if ( NULL == fp ) {
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        ERROR_MSG("Error while trying to open %s file --- Aborts",file);
    }

    *nlines = 0;

    while(!feof(fp)) {

        /*read source code line-by-line */
        if ( NULL != fgets( line, STRLEN-1, fp ) ) {

            line[strlen(line)-1] = '\0';  /* eat final '\n' */
            (*nlines)++;

            if ( 0 != strlen(line) ) {
                lex_standardise( line, res );
                lex_read_line(res,*nlines);
            }
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
        	if (in[i] == ':' || in[i] == ',' || in[i] == '#' )
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
    WARNING_MSG("%s", out);
}






















