/**
 * @file global.h
 * @brief Global definitions.
 *
 * Global definitions.
 */

#ifndef _GLOBAL_H
#define _GLOBAL_H



/**
 * @mainpage MIPS Assembler
 *
 * @section sec0 Command line invocation
 *
 * Usage: <br/>
 * <br/>
 * ./as-mips source.asm
 *
 *
 * @section sec3 What works
 *
 * - the documentation! <br/>
 * - the lexical analysis <br/>
 * - the grammatical analysis <br/>
 * - addressing modes management <br/>
 * - loading of instructions for a dictionary file <br/>
 * - labels management <br/>
 * - code generation as a binary file and as a text file <br/>
 *
 * @section sec4 What is left for future happy hacking
 *
 * - management of all directives <br/>
 * - generation of elf relocatable files <br/>
 * - extensive testing.
 *
 */


/*!
  \brief INTERNALS: Maximum length of static strings.
 */
#define STRLEN          256

/*!
  \brief INTERNALS: Value for boolean FALSE.
 */
#define FALSE            0

/*!
  \brief INTERNALS: Value for boolean TRUE.
 */
#define  TRUE            1

/*!
  \brief INTERNALS: Return value for boolean SUCCESS.
 */
#define SUCCESS          0

/*!
  \brief INTERNALS: Return value for boolean FAILURE.
 */
#define FAILURE          1

/*!
  \brief INTERNALS: Definition of some state for our FSM. EXIT is managed the same way as COMMENT : We do not consider it. ( ',' for example)
 */

enum {INIT, DECIMAL_ZERO, DECIMAL, OCTO, HEXA, SYMBOL, COMMENT, REGISTER, DIRECTIVE, COMMA, LABEL, ERROR};



/*!
  \brief INTERNALS: Type definition of digit.
 */
 
typedef struct digit_t {
	unsigned int type;
	
	union {
		int integer;
		int octo;
		char * hexa;
	}this;
} digit;

/*!
  \brief INTERNALS: Type definition of lex.
 */
 
typedef struct lexeme_t {
	unsigned int type;
	
	union {
		digit digit;
		char value[STRLEN];
	}this;
} *lex;

/* All types of instructions */
enum {R, I, J};

/* Instruction structure as is use in the instruction set */

typedef struct inst_t {
	char name[16];
	char op[16];
	int type;
	
	/* Give indication about the waited operands in binary 
	   For example : In R-type, 1010 if $rd and $rt in waited. */
	   
	char operand[8];
	char special[STRLEN];
} *inst;

/* Symbol contained in symTab */
typedef struct symbol_t {
	unsigned int line;
	
	int section;
	unsigned int addr;
	
	char value[STRLEN];
	
} *symbol;

/*!
  \brief INTERNALS: Type definition of chain. To understand the signification of next / bottom, please read the documentation graphs.
 */
 
typedef struct chain_t {
	struct chain_t *next;
	
	union {
		struct chain_t *bottom;
		lex bottom_lex;
		inst bottom_ins;
		symbol sym;
	}this;
} *chain;


/* After decode, used to struture the chain list in order to build the binary code. */

typedef struct code_t {
	chain origin;
	
	/* Decoded code in hexa */
	char * s;
	
} code;


#endif /* _GLOBAL_H */













