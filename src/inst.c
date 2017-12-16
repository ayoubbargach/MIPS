
/**
 * @file inst.c
 * @author François Portet <francois.portet@imag.fr>
 * @brief Instructions routines.
 *
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <inst.h>
#include <functions.h>



/**
 * @return A tab of the desired instruction set.
 * @brief A table with all instruction set information is loaded. The structure of this information is described in "inst" type definition.
 *
 */

void instructionSet( inst * tab ) {
	
	/* File init */
	FILE *fp   = NULL;
    char * file = NULL;
	
	/* We load the file which contain all the instruction set, you can rename it here if you want ! */
	
	file  	= "instSet.txt";


    if ( NULL == file ) {
        ERROR_MSG("Error, no intruction set file specified in source code !");

    }
    
    fp = fopen( file, "r" );
    
    if ( NULL == fp ) {
        ERROR_MSG("Error while trying to open %s file --- Aborts",file);
    }
    
    /* File opened, the structure is like this : "SRL 000010 0 0111 rs=00001". For more information, please read the documentation. */
    
    /* General struture : OP BinaryDecode Type(R=0,I=1,J=2) Operand SpecialSet */
    
    /* Init */
    int i=0, j=0;
    char line[STRLEN];
    char *seps = " ";
    char *token = NULL;
    
    /* Used to save the tokenized strings */
    char* result[5];
    
    
    while(!feof(fp)) {
    
    	/*read line-by-line */
    	
    	/* In case there are no special arguments */
    	result[4]=NULL;
    
        if ( NULL != fgets( line, STRLEN-1, fp ) ) {
        	line[strlen(line)-1] = '\0';  /* eat final '\n' */
            
            if ( 0 != strlen(line) ) {
            	for( token = strtok( line, seps ); NULL != token; token = strtok( NULL, seps )) {
            		result[i] = token;
            		i++;
            	}
            }
            
            /* Add instruction to the table */
            
            j = hash( result[0], 1000);
            
            if ( tab[j] != NULL ) {
            	ERROR_MSG("There is a collision, hash function have to be changed");
            }
            else {
            	/* Make instruction and record it */
            	tab[j]= makeInst( result[0], result[1], result[2], result[3], result[4]);
            } 
            
            
            i=0;
        }
		
	
	}
	
	/* The last one is NULL (to treat with loop) */
	
	tab[j] = NULL;
	
	/* ---- TEST 3 ---- */
	
	if (testID == 3) {
    
		i=0;
		while ( i<1000 ) {
			if (tab[i] != NULL) {
				WARNING_MSG("%s | %s key %d", tab[i]->name, tab[i]->opcode, i );
			}
		
			i++;
		}
	}
	
	
}

/**
 * @param name Name of instruction, "ADD"
 * @param op Binary translation, "10000"
 * @param type From enum (see header) R => 0, ...
 * @param operand Define which operands are defined. For example, 0110 => Only rt and rd can be defined in assembly source code. 
 * @param special Define automatically some operands. For example, sa=00000 in case of ADD.
 * @return A completed structure Inst, ready to be added to the instruction set chain.
 * @brief Routine to make fastly a instruction using string parameters.
 *
 */


inst makeInst( char* name, char* op, char* type, char* operand, char* special ) {
	inst ins = malloc( sizeof( *ins ) );

	/* strncpy copy the string arguments in the structure, you can't do that with a simple "=" ! */
	strncpy ( ins->name, name, sizeof(ins->name) );
	strncpy ( ins->opcode, op, sizeof(ins->opcode) );
	ins->op = binaryToInt( op );
	
	ins->type = atoi( type );
	strncpy ( ins->operand, operand, sizeof(ins->operand) );
	
	if (special != NULL && special[0] != '#') {
		strcpy ( ins->special, special );
	}
	else {
		strcpy ( ins->special, "#" );
	}
	
	
	return ins;
}


