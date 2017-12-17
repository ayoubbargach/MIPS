
/**
 * @file main.c
 * @author Bargach Ayoub <ayoub.bargach@phelma.grenoble-inp.fr> from François Portet <francois.portet@imag.fr> and François Cayre
 * @brief Main entry point for MIPS assembler.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>


#include <global.h>
#include <notify.h>


#include <functions.h>
#include <lex.h>
#include <inst.h>
#include <syn.h>
#include <eval.h>
#include <print.h>



/* Extern variable definition */
int testID = 0;
int section = UNDEFINED;
unsigned int addr = 0;
unsigned int line = 1;

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
    
    /* ---------------- Options Management -------------------*/

    int opt;
    int mode = ELF_MODE;
    
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
    
    /* ---------------- init all collections -------------------*/
    
    /* We create a collection used to contruct the tree of lexemes */ 
    chain chLex = make_collection();
    
    /* We make the symTab collection */
    chain symTab = make_collection();
    
    /* We make the code collection */
    chain chCode = make_collection();
    
    /* We make the relocation table collection */
    chain chRel = make_collection();
    
    /* We create an array ro reach easily the starting point */
    chain source[4] = {chLex, symTab, chCode, chRel};
    chain * c[4] = {&chLex, &symTab, &chCode, &chRel};
    
    
    /* ---------------- do the lexical analysis -------------------*/
    
    lex_load_file( file, &nlines, chLex );
    
    /* ---- TEST 2 ---- */

    /* Dump lexeme chain : */
    
    if (testID == 2) {
    
		chain chcopy = chLex;
		chain in;
		
		while (  chcopy != NULL ) {
			in = chcopy;
			in = read_next( in );
			
			if ( in != NULL ) {
				DEBUG_MSG("Line %d", in->line );
			
				do {
				
					if (read_lex( in ) != NULL ) {
						WARNING_MSG("%s", state_to_string( read_lex(in)->type ) );
					}
			
					in = read_next( in );
				} while ( in != NULL );
			
				DEBUG_MSG("[NL]");
			}
			
			chcopy = read_bottom( chcopy );
			
		}
    }
    
    /* ---------------- init instruction set - See inst.h -------------------*/
    
    /* Generate the instruction set tab */
	inst instSet[1000] = {NULL};
    instructionSet(instSet);
    
    /* ---------------- do the syntactic analysis - See syn.h -------------------*/
    
    
    /* We fetch each line */
    while ( chLex != NULL && read_next( chLex ) != NULL ) {
    	fetch(c, instSet);
    	
    	chLex = read_bottom( chLex );
    	
    	/* WARNING_MSG("Type => %p", chLex );
    	
    	lex l = read_lex( read_next( chLex ));
    	
    	WARNING_MSG("Type => %s", state_to_string(l->type)); */
    }
    
    /* SOLVE relocations section */
    /* Here we need to use reloc chain and Code chain to solve relocation and delete relative ones */
    
    solve( symTab, source[2], source[3] );


    /* ---------------- print results - See print.h -------------------*/
    print( source, mode, nlines, file );
    
    
    
    
    /* We use this collection for the syntactic analysis */
    
    DEBUG_MSG("source code got %d lines",nlines);

    /* ---------------- Free memory and terminate -------------------*/

    /* TODO free everything properly*/

    exit( EXIT_SUCCESS );
}

