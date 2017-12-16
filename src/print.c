
/**
 * @file print.c
 * @author Ayoub Bargach
 * @brief MIPS assembly print.
 *
 * These routines perform the printing respecting in which mode the program is configured.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <functions.h>

#include <eval.h>
#include <syn.h>
#include <lex.h>
#include <print.h>

/**
 * @param c the tab with all inital chain collections pointers.
 * @param mode Output mode
 * @param nline Total lines.
 * @return nothing
 * @brief Using chains, print according to mode.
 */
 
void print( chain * c, int mode, int nlines, char* file ) {
	FILE *fp = NULL;
	FILE *fo = NULL;
	char source_line[STRLEN];
	int i = 1;
	
	/* INIT */
	symbol sym;
	chain symTab = c[1];
	chain chCode = c[2];
	chain chRel = c[3];
	
	switch (mode) {
		case LIST_MODE :
			fp = fopen("file.l", "w+");
			
			fo = fopen(file, "r");
			
			if ( fo == NULL ) {
				ERROR_MSG("Error while trying to open %s file a second time (Deleted while program is running ?) --- Aborts",file);
			}
			
			/* Print code */
			for (i=1; i<nlines; i++) {
				
				if ( NULL != fgets( source_line, STRLEN-1, fp ) ) {
					
					/* We get the first code */
					chCode = read_next(chCode);
				
					WARNING_MSG("%3u␣%08X␣%08X␣%s\n",i,address,code,source_line);
				}
				
				
				/* fprintf(fp,"%3u␣%08X␣%08X␣%s\n",i,address,code,source_line); */
				
				/* fprintf(fp,"%08x\t%s\t.%-4s:%08x\t%s\n",address,type_reloc,sym_section,sym_address,sym_name); */
				
			}
			
			/* Print symbol table */
			while (symTab != NULL) {
			
				sym = readSymbol( symTab );
				
				if (sym != NULL) {
					WARNING_MSG("%3d\t.%-4s:%08X\t%s\n", sym->line, section_to_string( sym->section ), sym->addr, sym->value);
					fprintf(fp,"%3d\t.%-4s:%08X\t%s\n", sym->line, section_to_string( sym->section ), sym->addr, sym->value);
				}
				symTab = read_next( symTab );
			}
			
			fclose(fp);
			WARNING_MSG("LIST mode - file.l generated");
		break;
		
		case OBJECT_MODE :
			fp = fopen("file.obj", "w+");
			
			
			fclose(fp);
			WARNING_MSG("OBJECT mode - file.obj generated");
		break;
		
		case TEST_MODE :
			WARNING_MSG("Test mode END");
		break;
		
		default:
			fp = fopen("file.o", "w+");
			
			
			fclose(fp);
			WARNING_MSG("ELF mode - file.o generated");
		break;
	}
	
	
	
	
	return;
}




/**
 * @param 
 * @return 
 * @brief 
 *
 */
 
char* section_to_string(int section) {
	switch (section) {
    		case TEXT :
    			return "TEXT";
    			break;
    		
    		case DATA :
    			return "DATA";
    			break;
    			
    		default :
    			return "BSS";
				break;
	}
}



