
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

unsigned int decodeInstruction( chain ch, inst * instSet ) {
	
	int i = 0;
	char code[33];
	code[32] = '\0';
	
	/* We start by verifying the size of int */
	if ( sizeof( int ) == 4 ) {
	
		/* init */
		
		chain in;
		lex l;
		in = ch;
		
		if (in != NULL) {
			l = read_lex( in );
			if ( l != NULL ) {
				
				/* The function work here */
				
				while (instSet[i] != NULL && strncmp( l->this.value, instSet[i]->name, sizeof( instSet[i]->name) )) {
					
					i++;
				}
				
				WARNING_MSG("Match ! IS = %s and input = %s", instSet[i]->name, l->this.value);
				
				
				
				
			
			
			
			
			
			}
			
			
			
			/* The type of instruction permit to verify the arguments */
			
			if (instSet[i]->type == R) {
				
				/* Operand init */
				char * zero = "00000";
				
				strncpy( code, "000000", 6);
				
				/* /!\ Verification of operands is done there /!\ */
				
				/* rd */
				if ( atoi(&instSet[i]->operand[2]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					strncpy(code + 16, registerToBinary(l->this.value), 5);
					
				}
				else {
					strncpy(code + 16, zero, 5);
				}
				
				/* rs */
				if ( atoi(&instSet[i]->operand[0]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					strncpy( code + 6, registerToBinary(l->this.value), 5);
				}
				else {
					strncpy(code + 6, zero, 5);
				}
				
				
				
				
				/* rt */
				if ( atoi(&instSet[i]->operand[1]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					strncpy(code + 11, registerToBinary(l->this.value), 5);
				}
				else {
					strncpy(code + 11, zero, 5);
				}
				
				
				
				
				
				/* sa */
				if ( atoi(&instSet[i]->operand[3]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					strncpy(code + 21, registerToBinary(l->this.value), 5);
				}
				else {
					strncpy( code + 21, zero, 5);
				}
				
				/* Finally, we add the operation code */
				
				strncpy( code + 26,  instSet[i]->op,  6);
				
				/* Conversion string -> int, and me return int */
				
				
			}
			else if ( instSet[i]->type == I ) {
			
				char * zero = "00000";
				
				strncpy( code, instSet[i]->op,  6);
				
				/* rt */
				if ( atoi(&instSet[i]->operand[1]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					strncpy(code + 11, registerToBinary(l->this.value), 5);
				}
				else {
					strncpy(code + 11, zero, 5);
				}
				
				
				
				
				
				/* rs */
				if ( atoi(&instSet[i]->operand[0]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					strncpy(code + 6, registerToBinary(l->this.value), 5);
				}
				else {
					strncpy(code + 6, zero, 5);
				}
				
				
			
				
				
				
				
				/* offset */
				if ( atoi(&instSet[i]->operand[2]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					strncpy(code + 16, registerToBinary(l->this.value), 16);
				}
				else {
					strncpy(code + 16, zero, 16);
				}
				
			}
			else if ( instSet[i]->type == J ) {
			
				strncpy( code, instSet[i]->op, 6);
				
				if ( atoi(&instSet[i]->operand[0]) ){
					
					in = read_line( in );
					l = read_lex( in ); 
					
					strncpy(code + 6, registerToBinary(l->this.value), 26);
				}
			}
			
			
			
		}
	}
	else {
		ERROR_MSG("Critical error, size of int is not 4 bytes.");
	}
	
	
	/* Conversion string -> int, and we return int */
	WARNING_MSG("%s", code);
	
	return (int) strtol(code, NULL, 2);

}

/**
 * @param ch The chain to analyse.
 * @return int of decoded directive.
 * @brief this routine decode only seleral directives. All other directives are directly managed by lex.c
 * In this routine, we manage this directives :
 * - .word w1, ... wn : put n word in contiguous way.
 * - .byte b1, ... bn : put n bytes in contiguous way.
 * - .asciiz s1, ... sn : put n string in contiguous way.
 */
 
 unsigned int decodeDirective( chain ch ) {
 	
 	return 0;
 
 }


/**
 * @return A translated string of register name
 * @brief this routine convert a char of a specified register to his binary equivalent.
 *
 */

char* registerToBinary( char *input ) {

	char * reg = malloc( sizeof(char) * 6);
	int t, i;
	reg[5]='\0';
	t = (atoi ( input+1) );
	
	if ( t <32 ) {
		for ( i=0; i<5; i++) {
			reg[4-i] = t % 2 +'0';
			t = t >> 1;
		}
	}
	
	return reg;
	
}



/**
 * @param ch Chain built by lex.c
 * @param symTab Last element of the symbol chain.
 * @param code The code chain. In fact, this is return of this routine.
 * @param section Pointer on the section, if NULL, no section defined yet.
 * @param addr Address in this section, if NULL, no section defined yet.
 * @param instSet Instruction Set if instruction decode is needed.
 * @return nothing 
 * @brief This routine is used to fetch and decode if needed the input intruction. 
 */
 
 void fetch( chain ch, chain symTab, chain code, int * section, unsigned int * addr, inst * instSet ) {
 	chain element = ch;
 	lex l;
 	
 	/* We read the line */
 	element = read_line( element );
 	l = read_lex( element );
 	
 	/* If line is not empty */
 	
 	if ( l != NULL ) {
 	
	 	if (  l->type == DIRECTIVE ) {
	 		if ( strncmp( l->this.value + 1, "text", 4 ) ) {
	 			*section = TEXT;
	 			*addr = 0;
	 			WARNING_MSG("Here");
	 		}
	 		else if ( strncmp( l->this.value + 1, "data", 4 ) ) {
	 			*section = DATA;
	 			*addr = 0;
	 		}
	 		else if ( strncmp( l->this.value + 1, "bss", 3 ) ) {
	 			*section = BSS;
	 			*addr = 0;
	 		}
	 		else if ( strncmp( l->this.value + 1, "set", 3 ) ) {
	 			/* We ignore this directive for the moment, it will be used once optimisation has been coded */
	 			
	 		}
	 		else {
	 			/* In other cases, launch decodeDirective and we increase the addr */
	 			
	 		}
	 		
	 	}
	 	else if ( l->type == LABEL ) {
	 		/* Here, it is a label, we add it the symTab without forgetting some verifications ;). After that we launch fetch again to treat rest of the chain */
	 		
	 		addSymbol( l->this.value , symTab, *section, *addr );
	 		fetch( read_line( element ), symTab, code, section, addr, instSet );
	 	}
	 	else {
	 		/* The list is not empty, we are in the case of instruction */
	 		
	 	}
	}
	return;
 	
 }
 



/**
 * @param value String value of symbol
 * @param symTab Table to complete
 * @param section Section identified
 * @param addr Address in section
 * @return nothing
 * @brief Add the symbol to the table of symbols. Two path for resolution :
 * - if section 
 */

void addSymbol( char * value, chain symTab, int section, unsigned int addr ) {
	chain element = symTab;
	symbol temp;
	int match = 0;
	
	if ( section != UNDEFINED ) { /* In case of no section/addr filled, we just add the symbol to the tab */
		while ( read_line( element ) != NULL && element->this.sym != NULL && !match ) {
			temp = readSymbol( element );
			if ( strcmp( temp->value, value ) ) {
				match = 1;
			}
			element = read_line( element );
		}
		
		/* If we have a match we update the symbol : */
		if (match) {
			temp->section = section;
			temp->addr = addr;
		}
		else {
		
			/* Create a new element of the chain and add the symbol */
			element = add_chain_next( element );
			element->this.sym = createSymbol( value, section, addr );
		}
	}
	else {
		while ( read_line( element ) != NULL && element->this.sym != NULL && !match ) {
			temp = readSymbol( element );
			if ( strcmp( temp->value, value ) ) {
				match = 1;
			}
			element = read_line( element );
		}
		
		/* If we have a match, nothing to do */
		if (!match) {
		
			/* Create a new element of the chain and add the symbol */
			element = add_chain_next( element );
			element->this.sym = createSymbol( value, section, addr );
		}
		
	}
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
		if ( temp != NULL && strncmp( temp->value, value, strlen(value) ) ) {
			return temp;
		}
	} while ( (element = read_line( element )) != NULL );
	
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

symbol createSymbol(char * value,  int section, unsigned int addr ) {
	symbol sym = malloc ( sizeof( symbol ) );
	
	strcpy(sym->value , value);
	sym->section = section;
	sym->addr = addr;
	
	return sym;
}








