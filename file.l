  1	        	        	## Fichier mult.s : un programme qui fait une division et une multiplication
  2	        	        	.text            
  3	        	        	.set noreorder		#pas de réordonnancement
  4	        	        	
  5	        	        	##----------------------------------------------------------------------------
  6	        	        	# initialise les variables
  7	00000000	20190000	  ADDI  $t9,$zero,EXIT
  8	00000004	200AFFD5	  addi  $t2,$zero,-	43     	# $t2 <- -43
  9	00000008	200BFFFF	  addi  $t3,	$0,0xffff      	# $t3 <- 0xff
 10	        	        	
 11	        	        	#fait la division
 12	0000000C	014B001A	  DIV  $t2,$t3			# divise les deux nombres
 13	00000010	00004012	  mflo $t0 			# prend le quotient du résultat de la division
 14	00000014	00004810	  mfhi $t1 			# prend le reste du résultat de la division
 15	00000018	11200000	  BEQ $t1,$zero, mult           # si il n'y a pas de reste alors on peut tester dans l'autre sens
 16	0000001C	00005020	  add $t2,$zero,$zero		# si pas réussi on set $t2 à 0
 17	00000020	08000000	  J EXIT			# saut à la sortie sinon  		
 18	        	        	
 19	        	        	#fait la multiplication (remarquez le nom de l'étiquette)
 20	        	        	mult: 
 21	00000024	00000000	  NOP				# quelques non opérations pour respecter la consigne 
 22	00000028	00000000	  nop				# de la doc concernant le MFHI
 23	0000002C	010B0018	  mult $t0,$t3			# on essaye de retrouver le nombre de départ
 24	00000030	00004012	  MFLO $t0 			# prend la partie basse de la multiplication
 25	00000034	00004810	  MFHI $t1 			# prend la partie haute de la multiplication
 26	00000038	00006820	  add $t5,$zero,$zero		# astuce pour éviter les optimisation de boucle
 27	0000003C	152D0000	  BNE $t1,$t5,EXIT		# si le résultat est trop grand on sort
 28	00000040	110A0000	  BEQ $t0,$t2,reussi		# si $t2 et $t0 sont égaux on a retrouvé le résultat
 29	00000044	00005020	  add $t2,$zero,$zero		# si pas réussi on set $t2 à 0 et on sort
 30	00000048	08000000	  J EXIT			
 31	        	        	reussi:
 32	0000004C	200A0001	  addi $t2,$zero,0x1		# si réussi on set $t2 à 1
 33	00000050	08000000	  J EXIT			# et on sort
 34	        	        	
 35	        	        	
 36	        	        	EXIT	:
 37	00000054	0000000C	syscall
 38	        	        	## The End
 39	        	        	
 40	        	        	.data 
 41	00000000	0000000C	.byte 12,0xAA,0xBB,0xCC,0xdd
 41	00000001	000000AA	
 41	00000002	000000BB	
 41	00000003	000000CC	
 41	00000004	000000DD	
 42	00000005	000000FF	.byte 0xFF
 43	00000006	AABBCCDD	.word 0xAABBCCDD

.symtab
 36	.text:00000054	EXIT
 20	.text:00000024	mult
 31	.text:0000004C	reussi

rel.text
00000020	R_MIPS_26	.text:00000054	EXIT
0000003c	NONE	.text:00000054	EXIT
00000048	R_MIPS_26	.text:00000054	EXIT
00000050	R_MIPS_26	.text:00000054	EXIT

rel.data
