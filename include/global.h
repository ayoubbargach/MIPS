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
  \brief All enum definitions.
 */

enum {INIT, DECIMAL_ZERO, BIT, DECIMAL, OCTO, HEXA, SYMBOL, COMMENT, REGISTER, DIRECTIVE, PUNCTUATION, LABEL, ERROR};

enum {UNSIGNED, SIGNED};

enum {R, I, J, IB, I2};

enum {UNDEFINED, TEXT, DATA, BSS};

enum { LIST_MODE, OBJECT_MODE, ELF_MODE, TEST_MODE };

enum { NONE, R_MIPS_32, R_MIPS_26, R_MIPS_HI16, R_MIPS_LO16, RELATIVE };

enum { WORD, BYTE};


/*!
  \brief Type definition of digit.
 */
 
typedef struct digit_t {
	unsigned int type;
	int sign;
	
	/* For debug purposes only, we keep all data */
	union {
		int integer;
		int octo;
		char * hexa;
	}this;
	
	unsigned int value;
}* digit;

/*!
  \brief Type definition of lex.
 */
 
typedef struct lexeme_t {
	unsigned int type;
	
	union {
		digit digit;
		char value[STRLEN];
	}this;
} *lex;

/*!
  \brief Instruction structure as is use in the instruction set.
 */

typedef struct inst_t {
	char name[16];
	char opcode[16];
	unsigned int op;
	int type;
	
	/* Give indication about the waited operands in binary 
	   For example : In R-type, 1010 if $rd and $rt in waited. */
	   
	char operand[8];
	char special[STRLEN];
} *inst;

/*!
  \brief : Symbol struture. Symbol contained in symTab.
 */
 
typedef struct symbol_t {
	unsigned int line;
	
	int section;
	unsigned int addr;
	
	char value[STRLEN];
	
} *symbol;

/*!
  \brief : Code struture. After decode, used to struture the chain list in order to build the binary code.
 */

typedef struct code_t {
	unsigned int line;
	int section;
	unsigned int addr;
	int type; /* BYTE WORD ... */
	
	/* Decoded code */
	unsigned int value;
	
}* code;

/*!
  \brief : Relocation struture.
 */

typedef struct rel_t {
	/* Address of relocation */
	unsigned int addr;
	
	/* Type of relocation : R_MIPS_HI16, R_MIPS_LO16.. */
	int type;
	
	/* Target symbol */
	symbol sym;
	
	/* Target code : Used to update code */
	code * c;
	
	/* Relocation section origin */
	int section;
	
	/* Symbol to relocate */
	char value[STRLEN];
	
}* rel;

/*!
  \brief : Type definition of chain. To understand the signification of next / bottom, please read the documentation graphs.
 */
 
typedef struct chain_t {
	
	/* Start at 1. If 0, not defined */
	unsigned int line;
	
	union {
		struct chain_t *bottom;
		lex bottom_lex;
		inst bottom_ins;
		symbol sym;
		code c;
		rel r;
	}this;
	
	struct chain_t *next;
	
} *chain;

/*!
  \brief : A global variable is declared in all files. This variable is used to manage tests. It is defined in top of main.
 */

extern int testID;
extern int section;
extern unsigned int addr;
extern unsigned int line;
extern int typeCode;

#endif /* _GLOBAL_H */













