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
#include <math.h>

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

int power (int a, int b) {
	if (b==0) return 1;
	
	int num = 1;
	for(int i = 0; i < b; i++) {
		num = num*a;
	}
	return num;
}





/***************************************************************** 
Returns decimal offset (PCoffset, offset6, imm5, etc.)
******************************************************************/
int convertOffset(int bitrep[16], int index, int width) {
	int decOffset = 0;
	int isNegative = 0;
	// Check if offset is negative
	if(bitrep[index] == 1) {
		isNegative = 1;
		for(int j = 0; j < width - 1; j++) {
			bitrep[j] = !bitrep[j];
		}
	}
	for (int i = 0; i < width - 1; i++ ) {
		int bitVal = bitrep[i]*power(2,i);
		decOffset += bitVal;
	}
	if(isNegative) {
		decOffset = decOffset + 1;
		decOffset = decOffset*(-1);
	}
	return decOffset;
}

/***************************************************************/
/* Returns bit representation where bitrep[16] is the 
   most significant bit. */
/***************************************************************/
void decToBitRep(int decLSB, int decMSB, int bitrep[16]) {
	// least significant bits
	int curr_byte = decLSB;
	for (int i = 0; i < 8; i++) {
		int bit = curr_byte % 2;
		curr_byte = curr_byte / 2;
		bitrep[i] = bit;
	}
	// most significant bits
	curr_byte = decMSB;
	for (int j = 8; j < 16; j++) {
		int bit = curr_byte % 2;
		curr_byte = curr_byte / 2;
		bitrep[j] = bit;
	} 
}

/*
Returns a register number in range 0-7. 
Requires the bit representation and the high bit index for the register.
*/

int getRegisterNumber(int bitrep[16], int index) {
	int regNum = bitrep[index]*pow(2,2) + bitrep[index-1]*pow(2,1) + bitrep[index-2]*pow(2,0);
	return regNum;

}

/*
 * Set condition codes based on value obtained (usually in DR).
 */
void setCC (int value) {
    if (value > 0) {
        NEXT_LATCHES.P = 1;
    }
    else {
        NEXT_LATCHES.P = 0;
    }
    if (value == 0) {
        NEXT_LATCHES.Z = 1;
    }
    else {
        NEXT_LATCHES.Z = 0;
    }
    if (value < 0) {
        NEXT_LATCHES.N = 1;
    }
    else {
        NEXT_LATCHES.N = 0;
    }
}

/*
 * Returns unsigned bit representation to decimal value.
 * Requires high bit index and number of bits.
 */
int getUnsignedValue(int bitrep[16], int index, int width) {
    int unsignedValue = 0;
    for (int i = 0; i < width; i++) {
        unsignedValue += bitrep[i]*power(2,i);
    }
    return unsignedValue;
}


void add(int bitrep[16]) {
    printf("Reached add\n");

    int DR = getRegisterNumber(bitrep, 11);
	int SR1 = getRegisterNumber(bitrep, 8);
	

	int result;
	if (bitrep[5] == 0) {
		int SR2 = getRegisterNumber(bitrep,2);
		result = CURRENT_LATCHES.REGS[SR1] + CURRENT_LATCHES.REGS[SR2];
		NEXT_LATCHES.REGS[DR] = Low16bits(result);
	}
	else {
        int imm5 = convertOffset(bitrep, 4, 5);
        result = CURRENT_LATCHES.REGS[SR1] + imm5;
        NEXT_LATCHES.REGS[DR] = Low16bits(result);
    }
	//set condition codes;
	setCC(result);
}


void and(int bitrep[16]) {
    printf("Reached and\n");
    int DR = getRegisterNumber(bitrep, 11);
    int SR1 = getRegisterNumber(bitrep, 8);

    int result;
    if (bitrep[5] == 0) {
        int SR2 = getRegisterNumber(bitrep,2);
        result = CURRENT_LATCHES.REGS[SR1] & CURRENT_LATCHES.REGS[SR2];
        NEXT_LATCHES.REGS[DR] = Low16bits(result);
    }
    else {
        int imm5 = convertOffset(bitrep, 4, 5);
        result = CURRENT_LATCHES.REGS[SR1] & imm5;
        NEXT_LATCHES.REGS[DR] = Low16bits(result);
    }
    // set condition codes;
    setCC(result);
}

void br(int bitrep[16]){
    printf("Reached br\n");
    if (bitrep[11] && CURRENT_LATCHES.N || bitrep[10] && CURRENT_LATCHES.Z || bitrep[9] && CURRENT_LATCHES.P) {
        int PCoffset9 = convertOffset(bitrep, 8, 9);
        int lshfPCOffset9 = PCoffset9 << 1;
        int newPC = NEXT_LATCHES.PC + lshfPCOffset9;
        NEXT_LATCHES.PC = newPC;
    }
    // clear condition bits
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
}

void trap_(int bitrep[16]) { // alternatively, you can set PC = 0! But instructions say to implement like in Appendix A
    printf("Reached trap\n");
    // R7 = PC
    NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC;

    int unsignedValue = getUnsignedValue(bitrep, 7, 8);
    int lshfUnsignedValue = unsignedValue << 1;
    int address = lshfUnsignedValue >> 1;
    if(unsignedValue == 0x25) { // Reached HALT
        NEXT_LATCHES.PC = MEMORY[address][0];
    }
    int pcValue = MEMORY[address][0]; // the trap vector table will be set to 0
    NEXT_LATCHES.PC = pcValue;
}

void jsr_(int bitrep[16]) {
    // make sure you GET RIGHT LATCHES... change below to NEXT_LATCHES???
    int temp = NEXT_LATCHES.PC;
    if (bitrep[11] == 0) {
        int baseR = getRegisterNumber(bitrep, 8);
        NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[baseR];
    }
    else if (bitrep[11]){
        int PCoffset11 = convertOffset(bitrep, 10, 11);
        NEXT_LATCHES.PC = NEXT_LATCHES.PC + (PCoffset11 << 1);
    }
    NEXT_LATCHES.REGS[7] = temp;
}

void ldb(int bitrep[16]) {
    int DR = getRegisterNumber(bitrep, 11);
    int BaseR = getRegisterNumber(bitrep, 8);
    int boffset6 = convertOffset(bitrep, 5, 6);
    int isOdd = 0;
    int valueToLoad = 0xDEADBEEF;

    // size of memory location is 4 bytes ~ 32 bits
    int BaseRValue = CURRENT_LATCHES.REGS[BaseR];
    int baseAddress = BaseRValue >> 1;

    // if offset is odd, access the MSB
    if(boffset6 %2 == 1) {
        isOdd = 1;
    }
    boffset6 = boffset6 >> 1;
    int address = baseAddress + boffset6;
    if (isOdd) {
        valueToLoad = MEMORY[address][1] & 0xFF;
        valueToLoad = valueToLoad << 24;         // sign extend
        valueToLoad = valueToLoad >> 24;
    } else {
        valueToLoad = MEMORY[address][0] & 0xFF;
        valueToLoad = valueToLoad << 24;         // sign extend
        valueToLoad = valueToLoad >> 24;
    }
    NEXT_LATCHES.REGS[DR] = Low16bits(valueToLoad); //  & 0xFF;
    setCC(valueToLoad);
}

void ldw(int bitrep[16]) {
    int DR = getRegisterNumber(bitrep, 11);
    int BaseR = getRegisterNumber(bitrep, 8);
    int offset6 = convertOffset(bitrep, 5, 6);
    int BaseRValue = CURRENT_LATCHES.REGS[BaseR];
    int valueToLoadLSB = Low16bits(MEMORY[(BaseRValue >> 1) + offset6][0]);
    int valueToLoadMSB = Low16bits(MEMORY[(BaseRValue >> 1) + offset6][1]);

    // MSB + LSB = Word
    int valueToLoad = 0x00FF & valueToLoadLSB; // valueToLoad gets low byte
    valueToLoadMSB = 0xFF00 & ( valueToLoadMSB << 8 ); // MSB shifted to high byte
    valueToLoad = valueToLoadMSB | valueToLoad; // valueToLoad is MSB + LSB
    valueToLoad = valueToLoad << 16;
    valueToLoad = valueToLoad >> 16;
    NEXT_LATCHES.REGS[DR] = Low16bits(valueToLoad);
    setCC(valueToLoad);
}

void lea(int bitrep[16]){
    //get register
    //find pcoffset
    //put into register

    int DR = getRegisterNumber(bitrep, 11);
    int off = convertOffset(bitrep, 8, 9);
    NEXT_LATCHES.REGS[DR] = NEXT_LATCHES.PC + (off << 1); //remember to multiple offset by 2)
}

void nop(int bitrep[16]){
    //nothing, just increment PC
}

void notxor(int bitrep[16]){
    int DR = getRegisterNumber(bitrep, 11);
    int SR1 = getRegisterNumber(bitrep, 8);

    if(bitrep[5] == 0){
        int imm = convertOffset(bitrep, 4,5);
        if(imm == -15){
            //not...
            int dec = ~(CURRENT_LATCHES.REGS[SR1]);
            NEXT_LATCHES.REGS[DR] = Low16bits(dec);
        }else{
            //xor with imm5
            int dec = (CURRENT_LATCHES.REGS[SR1]) ^ imm;
            NEXT_LATCHES.REGS[DR] = Low16bits(dec);
        }
    }else{
        int SR2 = getRegisterNumber(bitrep, 2);
        int dec = (CURRENT_LATCHES.REGS[SR1]) ^ (CURRENT_LATCHES.REGS[SR2]);
        NEXT_LATCHES.REGS[DR] = Low16bits(dec);
    }
}

void retjmp(int bitrep[16]){
    int br = getRegisterNumber(bitrep, 8);
    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[br];
}

void lshf(int bitrep[16]){
    int DR = getRegisterNumber(bitrep, 11);
    int SR = getRegisterNumber(bitrep, 8);
    int shift = convertOffset(bitrep, 3, 4);
    int dec = CURRENT_LATCHES.REGS[SR] << shift;
    NEXT_LATCHES.REGS[DR] = Low16bits(dec);
}

void rshfl(int bitrep[16]){
    int DR = getRegisterNumber(bitrep, 11);
    int SR = getRegisterNumber(bitrep, 8);

    int shift = convertOffset(bitrep, 3, 4);
    int val = CURRENT_LATCHES.REGS[SR];
    if(val < 0){
        int shift_and = 0;
        for(int sh = 0; sh < shift; sh++){
            shift_and = shift_and + power(2, sh);
        }
        shift_and = ~(shift_and << (16 - shift));
        val = val & shift_and;
        NEXT_LATCHES.REGS[DR] = Low16bits(val);

    }else{
        val = val >> shift;
        NEXT_LATCHES.REGS[DR] = Low16bits(val);
    }
}

void rshfa(int bitrep[16]){
    int DR = getRegisterNumber(bitrep, 11);
    int SR = getRegisterNumber(bitrep, 8);
    int shift = convertOffset(bitrep, 3,4);
    int dec = CURRENT_LATCHES.REGS[SR] >> shift;
    NEXT_LATCHES.REGS[DR] = Low16bits(dec);
}

void stb(int bitrep[16]){
    int SR = getRegisterNumber(bitrep, 11);
    int BR = getRegisterNumber(bitrep, 8);
    int offset = convertOffset(bitrep, 5, 6);

    int dec = Low16bits(CURRENT_LATCHES.REGS[SR]);
    dec = (0x00FF) & dec;
    int base = CURRENT_LATCHES.REGS[BR];
    base = base + (offset *2);
    MEMORY[base][0] = dec;
    MEMORY[base][1] = 0x0000;
}

void stw(int bitrep[16]){
    int SR = getRegisterNumber(bitrep, 11);
    int BR = getRegisterNumber(bitrep, 8);
    int offset = convertOffset(bitrep, 5, 6);

    int dec = Low16bits(CURRENT_LATCHES.REGS[SR]);
    int MSB = (dec >> 8) & 0xFF00;
    int LSB = dec & 0x00FF;

    int base = CURRENT_LATCHES.REGS[BR];
    base = base + (offset * 2);
    MEMORY[base][0] = LSB;
    MEMORY[base][1] = MSB;

}


void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */     
	printf("CURRENT PC: %d\n", CURRENT_LATCHES.PC);
	printf("LSB: %x\n", MEMORY[CURRENT_LATCHES.PC >> 1][0]); 	
	printf("MSB: %x\n", MEMORY[CURRENT_LATCHES.PC >> 1][1]);
   
	int decLSB = MEMORY[CURRENT_LATCHES.PC >> 1][0];
	int decMSB = MEMORY[CURRENT_LATCHES.PC >> 1][1];

	if (decLSB == 0 && decMSB == 0) printf("PC is 0. Shell will halt simulation.");

	int bitrep[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	decToBitRep(decLSB, decMSB, bitrep);

    // increment PC
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;

    // check opcode
	int opcode = bitrep[15]*power(2,3) + bitrep[14]*power(2,2) + bitrep[13]*power(2,1) + bitrep[12]*power(2,0);
	switch (opcode) {
        case(0):
            br(bitrep); // handles nop
            break;
	    case(1):
			add(bitrep);
            break;
        case(2):
            ldb(bitrep);
            break;
        case(4):
            jsr_(bitrep);
            break;
        case(3):
            stb(bitrep);//stb
            break;
        case(5):
            and(bitrep);
            break;
        case(6):
            ldw(bitrep);
            break;
        case(7):
            stw(bitrep);//stw
            break;
        case(9):
            notxor(bitrep);//not or xor
            break;
        case(12):
            retjmp(bitrep);// jmp or ret
            break;
        case(13):
            //lshf, rshfl, rshfa
            if((bitrep[5] + bitrep[4]) == 0){
                //lshf
                lshf(bitrep);
            }else if((bitrep[5] + bitrep[4]) == 1){
                //logical
                rshfl(bitrep);
            }else{
                //arithmetic
                rshfa(bitrep);
            }
            break;
        case(14):
            lea(bitrep);//lea
            break;
        case(15):
            trap_(bitrep); // handles halt
            break;

		default:
			printf("Invalid opcode");
			break;
	}
}
