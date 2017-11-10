
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
            
            j = hash( result[0], 1000);
            
            if ( tab[j] != NULL ) {
            	ERROR_MSG("There is a collision");
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
				WARNING_MSG("%s -> Op %s at %d", tab[i]->name, tab[i]->opcode, i );
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
	
	if (special != NULL) {
		strncpy ( ins->special, special, sizeof(ins->special) );
	}
	
	
	return ins;
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


/**
 * @return A int that contain the instruction.
 * @brief In this routine, we decode the instruction using the instruction set. There is 3 types of instructions. We determine the type by analysing the operation symbol.
 *
 */
 
/* /!\ int is a 4 bytes type. But if you run this code in an ARM 16 bits for instance (Thumb mode), int will be coded in 2 bytes only ! The assembly will failure /!\ */

unsigned int decodeInstruction( chain ch, inst * instSet ) {
	
	int i = 0;
	unsigned int code = 0;
	
	/* We start by verifying the size of int */
	if ( sizeof( int ) == 4 ) {
	
		/* init */
		
		chain in = ch;
		lex l;
		
		if (in != NULL) {
		
			/* /1\ If the chain is well constructed, we can read lexeme */
			l = read_lex( in );
			
			if ( l != NULL ) {
				
				/* The function work here */
				
				i = hash( l->this.value, 1000);
				WARNING_MSG("Decode error : Symbol %s / %d", l->this.value, i);
				
				if ( instSet[i] == NULL || strcmp( instSet[i]->name, l->this.value)) {
					majuscule(l->this.value);
					i = hash( l->this.value, 1000);
					if ( instSet[i] == NULL || strcmp( instSet[i]->name, l->this.value)) {
						ERROR_MSG("Decode error : can not decode the symbol %s (In upper case neither)", l->this.value);
					}
				}
			
			}
			
			
			
			/* /2\ The type of instruction permit to verify the arguments */
			
			/* FROM right to left. ALSO, DO NOT CHANGE ORDER of rd, rs ... By this mecanism we test each lexeme in teh good order ! */
			
			if (instSet[i]->type == R) {
				
				/* /!\ STRUCTURE /!\ */
				
				/* |SPECIAL|rs|rt|rd|sa|Opcode| */
			
				/* /!\ First, we add the opcode /!\ */
				
				/* Opcode 6 bits */
				code = instSet[i]->op;
				
				/* /!\ Verification of operands is done there /!\ */
				
				
				/* rd */
				if ( atoi(&instSet[i]->operand[2]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					code = code + (registerToInt(l->this.value) << 11);
					
				}
				
				/* rs */
				if ( atoi(&instSet[i]->operand[0]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					code = code + (registerToInt(l->this.value) << 21);
				}
				
				/* rt */
				if ( atoi(&instSet[i]->operand[1]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					code = code + (registerToInt(l->this.value) << 16);
				}
				
				/* sa */
				if ( atoi(&instSet[i]->operand[3]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					code = code + (registerToInt(l->this.value) << 6);
				}
				
				
				/* /!\ END /!\ */
				
			}
			else if ( instSet[i]->type == I ) {
				
				/* /!\ STRUCTURE /!\ */
				
				/* |Opcode|rs|rt|offset| */
				
				/* /!\ Finally, we add the opcode /!\ */
				
				/* Opcode 6 bits */
				
				code = instSet[i]->op << 26;
				
				/* rt */
				if ( atoi(&instSet[i]->operand[1]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					code = code + (registerToInt(l->this.value) << 16);
				}
				
				
				/* rs */
				if ( atoi(&instSet[i]->operand[0]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					code = code + (registerToInt(l->this.value) << 21);
				}
				
				/* offset */
				if ( atoi(&instSet[i]->operand[2]) ){
				
					in = read_line( in );
					l = read_lex( in );
					
					code = code + registerToInt(l->this.value);
					
				}
				
				
				/* /!\ END /!\ */
				
			}
			else if ( instSet[i]->type == IB ) {
				
				/* /!\ STRUCTURE /!\ */
				
				/* |Opcode|rs|rt|offset| */
				
				/* /!\ First, we add the opcode /!\ */
				
				/* Opcode 6 bits */
				
				code = instSet[i]->op << 26;
			
				
				/* rt */
				if ( atoi(&instSet[i]->operand[1]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					code = code + (registerToInt(l->this.value) << 16);
				}
				
				/* offset */
				if ( atoi(&instSet[i]->operand[2]) ){
				
					in = read_line( in );
					l = read_lex( in );
					
					code = code + registerToInt(l->this.value);
					
				}
				
				
				/* rs */
				if ( atoi(&instSet[i]->operand[0]) ){
				
					in = read_line( in );
					l = read_lex( in ); 
					
					code = code + (registerToInt(l->this.value) << 21);
				}
				
				
				/* /!\ END /!\ */
				
			}
			else if ( instSet[i]->type == J ) {
				
				/* /!\ STRUCTURE /!\ */
				
				/* |Opcode|offset| */
				
				/* /!\ First, we add the opcode /!\ */
				
				/* Opcode 6 bits */
				
				code = instSet[i]->op << 26;
				
				/* Offset */
				if ( atoi(&instSet[i]->operand[0]) ){
					
					in = read_line( in );
					l = read_lex( in ); 
					
					code = code + registerToInt(l->this.value);
				}
				
				/* /!\ END /!\ */
				
			}
			
			
			
		}
	}
	else {
		ERROR_MSG("Critical error : size of int is not 4 bytes (Please verify if your system is running in 32/64 bits).");
	}
	
	
	/* Conversion string -> int, and we return int */
	WARNING_MSG("Decoded code : %X", code);
	
	return code;
}

/**
 * @param ch The chain to analyse.
 * @param tab a char array used to load code byte by byte.
 * @return nothing.
 * @brief this routine decode only seleral directives. All other directives are directly managed by lex.c
 * In this routine, we manage this directives :
 * - .word w1, ... wn : put n word in contiguous way.
 * - .byte b1, ... bn : put n bytes in contiguous way.
 * - .asciiz s1, ... sn : put n string in contiguous way.
 * - .space n : put n bytes initalized to 0.
 */
 
void decodeDirective( chain ch, char * tab ) {
	chain directive = ch;
	int i = 0 ;
	
	lex l = read_lex( directive );
	
	if ( strcmp( l->this.value + 1, "word" ) ) {
		directive = read_line( directive );
		
		while (directive != NULL) { /* If the chain is well built, it is not mandatory to verify if lex is NULL */
			l = read_lex( directive );
			directive = read_line( directive );
		}
	}
	else if ( strcmp( l->this.value + 1, "byte" ) ) {
		directive = read_line( directive );
		
		while (directive != NULL) { /* If the chain is well built, it is not mandatory to verify if lex is NULL */
			l = read_lex( directive );
			directive = read_line( directive );
		}
	}
	else if ( strcmp( l->this.value + 1, "asciiz" ) ) {
		directive = read_line( directive );
		
		while (directive != NULL) { /* If the chain is well built, it is not mandatory to verify if lex is NULL */
			l = read_lex( directive );
			
			/* Here we add each string and we finish it by a char '\0', after that we translate using variable i */
			strcpy( tab + i , l->this.value);
			i = i + strlen( tab + i );
			
			directive = read_line( directive );
		}
	}
	else if ( strcmp( l->this.value + 1, "space" ) ) {
		directive = read_line( directive );
		
		if (directive != NULL) { /* If the chain is well built, it is not mandatory to verify if lex is NULL */
			l = read_lex( directive );
			
			int n = l->this.digit->this.integer; /* Number of uninitialized bytes */
			for (i=0; i<n; i++) {
				tab[i] = 0;
			}
		}
	}
	else {
		/* TODO raise an error */
	}
	
 	
 	/* In the end we add the char '\0' */
 	return;
 
 }


/**
 * @return unsigned int to be construct to build the decode code.
 * @brief this routine convert a char of a specified register to his int equivalent.
 *
 */

unsigned int registerToInt( char *input ) {
	
	/* TODO make it more robust */
	
	unsigned int t = 0;
	if ( strcmp( input+1, "zero" ) ) {
		t = 0;
	}
	else if ( strcmp( input+1, "at" ) ) {
		t = 1;
	}
	else if ( strcmp( input+1, "gp" ) ) {
		t = 28;
	}
	else if ( strcmp( input+1, "sp" ) ) {
		t = 29;
	}
	else if ( strcmp( input+1, "fp" ) ) {
		t = 30;
	}
	else if ( strcmp( input+1, "ra" ) ) {
		t = 31;
	}
	
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
	
	if (t > 31) {
		ERROR_MSG("Decode error : one register is badly written");
	}
	
	return t;
	
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
 
 void fetch( chain ch, chain symTab, chain chCode, int * section, unsigned int * addr, inst * instSet, unsigned int *nline ) {
 	chain element = ch;
 	lex l;
 	*section = TEXT;
 	*addr = 0;
 	
 	/* We read the line */
 	element = read_line( element );
 	l = read_lex( element );
 	
 	/* If line is not empty */
 	
 	if ( l != NULL ) {
 	
	 	if (  l->type == DIRECTIVE ) {
	 		if ( strncmp( l->this.value + 1, "text", 4 ) ) {
	 			*section = TEXT;
	 			*addr = 0;
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
	 		
	 			/* In other cases, launch decodeDirective and we increase addr. Indeed, we manage here all other directives that record bytes : .word, .byte ... */
	 			
	 			/* The code may be larger than a word. The directive is saved byte by byte, i.e. a char (because it is the smallest data memory can manage. */
	 			
	 			unsigned int byte = 0;
	 			
	 			char result[STRLEN];
	 			decodeDirective( element, result );
	 			
	 			while ( byte < strlen(result) ) {
		 			addCode( chCode, 0, *addr, result[byte], nline );
		 			*addr = *addr + 1; /* Address is incremented byte by byte */
		 		}
	 			
	 		}
	 		
	 	}
	 	else if ( l->type == LABEL ) {
	 		/* Here, it is a label, we add it to symTab without forgetting some verifications ;). After that, we launch fetch again to treat rest of the chain */
	 		
		 	addSymbol( l->this.value , symTab, *section, *addr, nline );
		 	
	 		if (read_line( element ) != NULL ) {
	 			
	 			/* /!\ Recursive /!\ */
	 			
		 		fetch( element, symTab, chCode, section, addr, instSet, nline );
		 		*addr = *addr + 4;
		 	}
		 	
	 	}
	 	else {
	 		/* The list is not empty, we are in the case of instruction */
	 		
	 		/* Add an element to code */
	 		addCode( chCode, 0, *addr, decodeInstruction( element, instSet ), nline );
	 		
	 	}
	}
	WARNING_MSG("Here");
	return;
 	
 }
 



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

void addSymbol( char * value, chain symTab, int section, unsigned int addr, unsigned int *nline ) {
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
			element = add_chain_next( element, *nline );
			*nline = *nline + 1;
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
			element = add_chain_next( element, *nline );
			*nline = *nline + 1;
			element->this.sym = createSymbol( value, section, addr );
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
	symbol sym = malloc ( sizeof( *sym ) );
	
	strcpy(sym->value , value);
	sym->section = section;
	sym->addr = addr;
	
	return sym;
}


/**
 * @param line The line of the code. Can be used to final display. (Is like an ID)
 * @param addr Mandatory, the address of the code regarding to the section
 * @param value Unsigned int to store the code
 * @return a code.
 * @brief Create a code container
 */

code createCode( unsigned int addr, unsigned int value, unsigned int line ) {
	code c = malloc ( sizeof( *c ) );
	
	c->line = line;
	c->addr = addr;
	c->value = value;
	
	return c;
}

/**
 * @param line The line of the code. Can be used to final display. (Is like an ID)
 * @param addr Mandatory, the address of the code regarding to the section
 * @param value Unsigned int to store the code
 * @return a code.
 * @brief Add the code to the chain.
 */

void addCode( chain chCode, unsigned int line, unsigned int addr, unsigned int value, unsigned int *nline  ) {
	/* We add a new element in the chain code */
	chCode = add_chain_next( chCode, *nline );
	*nline = *nline + 1;
	
	chCode->this.c = createCode(addr, value, *nline);
	return;
}









