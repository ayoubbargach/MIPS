  1	        	        	# allons au ru
  2	        	        	.set noreorder
  3	        	        	.text
  4	00000000	3C010000	    Lw $t0 , lunchtime
  4	00000004	8C280000	
  5	00000008	8CE6FE00	    LW $6, -0x200($7)
  6	0000000C	20090008	    ADDI $t1,$zero,8
  7	        	        	boucle:
  8	00000010	11090004	    BEQ $t0 , $t1 , byebye
  9	00000014	00000000	    NOP
 10	00000018	21290001	    addi $t1 , $t1 , 1
 11	0000001C	08000004	    J boucle 
 12	00000020	00000000	    NOP
 13	        	        	byebye:
 14	00000024	0C000000	    JAL viteviteauru
 15	        	        	.data
 16	00000000	0000000C	lunchtime : .word 12

.symtab
  7	.text:00000010	boucle
 13	.text:00000024	byebye
 14	[UNDEFINED]	viteviteauru
 16	.data:00000000	lunchtime

rel.text
00000000	R_MIPS_HI16	.data:00000000	lunchtime
00000004	R_MIPS_LO16	.data:00000000	lunchtime
0000001c	R_MIPS_26	.text:00000010	boucle
00000024	R_MIPS_26	[UNDEFINED]	viteviteauru

rel.data
