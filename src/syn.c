
/**
 * @file lex.c
 * @author François Portet <francois.portet@imag.fr>
 * @brief MIPS assembly syntax.
 *
 * These routines perform the analysis of the syntactic aspect of the assembly.
 * 1/ Analysis of the symbol and loading of instruction information from a dictionnaire.
 * 2/ Analysis of instruction type : 3 type of instruction. The aim is to recognize the shape of the instruction using the collection of lexemes.
 * R-Instruction $rd, $rs, $rt, sa
 * I-Instruction $rt, $rs, immediate
 * J-Instruction target
 * 3/ If the intruction is legal, translate to binary. If not, raise an error.
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
    
    /* In case there are no special arguments */
    result[4]=NULL;
    
    while(!feof(fp)) {
    	/*read line-by-line */
        if ( NULL != fgets( line, STRLEN-1, fp ) ) {
        	line[strlen(line)-1] = '\0';  /* eat final '\n' */
            
            if ( 0 != strlen(line) ) {
            	for( token = strtok( line, seps ); NULL != token; token = strtok( NULL, seps )) {
            		result[i] = token;
            		i++;
            	}
            }
            
            /* Add instruction to the table */
            
            tab[j]= makeInst( result[0], result[1], result[2], result[3], result[4]);
            j++;
            
            /*
            else {
            	ERROR_MSG("The %s file is not well written --- Aborts",file);
            } */
            
            
            i=0;
        }
		
	
	}
	
	/* The last one is NULL (to treat with loop) */
	
	tab[j] = NULL;
	
	/* TEST CODE
	
	for (i=0;i<20;i++) {
		WARNING_MSG("The result is %s -> Op %s at %p", tab[i]->name, tab[i]->op, tab[i] );
	} */
	
	
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
	strncpy ( ins->op, op, sizeof(ins->op) );
	ins->type = atoi( type );
	strncpy ( ins->operand, operand, sizeof(ins->operand) );
	
	if (special != NULL) {
		strncpy ( ins->special, special, sizeof(ins->special) );
	}
	else {
		/* If no special indications, we initialize to "." */
		strncpy ( ins->special, ".", sizeof(ins->special) );
	}
	
	
	return ins;
}


/**
 * @return A int that contain the instruction.
 * @brief In this routine, we decode the instruction using the instruction set. There is 3 types of instructions. We determine the type by analysing the operation symbol.
 *
 */
 
/* /!\ int is a 4 bytes type. But if you run this code in an ARM 16 bits for instance (Thumb mode), int will be coded in 2 bytes only ! The assembly will failure /!\ */

int decodeInstruction( chain ch, inst * instSet ) {
	
	int result = 0, i = 0;
	/* We start by verifying the size of int */
	if ( sizeof( int ) == 4 ) {
	
		/* init */
		
		chain in;
		lex l;
		in = read_line( ch );
		
		if (in != NULL) {
			l = read_lex( in );
			if ( l != NULL ) {
				
				/* The function work here */
				
				while (instSet[i] != NULL && strncmp( l->this.value, instSet[i]->name, sizeof( instSet[i]->name) )) {
					
					i++;
				}
				
				WARNING_MSG("Match ! IS = %s and input = %s", instSet[i]->name, l->this.value);
				
				
				
				
			
			
			
			
			
			}
			
			in = read_line( in );
			l = read_lex( in ); /* We read the next lexeme */
			
			
			/* The type of instruction permit to verify the arguments */ 
			
			if (instSet[i]->type == R) {
				
			}
			else if ( instSet[i]->type == I ) {
			}
			else {
			}
			
			
			
		}
	}
	else {
		ERROR_MSG("Critical error, size of int is not 4 bytes. Please contact us if you want an upgrade.");
	}

	return result;

}

















