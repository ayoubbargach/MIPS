
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
#include <functions.h>

#include <lex.h>
#include <inst.h>
#include <eval.h>
#include <syn.h>




/**
 * @return A int that contain the instruction.
 * @brief In this routine, we decode the instruction using the instruction set. There is 3 types of instructions. We determine the type by analysing the operation symbol.
 *
 */
 
/* /!\ int is a 4 bytes type. But if you run this code in an ARM 16 bits for instance (Thumb mode), int will be coded in 2 bytes only ! The assembly will failure. (For further improvement, need to implement uint32_t structure included by ctype.h) /!\ */

void decodeInstruction( chain ** c, inst * instSet ) {
	
	chain ch = read_next( *c[0] );
	chain symTab = *c[1];
	chain * chCode = c[2];
	chain * chRel = c[3];
	
	int i = 0;
	int nextInst = 0;
	int typeRel = R_MIPS_LO16;
	unsigned int code = 0;
	
	
    
	/* We start by verifying the size of int */
	if ( sizeof( int ) == 4 ) {
	
		/* init */
		
		chain in;
		lex l;
		
		if (ch == NULL) {
			ERROR_MSG("Internal error : Lexeme chain is badly written. Please contact devs.");
		}
		
	
		/* /1\ If the chain is well constructed, we can read lexeme */
		l = read_lex( ch );
		
		if ( l != NULL ) {
			
			/* The function work here */
			
			i = hash( l->this.value, 1000);
			
			if ( instSet[i] == NULL || strcmp( instSet[i]->name, l->this.value)) {
			
				majuscule(l->this.value);
				i = hash( l->this.value, 1000);
				
				if ( instSet[i] == NULL || strcmp( instSet[i]->name, l->this.value)) {
					ERROR_MSG("Decode error : can not decode the symbol %s (In upper case neither)", l->this.value);
				}
			}
		
		}
		else { /* If no lexeme, Lexeme chain is not well constructed */
			ERROR_MSG("Decode error : Lexeme chain is badly built");
		}
		
		/* /2\ We start by managing special specifications */
		
		/* If special == '#', we do nothing */
		if ( strcmp(instSet[i]->special, "#") ) {
		
			/* init */
			int j=0;
			char *seps = ",=";
			char *token = NULL;
			chain chTemp = malloc( sizeof( *chTemp ) );
			*chTemp = *ch;
			
			in = chTemp;
			
			
			char* result[16] = {NULL};
			
			
			for( token = strtok( instSet[i]->special, seps ); NULL != token; token = strtok( NULL, seps )) {
        		result[j] = token;
        		j++;
        	}
        	
        	/* Special modifications depend on instruction type
        	 * If there is any, all the operands need to by specified in order !
        	 * Example : if you need to change rs and rt with the first and third operand of input chain, you must write something like;
        	 * rs=A,rt=C
        	 */
        	
        	j=0;
        	
        	/* first we verify if there is any second instrucion to do */
        	if ( result[j][0] == '*' ) {
        		nextInst = 1;
        		j ++;
        		
        		/* We also change OP into the lexeme for the next recurvise call of decodeInstruction */
        		strcat(l->this.value, "*");
        		
        		/* Case of a pseudo-instruction in two instructions => HILO reloc */
        		typeRel = R_MIPS_HI16;
        	}
        	else if ( l->this.value[strlen(l->this.value) - 1] == '*' ) { /* It means that previous reloc was HI16 */
				typeRel = R_MIPS_LO16;
			}
        	
        	
        	switch (instSet[i]->type) {
        	
        		case R :
					
					/* The special specifications must be constructed in the good order ! */
					
					if ( atoi(&instSet[i]->operand[2]) && !strcmp(result[j],"rd") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
					
					if ( atoi(&instSet[i]->operand[0]) && !strcmp(result[j],"rs") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
					
					if ( atoi(&instSet[i]->operand[1]) && !strcmp(result[j],"rt") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
					
					if ( atoi(&instSet[i]->operand[3]) && !strcmp(result[j],"sa") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
					
					
					break;
				
				case I :
				
					if ( atoi(&instSet[i]->operand[1]) && !strcmp(result[j],"rt") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
					
					if ( atoi(&instSet[i]->operand[0]) && !strcmp(result[j],"rs") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
					
					if ( atoi(&instSet[i]->operand[2]) && !strcmp(result[j],"offset") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
			
					j+=2; 

					break;
				
				case IB :
				
					if ( atoi(&instSet[i]->operand[1]) && !strcmp(result[j],"rt") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
					
					if ( atoi(&instSet[i]->operand[2]) && !strcmp(result[j],"offset") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
					
					if ( atoi(&instSet[i]->operand[0]) && !strcmp(result[j],"rs") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
			
					j+=2;
					
					break;
				
				case J :
					if ( atoi(&instSet[i]->operand[0]) && !strcmp(result[j],"offset") ) {
						chTemp = get_special( result[j+1], chTemp, ch);
						j+=2;
						
					}
					
					break;
				
				default :
					ERROR_MSG("Internal error : Error in instSet.txt");
					break;
			}
			
			
			/* TODO free the other chain if nextInst = 0 */
		
			
		}
		else {
			in = ch;
		}
		
		
		
		
		
		
		/* /3\ The type of instruction permit to verify the arguments */
		
		/* FROM right to left. ALSO, DO NOT CHANGE ORDER of rd, rs ... By this mecanism we test each lexeme in the good order ! */
		
		if (instSet[i]->type == R) {
			
			/* /!\ STRUCTURE /!\ */
			
			/* |SPECIAL|rs|rt|rd|sa|Opcode| */
		
			/* /!\ First, we add the opcode /!\ */
			
			/* Opcode 6 bits */
			code = instSet[i]->op;
			
			/* /!\ Verification of operands is done there /!\ */
			
			
			/* rd */
			if ( instSet[i]->operand[2] % '0' ){
				
				l = get_lex( &in ); 
				
				code = code + (l->this.digit->value << 11);
				
			}
			
			/* rs */
			if ( instSet[i]->operand[0] % '0' ){
			
				l = get_lex( &in ); 
				
				code = code + (l->this.digit->value << 21);
			}
			
			/* rt */
			if ( instSet[i]->operand[1] % '0' ){
			
				l = get_lex( &in ); 
				
				code = code + (l->this.digit->value << 16);
			}
			
			/* sa */
			if ( instSet[i]->operand[3] % '0' ){
			
				l = get_lex( &in ); 
				
				code = code + (l->this.digit->value << 6);
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
			if ( instSet[i]->operand[1] % '0' ){
			
				l = get_lex( &in );
				
				
				code = code + (l->this.digit->value << 16);
			}
			
			/* rs */
			if ( instSet[i]->operand[0] % '0'  ){
			
				l = get_lex( &in );
				
				code = code + (l->this.digit->value << 21);
			}
			
			
			/* offset */
			if ( instSet[i]->operand[2] % '0'  ){
			
				l = get_lex( &in );
				
				code = code + ((eval(l, typeRel, chRel, symTab) << 16) >> 16);
				
			}
			
			
			/* /!\ END /!\ */
			
		}
		else if ( instSet[i]->type == I2 ) { /* rs and rt order different => FOR BEQ TYPE OF INSTRUCTIONS !! */
			
			/* /!\ STRUCTURE /!\ */
			
			/* |Opcode|rs|rt|offset| */
			
			/* /!\ Finally, we add the opcode /!\ */
			
			/* Opcode 6 bits */
			
			code = instSet[i]->op << 26;
			
			
			
			/* rs */
			if ( instSet[i]->operand[0] % '0'  ){
			
				l = get_lex( &in );
				
				code = code + (l->this.digit->value << 21);
			}
			
			/* rt */
			if ( instSet[i]->operand[1] % '0' ){
			
				l = get_lex( &in );
				
				
				code = code + (l->this.digit->value << 16);
			}
			
			/* offset */
			if ( instSet[i]->operand[2] % '0'  ){
			
				l = get_lex( &in );
				
				code = code + ((eval(l, RELATIVE, chRel, symTab) << 16) >> 16);
				
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
			if ( instSet[i]->operand[1] % '0' ){
			
				l = get_lex( &in );
				
				code = code + (l->this.digit->value << 16);
			}
			
			/* offset */
			if ( instSet[i]->operand[2] % '0' ){
			
				l = get_lex( &in );
				
				code = code + ((eval(l, typeRel, chRel, symTab) << 16) >> 16); /* Keep in mind that me need to change FFFFFE00 to 0000FE00 before adding */
				
			}
			
			
			/* rs */
			if ( instSet[i]->operand[0] % '0' ){
			
				l = get_lex( &in );
				
				code = code + (l->this.digit->value << 21);
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
			if ( instSet[i]->operand[0] % '0' ){
				
				l = get_lex( &in ); 
				
				code = code + ((eval(l, R_MIPS_26, chRel, symTab) << 26) >> 26);
			}
			
			/* /!\ END /!\ */
			
		}	
			
			
		
	}
	else {
		ERROR_MSG("Critical error : size of int is not 4 bytes (Please verify if your system is running in 32/64 bits).");
	}
	
	
	/* In the end, we add the result code in the code chain without forgetting to increment addr ! */

	addCode( chCode,  code);
	
	addr = addr + 4;
	
	if ( nextInst == 1 ) {
		
		/* /!\ Recursive ! /!\ */
		decodeInstruction( c, instSet);
		
	}
	
	return;
}

/**
 * @param value Special value to implement
 * @return a new element to chain
 * @brief Routine used to create a new temporary chain of lexeme that will we free within the same loop in decode_instruction(). By building this new chain, we aim to execute it by taking into account special indications.
 */
 
chain get_special( char *value, chain parent, chain source ) {
    
	chain c = add_chain_next( parent );
	lex l = malloc( sizeof( *l ) );
	
	/* Error Management */
	if (l == NULL) {
		ERROR_MSG("Memory error : Malloc failed");
    }
    
	int loop=0, i=0;
	
	
	if ( value[0] >= 'A' && value[0] < 'Z' ) {
		
		/* Case when rs=A .. */
		loop = (value[0] % 'A') +1;
		for (i=0; i < loop; i++) {
			
			source = read_next(source);
		}
		
		
		l = read_lex( source );
		
		/* Test function
		if (value[0] == 'B') {
			WARNING_MSG("value B : %s %d", l->this.value, l->type);
		} */
		
			
		add_lex( c, l );
		
	}
	else {
		
		/* Case when rs=00001 or offset=(in bits)  */
		
		if (strlen(value) < 6)
			l = make_lex( REGISTER, value, UNSIGNED );
		else
			l = make_lex( BIT, value, UNSIGNED );
		
		add_lex( c, l );
		
	}
	
	return c;
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
 
void decodeDirective( chain ** c ) {

	chain directive = read_next( *c[0] );
	chain symTab = * c[1];
	chain * chCode = c[2];
	chain * chRel = c[3];

	/* Note that we increase addr. Indeed, we manage here all other directives that record bytes : .word, .byte ...
	 * The code may be larger than a word. The directive is saved byte by byte in some cases, i.e. a char (because it is the smallest data memory managed) 
	 */
	
	unsigned int code = 0;
	unsigned int byte = 0;
	int i;
	
	lex l = read_lex( directive );
	
	if ( !strcmp( l->this.value + 1, "word" ) ) {
	
	
		directive = read_next( directive );
		
		while (directive != NULL) { /* If the chain is well built, it is not mandatory to verify if lex is NULL */
			l = read_lex( directive );
			
			if (l != NULL) {
			
				code = eval(l, R_MIPS_32, chRel, symTab); /* We use there all the unsigned int ! */
			
				addCode( chCode, code );
				addr = addr + 4;
			}
			
			directive = read_next( directive );
		}
		
		
		
	}
	else if ( !strcmp( l->this.value + 1, "byte" ) ) {
	
		typeCode = BYTE;
		directive = read_next( directive );
		
		while (directive != NULL) {
			l = read_lex( directive );
			
			if (l != NULL) {
			
				code = ((eval(l, NONE, chRel, symTab) << 24) >> 24 ); /* We need only the first 8 bits */
			
				addCode( chCode, code );
				addr = addr + 1;
			}
			
			directive = read_next( directive );
		}
		
		
	}
	else if ( !strcmp( l->this.value + 1, "asciiz" ) ) {
	
		typeCode = BYTE;
		directive = read_next( directive );
		
		
		while (directive != NULL) { 
			l = read_lex( directive );
			
			/* Here we add each string and we finish it by a char '\0', after that we translate using variable i */
			WARNING_MSG("%s",l->this.value);
			while ( byte < strlen(l->this.value) - 2 ) {
				
				code = l->this.value[byte];

				addCode( chCode, code );
				byte++;
				addr = addr + 1; /* Address is incremented byte by byte */
			}
			
			directive = read_next( directive );
		}
		
		
	}
	else if ( !strcmp( l->this.value + 1, "space" ) ) {
	
		typeCode = BYTE;
		directive = read_next( directive );
		
		if (directive != NULL) { /* If the chain is well built, it is not mandatory to verify if lex is NULL */
			l = read_lex( directive );
			code = 0;
			
			int n = l->this.digit->this.integer; /* Number of uninitialized bytes */
			for (i=0; i<n; i++) {
				addCode( chCode, 0 );
				addr = addr + 1;
			}
		}
		
		
	}
	else {
		ERROR_MSG("Decode error : directive %s unknown", l->this.value);
	}
	
	/* In all way we put typeCode at WORD */
	typeCode = WORD;
	
	
	
 	return;
 
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
 
 void fetch( chain ** c, inst * instSet ) {
 	chain element = *c[0];
 	chain * symTab = c[1];
 	
 	lex l;
 	
 	/* We read the line */
 	element = read_next( element );
	l = read_lex( element );
	
	/* We get the line value; it is mandatory to add it to chain chRel symTab .. */
	line = element->line;
 	
 	/* If line is not empty, we analyse it */
 	
 	if ( l != NULL ) {
 	
	 	if (  l->type == DIRECTIVE ) {
	 	
	 		
	 		
	 		
	 		if ( !strcmp( l->this.value + 1, "text" ) ) {
	 			section = TEXT;
	 			addr = 0;
	 		}
	 		else if ( !strcmp( l->this.value + 1, "data" ) ) {
	 			section = DATA;
	 			addr = 0;
	 		}
	 		else if ( !strcmp( l->this.value + 1, "bss" ) ) {
	 			section = BSS;
	 			addr = 0;
	 		}
	 		else if ( !strcmp( l->this.value + 1, "set" ) ) {
	 			/* We ignore this directive for the moment, it will be used once optimisation has been coded */
	 			
	 		}
	 		else {
	 		
	 			decodeDirective( c );
	 			
	 		}
	 		
	 	}
	 	else if ( l->type == LABEL ) {
	 		/* Here, it is a label, we add it to symTab without forgetting some verifications ;). After that, we launch fetch again to treat rest of the chain */
	 		
		 	addSymbol( l->this.value , *symTab, 1);
		 
		 	
	 		if (read_next( element ) != NULL ) {
	 			
	 			/* /!\ Recursive /!\ */
	 			/* It is mandotory to proceed this way in order to avoid stack overflow */
	 			chain * cFetch[4] = { &element, c[1], c[2], c[3] };		
	 			
		 		fetch( cFetch, instSet );
		 	}
		 	
	 	}
	 	else {
	 		/* The list is not empty, we are in the case of instruction */

	 		decodeInstruction( c , instSet );
	 		
	 	}
	}
	
	return;
 }
 
/**
 * @param element chain element which contains lexeme.
 * @return nothing
 * @brief Get lexeme. Raise an error if missing.
 */
lex get_lex( chain * element ) {
	lex l;
	
	*element = read_next( *element );
	l = read_lex( *element );
	
	/* If NULL, raise an error */
	if ( l == NULL ) {
		ERROR_MSG("Syntax error : an operand is missing");
	}

	return l;
}
 


/* ##### Code functions ##### */

/**
 * @param line The line of the code. Can be used to final display. (Is like an ID)
 * @param addr Mandatory, the address of the code regarding to the section
 * @param value Unsigned int to store the code
 * @return a code.
 * @brief Add the code to the chain.
 */

void addCode( chain * chCode, unsigned int value ) {
	/* We add a new element in the chain code */
	*chCode = add_chain_next( *chCode );
	
	(*chCode)->this.c = createCode(addr, value);
	return;
}

/**
 * @param line The line of the code. Can be used to final display. (Is like an ID)
 * @param addr Mandatory, the address of the code regarding to the section
 * @param value Unsigned int to store the code
 * @return a code.
 * @brief Create a code container
 */

code createCode( unsigned int addr, unsigned int value ) {
	code c = malloc ( sizeof( *c ) );
	
	c->type = typeCode;
	c->section = section;
	c->line = line;
	c->addr = addr;
	c->value = value;
	
	
	return c;
}

/**
 * @param chCode The chain to analyse
 * @return a code.
 * @brief A simple way to get the code.
 */

code getCode( chain chCode ) {
	
	if( chCode->this.c != NULL )
		return chCode->this.c;
	
	return NULL;
}


/**
 * @param chCode The chain to analyse
 * @return a code.
 * @brief A simple way to find code using address.
 */

code findCode( chain chCode, unsigned int addr ) {
	code c;
	while ( chCode != NULL ) {
		c = getCode( chCode );
		if ( c->addr == addr )
			return c;
			
		chCode = read_next( chCode );
	}
	
	
	return NULL;
}









