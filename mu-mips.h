#include <stdint.h>

#define FALSE 0
#define TRUE  1
int ENABLE_FORWARDING = 0;

/******************************************************************************/
/* MIPS memory layout                                                                                                                                      */
/******************************************************************************/
#define MEM_TEXT_BEGIN  0x00400000
#define MEM_TEXT_END      0x0FFFFFFF
/*Memory address 0x10000000 to 0x1000FFFF access by $gp*/
#define MEM_DATA_BEGIN  0x10010000
#define MEM_DATA_END   0x7FFFFFFF

#define MEM_KTEXT_BEGIN 0x80000000
#define MEM_KTEXT_END  0x8FFFFFFF

#define MEM_KDATA_BEGIN 0x90000000
#define MEM_KDATA_END  0xFFFEFFFF

/*stack and data segments occupy the same memory space. Stack grows backward (from higher address to lower address) */
#define MEM_STACK_BEGIN 0x7FFFFFFF
#define MEM_STACK_END  0x10010000

typedef struct {
	uint32_t begin, end;
	uint8_t *mem;
} mem_region_t;

/* memory will be dynamically allocated at initialization */
mem_region_t MEM_REGIONS[] = {
	{ MEM_TEXT_BEGIN, MEM_TEXT_END, NULL },
	{ MEM_DATA_BEGIN, MEM_DATA_END, NULL },
	{ MEM_KDATA_BEGIN, MEM_KDATA_END, NULL },
	{ MEM_KTEXT_BEGIN, MEM_KTEXT_END, NULL }
};

#define NUM_MEM_REGION 4
#define MIPS_REGS 32

typedef struct CPU_State_Struct {

  uint32_t PC;		                   /* program counter */
  uint32_t REGS[MIPS_REGS]; /* register file. */
  uint32_t HI, LO;                          /* special regs for mult/div. */
} CPU_State;

typedef struct CPU_Pipeline_Reg_Struct{
	uint32_t PC;
	uint32_t IR;
	uint32_t A;
	uint32_t B;
	uint32_t imm;
	uint32_t ALUOutput;
	uint32_t LMD;

} CPU_Pipeline_Reg;

/***************************************************************/
/* CPU State info.                                                                                                               */
/***************************************************************/

CPU_State CURRENT_STATE, NEXT_STATE;
int RUN_FLAG;	/* run flag*/
uint32_t INSTRUCTION_COUNT;
uint32_t CYCLE_COUNT;
uint32_t PROGRAM_SIZE; /*in words*/


/***************************************************************/
/* Pipeline Registers.                                                                                                        */
/***************************************************************/
CPU_Pipeline_Reg IF_ID; //changed name
CPU_Pipeline_Reg ID_EX; //changed name
CPU_Pipeline_Reg EX_MEM;
CPU_Pipeline_Reg MEM_WB;

char prog_file[32];


/***************************************************************/
/* Function Declerations.                                                                                                */
/***************************************************************/
void help();
uint32_t mem_read_32(uint32_t address);
void mem_write_32(uint32_t address, uint32_t value);
void cycle();
void run(int num_cycles);
void runAll();
void mdump(uint32_t start, uint32_t stop) ;
void rdump();
void handle_command();
void reset();
void init_memory();
void load_program();
void handle_pipeline(); /*IMPLEMENT THIS*/
void WB();/*IMPLEMENT THIS*/
void MEM();/*IMPLEMENT THIS*/
void EX();/*IMPLEMENT THIS*/
void ID();/*IMPLEMENT THIS*/
void IF();/*IMPLEMENT THIS*/
void show_pipeline();/*IMPLEMENT THIS*/
void initialize();
void print_program(); /*IMPLEMENT THIS*/ /*Implemented (10/20/2022)*/


/*********************************************/
/* Student Added Functions                   */
/*********************************************/

//return requested register given the binary representiaiton of the register name
char* which_reg(char* reg_string){

	if(strncmp(reg_string,"00000",5)==0){
		return("$zero");
	}

	else if(strncmp(reg_string,"00001",5)==0){
		return("$at");
	}

	else if(strncmp(reg_string,"00010",5)==0){
		return("$v0");
	}

	else if(strncmp(reg_string,"00011",5)==0){
		return("$v1");
	}

	else if(strncmp(reg_string,"00100",5)==0){
		return("$a0");
	}

	else if(strncmp(reg_string,"00101",5)==0){
		return("$a1");
	}

	else if(strncmp(reg_string,"00110",5)==0){
		return("$a2");
	}

	else if(strncmp(reg_string,"00111",5)==0){
		return("$a3");
	}

	else if(strncmp(reg_string,"01000",5)==0){
		return("$t0");
	}

	else if(strncmp(reg_string,"01001",5)==0){
		return("$t1");
	}

	else if(strncmp(reg_string,"01010",5)==0){
		return("$t2");
	}

	else if(strncmp(reg_string,"01011",5)==0){
		return("$t3");
	}

	else if(strncmp(reg_string,"01100",5)==0){
		return("$t4");
	}

	else if(strncmp(reg_string,"01101",5)==0){
		return("$t5");
	}

	else if(strncmp(reg_string,"01110",5)==0){
		return("$t6");
	}

	else if(strncmp(reg_string,"01111",5)==0){
		return("$t7");
	}

	else if(strncmp(reg_string,"10000",5)==0){
		return("$s0");
	}

	else if(strncmp(reg_string,"10001",5)==0){
		return("$s1");
	}

	else if(strncmp(reg_string,"10010",5)==0){
		return("$s2");
	}

	else if(strncmp(reg_string,"10011",5)==0){
		return("$s3");
	}

	else if(strncmp(reg_string,"10100",5)==0){
		return("$s4");
	}

	else if(strncmp(reg_string,"10101",5)==0){
		return("$s5");
	}

	else if(strncmp(reg_string,"10110",5)==0){
		return("$s6");
	}

	else if(strncmp(reg_string,"10111",5)==0){
		return("$s7");
	}

	else if(strncmp(reg_string,"11000",5)==0){
		return("$t8");
	}

	else if(strncmp(reg_string,"11001",5)==0){
		return("$t9");
	}

	else if(strncmp(reg_string,"11010",5)==0){
		return("$k0");
	}

	else if(strncmp(reg_string,"11011",5)==0){
		return("$k1");
	}

	else if(strncmp(reg_string,"11100",5)==0){
		return("$gp");
	}

	else if(strncmp(reg_string,"11101",5)==0){
		return("$sp");
	}

	else if(strncmp(reg_string,"11110",5)==0){
		return("$fp");
	}

	else if(strncmp(reg_string,"11111",5)==0){
		return("$ra");
	}

	else{
		return("error retrieving register\n");
	}
}



// print a single instruction given the address of the line
void print_instruction(uint32_t addr){
	uint32_t hex = mem_read_32(addr); //retrieves the line at the current address
	char hex_string[9];
	sprintf( hex_string, "%x", hex); //turns the line retrieved aboce (should be a hex value) and turns it into a string
	char temp_binary[34] = "";
	int i = 0;
	while (hex_string[i]) { //while loop turns hex string/array into binary string/array

         switch (hex_string[i]) {
         case '0':
             strcat(temp_binary,"0000");
             break;
         case '1':
             strcat(temp_binary,"0001");
             break;
         case '2':
             strcat(temp_binary,"0010");
             break;
         case '3':
             strcat(temp_binary,"0011");
             break;
         case '4':
             strcat(temp_binary,"0100");
             break;
         case '5':
             strcat(temp_binary,"0101");
             break;
         case '6':
             strcat(temp_binary,"0110");
             break;
         case '7':
             strcat(temp_binary,"0111");
             break;
         case '8':
             strcat(temp_binary,"1000");
             break;
         case '9':
             strcat(temp_binary,"1001");
             break;
         case 'A':
         case 'a':
             strcat(temp_binary,"1010");
             break;
         case 'B':
         case 'b':
             strcat(temp_binary,"1011");
             break;
         case 'C':
         case 'c':
             strcat(temp_binary,"1100");
             break;
         case 'D':
         case 'd':
             strcat(temp_binary,"1101");
             break;
         case 'E':
         case 'e':
             strcat(temp_binary,"1110");
             break;
         case 'F':
         case 'f':
             strcat(temp_binary,"1111");
             break;
         default:
             printf("\nInvalid hexadecimal digit %c",
                    hex_string[i]);
         }
         i++;
     }
	char binary_string[34] = "";
	for(i = strlen(temp_binary); i < 32; i++) //make temp string with padding zeros
	{
		strcat(binary_string,"0");
	}
	strcat(binary_string,temp_binary); //binary_string should now be a 32 character binary string


	//find format
	if(strncmp(binary_string,"000000",6)==0){ //R format

		char *funct = &binary_string[strlen(binary_string)-6];

		char *rs_loc = &binary_string[strlen(binary_string)-26];
		char *rt_loc = &binary_string[strlen(binary_string)-21];
		char *rd_loc = &binary_string[strlen(binary_string)-16];
		uint32_t shamt = (hex << 21) >> 21;

		char* rs = which_reg(rs_loc);
		char* rt = which_reg(rt_loc);
		char* rd = which_reg(rd_loc);

		if(strcmp(funct,"100000")==0){
			printf("add ");
			printf("%s, %s, %s\n",rd, rs,rt);
		}

		else if(strcmp(funct,"100001")==0){
			printf("addu ");
			printf("%s, %s, %s\n",rd, rs,rt);
		}

		else if(strcmp(funct,"100100")==0){
			printf("and ");
			printf("%s, %s, %s\n",rd, rs,rt);
		}

		else if(strcmp(funct,"001000")==0){
			printf("jr ");
			printf("%s\n",rs);
		}

		else if(strcmp(funct,"100111")==0){
			printf("nor ");
			printf("%s, %s, %s\n",rd, rs,rt);
		}

		else if(strcmp(funct,"100101")==0){
			printf("or ");
			printf("%s, %s, %s\n",rd, rs,rt);
		}

		else if(strcmp(funct,"101010")==0){
			printf("slt ");
			printf("%s, %s, %s\n",rd, rs,rt);
		}

		else if(strcmp(funct,"000000")==0){
			printf("sll ");
			printf("%s, %s, %d\n",rd, rt,shamt);

		}

		else if(strcmp(funct,"000010")==0){
			printf("srl ");
			printf("%s, %s, %d\n",rd, rt,shamt);
		}

		else if(strcmp(funct,"100010")==0){
			printf("sub ");
			printf("%s, %s, %s\n",rd, rs,rt);
		}

		else if(strcmp(funct,"100011")==0){
			printf("subu ");
			printf("%s, %s, %s\n",rd, rs,rt);
		}

		else if(strcmp(funct,"011000")==0){
			printf("mult ");
			printf("%s, %s\n",rs,rt);
		}

		else if(strcmp(funct,"011001")==0){
			printf("multu ");
			printf("%s, %s\n",rs,rt);
		}

		else if(strcmp(funct,"011010")==0){
			printf("div ");
			printf("%s, %s\n",rs,rt);
		}

		else if(strcmp(funct,"011011")==0){
			printf("divu ");
			printf("%s, %s\n",rs,rt);
		}

		else if(strcmp(funct,"100110")==0){
			printf("xor ");
			printf("%s, %s, %s\n",rd, rs,rt);
		}

		else if(strcmp(funct,"000011")==0){
			printf("sra ");
			printf("%s, %s, %d\n",rd, rt,shamt);
		}

		else if(strcmp(funct,"001001")==0){
			printf("jalr ");
			printf("%s, %d\n",rt, shamt);
		}

		else if(strcmp(funct,"010000")==0){
			printf("mfhi ");
			printf("%s\n",rd);
		}

		else if(strcmp(funct,"010010")==0){
			printf("mflo ");
			printf("%s\n",rd);
		}

		else if(strcmp(funct,"010001")==0){
			printf("mthi ");
			printf("%s\n",rs);
		}

		else if(strcmp(funct,"010011")==0){
			printf("mtlo ");
			printf("%s\n",rs);
		}

		else if(strcmp(funct,"001100")==0){
			printf("SYSCALL\n");
		}

		else{
			printf("error printing R format\n");
		}
	}

	else if( (strncmp(binary_string,"000010",6)==0) || (strncmp(binary_string,"000011",6)==0) ){ //J format

		uint32_t jaddr = (hex << 6) >> 6;

		if(strncmp(binary_string,"000010",6)==0){
			printf("j ");
		}

		else{
			printf("jal ");
		}

		printf("0x%x\n",jaddr);
	}

	else{

		char *rs_loc = &binary_string[strlen(binary_string)-26];
		char *rt_loc = &binary_string[strlen(binary_string)-21];
		char *immediate_b = &binary_string[strlen(binary_string)-16];
		uint32_t immediate = (hex << 16) >> 16;

		char* rs = which_reg(rs_loc);
		char* rt = which_reg(rt_loc);



		if(strncmp(binary_string,"001000",6)==0){
			printf("addi ");
			printf("%s, %s, %d\n",rt,rs,immediate);
		}

		else if(strncmp(binary_string,"001001",6)==0){
			printf("addiu ");
			printf("%s, %s, %d\n",rt,rs,immediate);
		}

		else if(strncmp(binary_string,"001100",6)==0){
			printf("andi ");
			printf("%s, %s, 0b%s\n",rt,rs,immediate_b);
		}

		else if(strncmp(binary_string,"001101",6)==0){
			printf("ori ");
			printf("%s, %s, 0b%s\n",rt,rs,immediate_b);
		}

		else if(strncmp(binary_string,"001110",6)==0){
			printf("xori ");
			printf("%s, %s, 0b%s\n",rt,rs,immediate_b);
		}

		else if(strncmp(binary_string,"001111",6)==0){
			printf("lui ");
			printf("%s, 0b%s\n",rt,immediate_b);
		}

		else if(strncmp(binary_string,"000100",6)==0){
			printf("beq ");
			printf("%s, %s, %d\n",rs,rt,immediate);
		}

		else if(strncmp(binary_string,"000101",6)==0){
			printf("bne ");
			printf("%s, %s, %d\n",rs,rt,immediate);
		}

		else if(strncmp(binary_string,"000110",6)==0){
			printf("blez ");
			printf("%s, %d\n",rs, immediate);
		}

		else if(strncmp(binary_string,"000111",6)==0){
			printf("bgtz ");
			printf("%s, %d\n",rs, immediate);
		}

		else if(strncmp(binary_string,"000001",6)==0){  //**********depends on rt**************
			if(strncmp(rt, "000001",6)==0){
				printf("bgez ");
			}
			else if(strncmp(rt,"000000",6)==0){
				printf("bltz ");
			}
			printf("%s, %d\n",rs, immediate);
		}

		else if(strncmp(binary_string,"100000",6)==0){
			printf("lb ");
			printf("%s, %d(%s)\n",rt, immediate, rs);
		}

		else if(strncmp(binary_string,"100001",6)==0){
			printf("lh ");
			printf("%s, %d(%s)\n",rt, immediate, rs);
		}

		else if(strncmp(binary_string,"100011",6)==0){
			printf("lw ");
			printf("%s, %d(%s)\n",rt, immediate, rs);
		}

		else if(strncmp(binary_string,"001010",6)==0){
			printf("slti ");
			printf("%s, %s, %d\n",rt,rs,immediate);
		}

		else if(strncmp(binary_string,"101000",6)==0){
			printf("sb ");
			printf("%s, %d(%s)\n",rt, immediate, rs);
		}

		else if(strncmp(binary_string,"101001",6)==0){
			printf("sh ");
			printf("%s, %d(%s)\n",rt, immediate, rs);
		}

		else if(strncmp(binary_string,"101011",6)==0){
			printf("sw ");
			printf("%s, %d(%s)\n",rt, immediate, rs);
		}

		else{
			printf("Error printing I format\n");
		}
	}
}

/*THIS FUNCTION ENSURES THAT IR IS A VALID R-TYPE OR I-TYPE INSTRUCTION
THIS WILL BE IMPORTANT LATER DUING HAZARD DETECTION
IF THIS FUNCTION OUTPUTS 0, THEN WE DONT HAVE A HAZARD WE HAVE AN ERROR IN THE CODE
*/
int Hazard_Type(uint32_t ir){
	uint32_t opcode = ir  >> 26;
	uint32_t funct = (ir << 26) >> 26;
	int output = 0;
	if(opcode == 0){ //rtype
		if((funct == 0x20) || (funct == 0x21) || (funct == 0x24) || (funct == 0x27)
	  || (funct == 0x26) || (funct == 0x25) || (funct == 0x2a) || (funct == 0x02)
	  || (funct == 0x03) || (funct == 0x00) || (funct == 0x22) || (funct == 0x23)
	  || (funct == 0x18) || (funct == 0x19) || (funct == 0x1a) || (funct == 0x1b)
	  || (funct == 0x10) || (funct == 0x11) || (funct == 0x12)){
			output = 1;
		}
	}
	else if((opcode == 0x08) || (opcode == 0x09) || (opcode == 0x0c) || (opcode == 0x0d)
			 || (opcode == 0x0f) || (opcode == 0x0e) || (opcode == 0x23) || (opcode == 0x21)
			 || (opcode == 0x20)){
				 output = 2;
			 }
	return (output);
}


/*THIS FUNCTION DETECTS WEATHER OR NOT WE SHOULD FORWARD A IN THE CPU PIPELINE STRUCT
IT ALSO RETURNS WHICH INTERMEDIATE REGISTER WE SHOULD FORWARD FROM DEPENDING ON IF THE OUTPUT
IS 1 OR 2*/
int ForwardA(){
	int output = 0;
	uint32_t ExMemRd;
	uint32_t MemWbRd;
	uint32_t IdExRs;

	if((ID_EX.IR >> 26) == 0){//r-type instruction in id_ex stage
		IdExRs = ID_EX.A;
	}
	else{//i-type instruciton in id_ex stage
		IdExRs = -1; //does not exist. value doesn't really matter as long as it doesnt accidently equal later
	}
	if((EX_MEM.IR >> 26) == 0){ //r-type instruciton in ex_mem stage
		ExMemRd = (EX_MEM.IR << 16) >> 27;
	}
	else{//i-type instruction in ex_mem stage
		ExMemRd = (EX_MEM.IR << 11) >> 27;
	}
	if((MEM_WB.IR >> 26) == 0){//r-type instruction in mem_wb stage
		MemWbRd = (MEM_WB.IR << 16) >> 27;
	}
	else{//i-type instruction in ex_mem stage
		MemWbRd = (MEM_WB.IR << 11) >> 27;
	}


	if((Hazard_Type(EX_MEM.IR) != 0) && (ExMemRd != 0) && (ExMemRd == IdExRs)){
		output = 2;
	}
	else if((Hazard_Type(MEM_WB.IR) != 0) && (MemWbRd != 0) && (MemWbRd == IdExRs)){
		output = 1;
	}
	return output;

}

/*THIS FUNCTION DETECTS WEATHER OR NOT WE SHOULD FORWARD B IN THE CPU PIPELINE STRUCT
IT ALSO RETURNS WHICH INTERMEDIATE REGISTER WE SHOULD FORWARD FROM DEPENDING ON IF THE OUTPUT
IS 1 OR 2*/
int ForwardB(){
	int output = 0;
	uint32_t ExMemRd;
	uint32_t MemWbRd;
	uint32_t IdExRt;

	if((ID_EX.IR >> 26) == 0){ //r-type instruction in id_ex stage
		IdExRt = ID_EX.B;
	}
	else{//i-type instruciton in id_ex stage
		IdExRt = (ID_EX.IR << 6) >> 27;
	}

	if((EX_MEM.IR >> 26) == 0){ //r-type instruciton in ex_mem stage
		ExMemRd = (EX_MEM.IR << 16) >> 27;
	}
	else{//i-type instruction in ex_mem stage
		ExMemRd = (EX_MEM.IR << 11) >> 27;
	}

	if((MEM_WB.IR >> 26) == 0){//r-type instruction in mem_wb stage
		MemWbRd = (MEM_WB.IR << 16) >> 27;
	}
	else{//i-type instruction in ex_mem stage
		MemWbRd = (MEM_WB.IR << 11) >> 27;
	}

	if((Hazard_Type(EX_MEM.IR) != 0) && (ExMemRd != 0) && (ExMemRd == IdExRt)){
		output = 2;
	}
	else if((Hazard_Type(MEM_WB.IR) != 0) && (MemWbRd != 0) && (MemWbRd == IdExRt)){
		output = 1;
	}
	return output;
}

/*COMPLETE THE FORWARDING PROCESS BY CHECKING FOR HAZARD, CHECKING IF FORWARDING
IS ENABLED, COMPLETE THE FORWARDING, AND RETURN TO IF() IF NOP IS neccessary*/
int ScanForHazard(){
	uint32_t ExMemRd;
	uint32_t MemWbRd;
	uint32_t IdExRs;
	uint32_t IdExRt;
	int output = 0;

	if((ID_EX.IR >> 26) == 0){ //r-type instruction in id_ex stage
		IdExRs = ID_EX.A;
		IdExRt = ID_EX.B;
	}
	else{
		IdExRs = -1;
		IdExRt = (ID_EX.IR << 6) >> 27;
	}

	if((EX_MEM.IR >> 26) == 0){ //r-type instruciton in ex_mem stage
		ExMemRd = (EX_MEM.IR << 16) >> 27;
	}
	else{//i-type instruction in ex_mem stage
		ExMemRd = (EX_MEM.IR << 11) >> 27;
	}

	if((MEM_WB.IR >> 26) == 0){//r-type instruction in mem_wb stage
		MemWbRd = (MEM_WB.IR << 16) >> 27;
	}
	else{//i-type instruction in ex_mem stage
		MemWbRd = (MEM_WB.IR << 11) >> 27;
	}

	if(((Hazard_Type(EX_MEM.IR) != 0) && (ExMemRd != 0) && (ExMemRd == IdExRs)) ||
 		 ((Hazard_Type(MEM_WB.IR)) && (MemWbRd != 0) && (MemWbRd == IdExRs)) ||
	 	 ((Hazard_Type(EX_MEM.IR) != 0) && (ExMemRd != 0) && (ExMemRd == IdExRt)) ||
	 	 ((Hazard_Type(MEM_WB.IR)) && (MemWbRd != 0) && (MemWbRd == IdExRt))){

			 if(ENABLE_FORWARDING != 0){
				 if(ForwardA() == 1){
					 NEXT_STATE.REGS[ID_EX.A] = MEM_WB.ALUOutput;
					 output = 2;
				 }

				 if(ForwardA() == 2){
					 NEXT_STATE.REGS[ID_EX.A] = EX_MEM.ALUOutput;
					 output = 2;
				 }

				 if(ForwardB() == 1){
					 NEXT_STATE.REGS[ID_EX.B] = MEM_WB.ALUOutput;
					 output = 2;
				 }

				 if(ForwardB() == 2){
					 NEXT_STATE.REGS[ID_EX.B] = EX_MEM.ALUOutput;
					 output = 3;
				 }
				}
				else{
					output = 1;
				}


			 }
			 else{
				 output = 0;
			 }
			 return(output);
}
