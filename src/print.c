
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
	int j = 3;
	int k = 0;
	int n = 2;
	int intCode = 0;
	char strPrint[STRLEN] = "";
	char strNumber[STRLEN];
	int printed = 0;
	
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
						
						/* In our project, bss can only be used with directive space */
						
						if (codes->section == BSS) {
							fprintf(fp,"%3u %08X %s %s",i,codes->addr,"0000... ",source_line);
						
							/* We read all lines */
							while ( chCode != NULL && chCode->line == i ) {
								chCode = read_next(chCode);
							}
						}
						else {
							
							
							if (codes != NULL && codes->type == BYTE) {
								if ( read_next(chCode) != NULL && read_next(chCode)->line == i ) {
									intCode = codes->value << (j*2*4);
									j--;
								}
								else {
									intCode = codes->value;
								}
								
								while ( read_next(chCode) != NULL && read_next(chCode)->line == i ) {
									chCode = read_next(chCode);
									codes = getCode( chCode );
									
									if ( j >= 0) { /* Branch used to print correctly the code */
										intCode = intCode + (codes->value << (j*2*4));
										j--;
									}
									else {
										fprintf(fp,"%3u %08X %08X %s",i,codes->addr-4,intCode,source_line);
										intCode = codes->value;
										printed = 1;
										j=3;
									}
								
								
									
								}
								
								if ( j > 0 ) {
									strcat(strPrint, "%3u %08X %0");
									strNumber[0] = '8' - j * 2 ;
									strNumber[1] = 'X';
									k = j;
									
									while ( k != 0 ) {
										strNumber[n] = ' ';
										k--;
										n++;
									}
									
									strNumber[n] = '\0' ;
									strcat(strPrint, strNumber);
									strcat(strPrint, " %s");
									
									if ( printed ) {
										strcpy(source_line,"\n");
										printed = 0;
									}
									
									
									fprintf(fp,strPrint,i,codes->addr,intCode,source_line);
									
								}
								
								/* Reinit */
								strPrint[0] = '\0';
								
								
								chCode = read_next(chCode);
							
							}
							else {
								fprintf(fp,"%3u %08X %08X %s",i,codes->addr,codes->value,source_line);
						
								chCode = read_next(chCode);
						
								while ( chCode != NULL && chCode->line == i ) {	
									codes = getCode( chCode );
									fprintf(fp,"%3u %08X %08X %s\n",i,codes->addr,codes->value,"");
									chCode = read_next(chCode);
								}
							}
							
							
							
							
							
							
						}
					}
					else {
						/* We only print source_line */
						fprintf(fp,"%3u %s %s %s",i,"        ","        ",source_line);
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


