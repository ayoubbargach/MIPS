
/**
 * @file main.c
 * @author François Portet <francois.portet@imag.fr> from François Cayre
 * @brief Main entry point for MIPS assembler.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>


#include <global.h>
#include <notify.h>
#include <lex.h>
#include <syn.h>
#include <functions.h>



/* Extern variable definition */
int testID = 0;

/**
 * @param exec Name of executable.
 * @return Nothing.
 * @brief Print usage.
 *
 */
void print_usage( char *exec ) {
    fprintf(stderr, "Usage: %s [-lbr] [-t #ID] file.s\n",
            exec);
}



/**
 * @param argc Number of arguments on the command line.
 * @param argv Value of arguments on the command line.
 * @return Whether this was a success or not.
 * @brief Main entry point for MIPS assembler.
 *
 */
int main ( int argc, char *argv[] ) {

    unsigned int 	nlines 	= 0;
    char         	 *file 	= NULL;

    /* exemples d'utilisation des macros du fichier notify.h */
    /* WARNING_MSG : sera toujours affiche*/
    WARNING_MSG("MIPS Assembler");

    /* macro INFO_MSG : uniquement si compilé avec -DVERBOSE. Cf. Makefile*/
    INFO_MSG("Verbose mode %s", argv[0]);

    /* macro DEBUG_MSG : uniquement si compilé avec -DDEBUG (ie : compilation avec make debug). Cf. Makefile */
    DEBUG_MSG("Debug mode");
    
    /* La macro suivante provoquerait l'affichage du message
       puis la sortie du programme avec un code erreur non nul (EXIT_FAILURE) */
	/* ERROR_MSG("Erreur. Arret du programme"); */
    
    /* TODO  make it more robust */
    /* Options Management */

    int opt;
    
    enum { LIST_MODE, OBJECT_MODE, ELF_MODE, TEST_MODE } mode = ELF_MODE;
    
	while ((opt = getopt(argc, argv, "lbrt")) != -1) {
        switch (opt) {
        case 'l':
        	if ( argc <3 ) {
				print_usage(argv[0]);
				exit( EXIT_FAILURE );
			}
			
        	mode = LIST_MODE;
        	
        break;
        case 'b':
        	if ( argc <3 ) {
				print_usage(argv[0]);
				exit( EXIT_FAILURE );
			}
			
        	mode = OBJECT_MODE; 
        break;
        case 'r':
        	if ( argc <3 ) {
				print_usage(argv[0]);
				exit( EXIT_FAILURE );
			}
			
        	mode = ELF_MODE; 
        break;
        case 't': 
        	if ( argc <4 ) {
				print_usage(argv[0]);
				exit( EXIT_FAILURE );
			}
			
        	mode = TEST_MODE;
        	testID = atoi(argv[2]);
        	
        break;
        
        }
    }
    
    if ( argc <2 ) {
		print_usage(argv[0]);
		exit( EXIT_FAILURE );
	}

	/* Final argv (Merci d'y avoir pensé ;) ) */
    file  	= argv[argc-1];


    if ( NULL == file ) {
        fprintf( stderr, "Missing ASM source file, aborting.\n" );
        exit( EXIT_FAILURE );
    }



    /* ---------------- do the lexical analysis -------------------*/
    
    /* We create a collection used to contruct the tree of lexemes */ 
    chain ch = make_collection();
    
    lex_load_file( file, &nlines, ch );
    
    /* ---- TEST 2 ---- */

    /* Dump lexeme chain : */
    
    if (testID == 2) {
    
		chain chcopy = ch;
		chain in;
		
		while (  chcopy != NULL ) {
			in = chcopy;
			in = read_line( in );
			
			if ( in != NULL ) {
				DEBUG_MSG("Line %d", in->line );
			
				do {
				
					if (read_lex( in ) != NULL ) {
						WARNING_MSG("%s", state_to_string( read_lex(in)->type ) );
					}
			
					in = read_line( in );
				} while ( in != NULL );
			
				DEBUG_MSG("[NL]");
			}
			
			chcopy = next_line( chcopy );
			
		}
    }
    
    
    /* Generate the instruction set tab */

	inst instSet[1000];
    instructionSet(instSet);
    
    /* We make the symTab collection */
    chain symTab = make_collection();
    
    /* Some basic variables */
    int section = UNDEFINED;
    unsigned int addr = 0;
    
    /* We make the code collection */
    chain chCode = make_collection();
    chain c = chCode;
    
    
    
    
    
    /* We use this collection for the syntactic analysis */
    
    DEBUG_MSG("source code got %d lines",nlines);

    /* ---------------- Free memory and terminate -------------------*/

    /* TODO free everything properly*/

    exit( EXIT_SUCCESS );
}

