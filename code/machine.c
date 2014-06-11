#include "machine.h"
#include <stdlib.h>
#include <stdio.h>

void load_program(Machine *pmach,
                  unsigned textsize, Instruction text[textsize],
                  unsigned datasize, Word data[datasize],  unsigned dataend)
{
	pmach->_text = text;
	pmach->_textsize = textsize;
	
	pmach->_data = data;
	pmach->_datasize = datasize;
	
	pmach->_dataend = dataend;
	
	pmach->_cc = CC_U;
	pmach->_sp = datasize-1;
	pmach->_pc = 0;
	
	printf("textsize %d, datasize %d, dataend %d, cc %d, sp %d\n", 
				pmach->_textsize, pmach->_datasize, pmach->_dataend,
				pmach->_cc, pmach->_sp);
	print_instruction(pmach->_text[1], 0);
	
	int i = 0;
	for(i = 0; i < NREGISTERS - 1; i++)
	{
		pmach->_registers[i] = 0;
	}
}

void read_program(Machine *mach, const char *programfile)
{
	FILE* pfd = NULL;

	unsigned int textsize;
	unsigned int datasize;
	unsigned int dataend;
	
	if((pfd = fopen(programfile, "r")) == NULL)
    {
    	perror("read_program Can not open file");
    	return;
    }
    
    //	Lire les 3 premiers entiers non signés
    fread(&textsize, 	sizeof(unsigned int),	1,	pfd);
    fread(&datasize, 	sizeof(unsigned int),	1,	pfd);
   	fread(&dataend, 	sizeof(unsigned int),	1,	pfd);
 
 	//	Allouer la mémoire pour les instructions et données
	Instruction *text = malloc(sizeof(Instruction) * textsize);
	Word *data = malloc(sizeof(Word) * datasize);

	//	Lire les textsize instructions et datasize données
	fread(text, sizeof(Instruction), textsize, pfd);	
	fread(data, sizeof(Word), datasize, pfd);
    
	//	Charger le programme
    load_program(mach,textsize, text, datasize, data, dataend);
    
    if(fclose(pfd) == -1)
    {
    	perror("read_program Can not close opened file");
    }    
}

void dump_memory(Machine *pmach)
{

}

void print_program(Machine *pmach)
{
	int i;
	
	printf("*** PROGRAM (size: %d) ***\n", pmach->_textsize);
	for(i = 0 ; i < pmach->_textsize ; i++)
	{
		printf("0x%04x: 0x%08x\t", i, pmach->_text[i]._raw);
		print_instruction(pmach->_text[i], i);
		printf("\n");
	}
}

void print_data(Machine *pmach)
{
	int i;
	
	for(i = 0 ; i < pmach->_datasize ; i++)
	{
		printf("%d\n",pmach->_data[i]);
	}
}

void print_cpu(Machine *pmach)
{
	int i;
	
	printf("*** CPU	***\n");
	for(i = 0 ; i < NREGISTERS ; i ++)
	{
		printf("R%02d: 0x%08x %d\t", i, pmach->_registers[i],pmach->_registers[i]);
		if(i%3==2) printf("\n");
		
	}
	
	//printf("R%02d: 0x%08x %d\t\n", 15, pmach->_sp,pmach->_sp);
}

void simul(Machine *pmach, bool debug)
{
	Instruction *instruction_courante;
	int i = 0;
	
	while( (instruction_courante = pmach->_text[i])->_cop != HALT)
	{
		if(decode_execute(pmach, instruction_courante) == FALSE)
		{
			//TODO ERROR
		}
	
		if(debug)
		{
			//FIXME CONSTANTES
			trace("", pmach, instruction_courante, 0);
		}	
		i++;
	}
}
