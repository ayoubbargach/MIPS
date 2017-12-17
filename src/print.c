
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
	chain chCode = read_next( c[2] );
	chain chRel = read_next( c[3] );
	
	code codes;
	
	switch (mode) {
		case LIST_MODE :
			fp = fopen("file.l", "w+");
			
			fo = fopen(file, "r");
			
			if ( fo == NULL ) {
				ERROR_MSG("Error while trying to open %s file a second time (Deleted while program is running ?) --- Aborts",file);
			}
			
			/* Print code */
			for (i=1; i<nlines+1; i++) {
				
				if ( NULL != fgets( source_line, STRLEN-1, fo ) ) {
					
					/* We get the first code */
					codes = getCode( chCode );
					
					if ( codes->line == i ) {
						fprintf(fp,"%3u\t%08X\t%08X\t%s",i,codes->addr,codes->value,source_line);
						
						chCode = read_next(chCode);
						
						while ( chCode != NULL && chCode->line == i ) {
							codes = getCode( chCode );
							fprintf(fp,"%3u\t%08X\t%08X\t%s\n",i,codes->addr,codes->value,"");
							chCode = read_next(chCode);
						}
					}
					else {
						/* We only print source_line */
						fprintf(fp,"%3u\t%s\t%s\t%s",i,"        ","        ",source_line);
					}
					
					
				}
				
				
				/* fprintf(fp,"%3u␣%08X␣%08X␣%s\n",i,address,code,source_line); */
				
			}
			
			/* Print symbol table */
			fprintf(fp,"\n.symtab\n");
			while (symTab != NULL) {
			
				sym = readSymbol( symTab );
				
				if (sym != NULL) {
					if (sym->section == NONE )
						fprintf(fp,"%3d\t%-4s\t%s\n", sym->line, section_to_string( sym->section ), sym->value);
					else
						fprintf(fp,"%3d\t%-4s:%08X\t%s\n", sym->line, section_to_string( sym->section ), sym->addr, sym->value);
				}
				symTab = read_next( symTab );
			}
			
			
			
			/* Print Reloc table */
			
			
			fprintf(fp,"\nrel.text\n");
			while ( chRel != NULL ) {
				rel r = readRel( chRel );
				
				sym = r->sym;
				
				
				if (r->section == TEXT) {
					if (sym->section == NONE )
						fprintf(fp,"%08x\t%s\t%-4s\t%s\n", r->addr, rel_to_string( r->type ), section_to_string( sym->section ), sym->value);
					else
						fprintf(fp,"%08x\t%s\t%-4s:%08x\t%s\n", r->addr, rel_to_string( r->type ), section_to_string( sym->section ), sym->addr, sym->value);
					
				}
					
				
				chRel = read_next( chRel );
			}
			
			
			
			fprintf(fp,"\nrel.data\n");
			chRel = read_next( c[3] );
			while ( chRel != NULL ) {
				rel r = readRel( chRel );
				
				sym = r->sym;
				
				if (r->section == DATA) {
					if (sym->section == NONE )
						fprintf(fp,"%08x\t%s\t%-4s\t%s\n", r->addr, rel_to_string( r->type ), section_to_string( sym->section ), sym->value);
					else
						fprintf(fp,"%08x\t%s\t%-4s:%08x\t%s\n", r->addr, rel_to_string( r->type ), section_to_string( sym->section ), sym->addr, sym->value);
					
				}
				
				chRel = read_next( chRel );
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
    			return ".text";
    			break;
    		
    		case DATA :
    			return ".data";
    			break;
    			
    		case BSS :
    			return ".bss";
				break;
			
			default :
    			return "[UNDEFINED]";
				break;
	}
}


/**
 * @param 
 * @return 
 * @brief 
 *
 */
 
char* rel_to_string(int section) {
	switch (section) {
    		case R_MIPS_32 :
    			return "R_MIPS_32";
    			break;
    		
    		case R_MIPS_26 :
    			return "R_MIPS_26";
    			break;
    			
    		case R_MIPS_HI16 :
    			return "R_MIPS_HI16";
				break;
				
			case R_MIPS_LO16 :
    			return "R_MIPS_LO16";
				break;
			
			case RELATIVE :
    			return "RELATIVE";
				break;
			
			default :
    			return "NONE";
				break;
	}
}


