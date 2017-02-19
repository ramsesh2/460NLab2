/*
    Remove all unnecessary lines (including this one) 
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Full name of the first partner 
    Name 2: Full name of the second partner
    UTEID 1: UT EID of the first partner
    UTEID 2: UT EID of the second partner
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/


/*	Instruction Register 16 bits long	*/
int IR[16];


void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */     
   
    /* Fetch the instruction at current PC */
	int currentByte;
	int i;
	int opcode;
	
	/* Least Significant byte of instruction first */
	currentByte = MEMORY[CURRENT_LATCHES.PC / 2][0];

	for (i = 0; i < 8; i++) {
		IR[i] = currentByte % 2;
		currentByte = currentByte / 2;
	}

	/* Most Significant byte of instruction */
	currentByte = MEMORY[CURRENT_LATCHES.PC / 2][1];

	for (i = 0; i < 8; i++) {
		IR[i] = currentByte % 2;
		currentByte = currentByte / 2;
	}

	opcode = 8 * IR[0] + 4 * IR[1] + 2 * IR[2] + 1 * IR[3];

	switch (opcode) {
	case 0: BR(); break; 
	case 1: ADD(); break;
	case 2: LDB(); break;
	case 3: STB(); break; 
	case 4: JSR(); break;
	case 5: AND(); break;
	case 6: LDW(); break;
	case 7: STW(); break;
	/* RTI not implemented */
	case 9: XOR(); break; 
/*	case 10 not used	*/
/*	case 11 not used	*/
	case 12: JMP(); break;
	case 13: SHF(); break;
	case 14: LEA(); break;
	case 15: TRAP(); break;
	default: break;
	}

}

/*	Sign extends bits 15 -> 15-val in the IR	*/
int signEx(int val) {
	int i;
	int sum = 0;

	for (i = 0; i < val; i++) {
		sum = sum + (IR[15 - i] * (1 << i));
	}
	if (IR[16 - val] == 1) {
		sum = sum - (1 << val);
	}

	return sum;
}

/*	Specifies an int with the register number from the three bits in the instruction	*/
int regNum(int start, int fin) {
	int j = 0;
	int i = fin;
	int sum = 0;

	while (i >= start) {
		sum = sum + (IR[i] * (1 << j));
		i--;
		j++;
	}

	return sum;
}

void BR(void) {
	/* if n and N or z and Z or p and P branch */
	if ((CURRENT_LATCHES.N && IR[4]) || (CURRENT_LATCHES.Z && IR[5]) || (CURRENT_LATCHES.P && IR[6])) {
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + (signEx(9) << 1);
	}
	/* else increment like usual */
	else {
		NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
	}
}

void ADD(void) {
	int DR = regNum(4, 6);
	int SR1, SR2;

	SR1 = regNum(7, 9);
	if (IR[10] == 1) {
		SR2 = signEx(5);
		NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + SR2);
	}
	else {
		SR2 = regNum(13, 15);
		NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + CURRENT_LATCHES.REGS[SR2]);
	}
	
	/* condition codes */
	if (NEXT_LATCHES.REGS[DR] == 0) {
		/* sum is zero */
		NEXT_LATCHES.Z = 1;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 0;
	}

	else if ((NEXT_LATCHES.REGS[DR] >> 15) == 0) {
		/* sum is positive */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 1;
		NEXT_LATCHES.N = 0;
	}
	else {
		/* sum is negative */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 1;
	}

	NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC);
}

void LDB(void) {
	int DR, baseR, loc, val;

	DR = regNum(4, 6);
	baseR = regNum(7, 9);
	loc = Low16bits(CURRENT_LATCHES.REGS[baseR] + signEx(6));
	val = MEMORY[loc / 2][loc % 2];

	/* sign extend */
	if ((val >> 7) == 0) {
		NEXT_LATCHES.REGS[DR] = Low16bits(val);
	}
	else NEXT_LATCHES.REGS[DR] = Low16bits(0xFFFFFF00 | val);

	/* condition codes */
	if (NEXT_LATCHES.REGS[DR] == 0) {
		/* val is zero */
		NEXT_LATCHES.Z = 1;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 0;
	}

	else if ((NEXT_LATCHES.REGS[DR] >> 15) == 0) {
		/* val is positive */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 1;
		NEXT_LATCHES.N = 0;
	}
	else {
		/* val is negative */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 1;
	}

	NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
}

void STB(void) {
	int SR, baseR;
	int temp;

	SR = regNum(4, 6);
	baseR = regNum(7, 9);

	temp = Low16bits(CURRENT_LATCHES.REGS[baseR] + signEx(6));
	MEMORY[temp / 2][temp % 2] = CURRENT_LATCHES.REGS[SR] & 0x000000FF;

		NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
}

void JSR(void) {
	int baseR;
	NEXT_LATCHES.REGS[7] = Low16bits(CURRENT_LATCHES.PC);

	if (IR[4] == 1) {
		NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2 + (signEx(11) << 1));
	}
	else {
		baseR = regNum(7, 9);
		NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[baseR]);
	}
}

void AND(void) {
	int DR, SR1, SR2;

	DR = regNum(4, 6);
	SR1 = regNum(7, 9);

	if (IR[10] == 0) { 
		SR2 = regNum(13, 15); 
		NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] & CURRENT_LATCHES.REGS[SR2]);
	}
	else {
		SR2 = signEx(5);
		NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] & SR2);
	}

	/* condition codes */
	if (NEXT_LATCHES.REGS[DR] == 0) {
		/* val is zero */
		NEXT_LATCHES.Z = 1;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 0;
	}

	else if ((NEXT_LATCHES.REGS[DR] >> 15) == 0) {
		/* val is positive */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 1;
		NEXT_LATCHES.N = 0;
	}
	else {
		/* val is negative */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 1;
	}

	NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
}

void LDW(void) {
	int DR, baseR, temp;

	DR = regNum(4, 6);
	baseR = regNum(7, 9);
	temp = Low16bits(CURRENT_LATCHES.REGS[baseR] + (signEx(6) << 1));

	NEXT_LATCHES.REGS[DR] = MEMORY[temp / 2][0];
	NEXT_LATCHES.REGS[DR] = Low16bits(NEXT_LATCHES.REGS[DR] + (MEMORY[temp / 2][1] << 8));

	/* condition codes */
	if (NEXT_LATCHES.REGS[DR] == 0) {
		/* val is zero */
		NEXT_LATCHES.Z = 1;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 0;
	}

	else if ((NEXT_LATCHES.REGS[DR] >> 15) == 0) {
		/* val is positive */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 1;
		NEXT_LATCHES.N = 0;
	}
	else {
		/* val is negative */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 1;
	}

	NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
}

void STW(void) {
	int SR, baseR, temp;

	SR = regNum(4, 6);
	baseR = regNum(7, 9);
	temp = Low16bits(CURRENT_LATCHES.REGS[baseR] + (signEx(6) << 1));

	MEMORY[temp / 2][0] = Low16bits(CURRENT_LATCHES.REGS[SR] & 0x000000FF);
	MEMORY[temp / 2][1] = Low16bits(CURRENT_LATCHES.REGS[SR] >> 8);

	NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
}

void XOR(void) {
	int DR, SR1, SR2;

	DR = regNum(4, 6);
	SR1 = regNum(7, 9);

	if (IR[10] == 0) {
		SR2 = regNum(13, 15);
		NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ CURRENT_LATCHES.REGS[SR2]);
	}
	else {
		NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ signEx(5));
	}


	/* condition codes */
	if (NEXT_LATCHES.REGS[DR] == 0) {
		/* val is zero */
		NEXT_LATCHES.Z = 1;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 0;
	}

	else if ((NEXT_LATCHES.REGS[DR] >> 15) == 0) {
		/* val is positive */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 1;
		NEXT_LATCHES.N = 0;
	}
	else {
		/* val is negative */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 1;
	}

	NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);

}

void JMP(void) {
	int baseR;

	baseR = regNum(7, 9);

	NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[baseR];
}

void SHF(void) {
	int DR, SR, amount, k, temp;

	DR = regNum(4, 6);
	SR = regNum(7, 9);

	amount = regNum(12, 15);

	if (IR[11] == 0) {
		NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR] << amount);
	}
	else if (IR[10] == 0) {
		NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR] >> amount);
	}
	else {
		if ((CURRENT_LATCHES.REGS[SR] >> 15) == 0) NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR] >> amount);
		else {
			temp = CURRENT_LATCHES.REGS[SR];
			for (k = 0; k < amount; k++) {
				temp = Low16bits((temp >> 1) + (1 << 15));
			}
			NEXT_LATCHES.REGS[DR] = temp;
		}
	}

	/* condition codes */
	if (NEXT_LATCHES.REGS[DR] == 0) {
		/* val is zero */
		NEXT_LATCHES.Z = 1;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 0;
	}

	else if ((NEXT_LATCHES.REGS[DR] >> 15) == 0) {
		/* val is positive */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 1;
		NEXT_LATCHES.N = 0;
	}
	else {
		/* val is negative */
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 0;
		NEXT_LATCHES.N = 1;
	}

	NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);

}

void LEA(void) {
	int DR;

	DR = regNum(4, 6);

	NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.PC + 2 + (signEx(9) << 1));
	NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
}

void TRAP(void) {
	int trapvect8;

	NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC + 2;
	NEXT_LATCHES.PC = Low16bits(signEx(8));

	trapvect8 = regNum(8, 15);
	
	trapvect8 = Low16bits(trapvect8 << 1);
	NEXT_LATCHES.PC = MEMORY[trapvect8 / 2][0];
	NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + (MEMORY[trapvect8 / 2][1] << 8));
}