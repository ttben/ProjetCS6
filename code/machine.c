#include "machine.h"
#include <stdlib.h>
#include <stdio.h>

#include "exec.h"
#include "debug.h"
bool dumping_in_progress =0;

void load_program(Machine *pmach,
                  unsigned textsize, Instruction text[textsize],
                  unsigned datasize, Word data[datasize],  unsigned dataend)
{

	pmach->_text = malloc(sizeof(Instruction) * textsize);	
	pmach->_data = malloc(sizeof(Word) * datasize);	
	
	//	Sauvegarder les paramètres
	for(int i = 0; i < textsize ; i++)
		pmach->_text[i] = text[i];
	pmach->_textsize = textsize;
	
	for(int i = 0; i < datasize ; i++)
		pmach->_data[i] = data[i];
	pmach->_datasize = datasize;
	
	pmach->_dataend = dataend;
	
	//	Initialiser les pointeurs
	pmach->_cc = CC_U;
	pmach->_sp = datasize-1;
	pmach->_pc = 0;
	
	//	Réinitialiser les registres
	for(int i = 0; i < NREGISTERS - 1; i++)
		pmach->_registers[i] = 0;
}

void read_program(Machine *mach, const char *programfile)
{
	FILE* pfd = NULL;
	
	//	Ouvrir le fichier binaire passé en paramètre
	if((pfd = fopen(programfile, "r")) == NULL)
    {
    	perror("read_program : Can not open file");
    	exit(-1);
    }

	unsigned int textsize;
	unsigned int datasize;
	unsigned int dataend;
	    
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
    
    free(text);
    free(data);
    
    if(fclose(pfd) == -1)
       	perror("read_program Can not close opened file");
}

void dump_memory(Machine *pmach)
{
	//	Réinitialiser les variables de dump
	dumping_in_progress = 1;
	
	//	Afficher le contenu des segments
	print_program(pmach);
	print_data(pmach);

   	FILE *file;
    if(!(file = fopen("dump.bin", "w+")))
    {
        fprintf(stderr, "Ecriture du fichier impossible.\n");
        exit(1);
    }

    if(fwrite(&(pmach->_textsize), sizeof(unsigned), 1, file) != 1)
    {
        fprintf(stderr, "Erreur durant l'écriture de _textsize.\n");
        exit(1);
    }

    if(fwrite(&(pmach->_datasize), sizeof(unsigned), 1, file) != 1)
    {
        fprintf(stderr, "Erreur durant l'écriture de _datasize.\n");
        exit(1);
    }

    if(fwrite(&(pmach->_dataend), sizeof(unsigned int), 1, file) != 1)
    {
        fprintf(stderr, "Erreur durant l'écriture de _dataend.\n");
        exit(1);
    }

    if(fwrite(&(pmach->_text->_raw), sizeof(Word), pmach->_textsize, file) !=
            pmach->_textsize)
    {
        fprintf(stderr, "Erreur durant l'écriture des instructions.\n");
        exit(1);
    }

    //Ecriture des données
    if(fwrite(pmach->_data, sizeof(Word), pmach->_datasize, file)
            != pmach->_datasize)
    {
        fprintf(stderr, "Erreur durant l'écriture des données.\n");
        exit(1);
    }
    //On ferme le fichier
    fclose(file); 
	
	//	Réinitialiser la variable de dump
	dumping_in_progress = 0;
}

void print_program(Machine *pmach)
{

	int i;
	
	//	Détecter si un dump est en cours
	if(!dumping_in_progress) printf("*** PROGRAM (size: %d) ***\n", pmach->_textsize);
	else printf("Instruction text[] = {");
	
	//	Parcourir le segment de text
	for(i = 0 ; i < pmach->_textsize ; i++)
	{
		if(dumping_in_progress == 0)
		{
			printf("0x%04x: 0x%08x\t", i, pmach->_text[i]._raw);
			print_instruction(pmach->_text[i], i);
			printf("\n");
		}
		else
		{
			if(i % 4 == 0) printf("\n\t");
			printf("0x%08x, ", pmach->_text[i]._raw);
		}
	}
	
	if(dumping_in_progress == 1)
	{
		printf("\n};\n");
		printf("unsigned textsize = %d;\n\n", pmach->_textsize);
	}
	else printf("\n");
}

void print_data(Machine *pmach)
{

	int i;
	
	//	Detecter si un dump est en cours
	if(dumping_in_progress == 1) printf("Word data[] = {\n\t");
	else printf("*** DATA (size: %d, end = %08x (%d)) ***\n", pmach->_datasize, pmach->_dataend, pmach->_dataend);
	
	//	Parcourir le segment de données
	for(i = 0 ; i < pmach->_datasize ; i++)
	{
		if(!dumping_in_progress) 
		{
			printf("0x%04x:  0x%08x %d\t\t",i, pmach->_data[i],pmach->_data[i]);
			if( ((i+1) % 3) == 0) printf("\n");
		}
		else 
		{
			printf("0x%08x, ", pmach->_data[i]);
			if( (i+1) % 4 == 0) printf("\n\t");
			//	Sauvegarder les valeurs brutes (bin) des datas
		}
	}
	
	if(dumping_in_progress == 1) 
	{
		printf("\n};\n");
		printf("unsigned datasize = %d;\nunsigned dataend = %d;\n\n", pmach->_datasize, pmach->_dataend);
	}
}

void print_cpu(Machine *pmach)
{
	int i;
	static char* state_last_op ="UZPN";
	
	printf("\n*** CPU	***\n");
	printf("PC: %08x\tCC: %c\n\n", pmach->_pc, state_last_op[pmach->_cc]);
	
	//	Afficher l'état des processeurs
	for(i = 0 ; i < NREGISTERS ; i ++)
	{
		printf("R%02d: 0x%08x %d\t", i, pmach->_registers[i],pmach->_registers[i]);
		if(i%3==2) printf("\n");
		
	}
	
	printf("\n");
}

void simul(Machine *pmach, bool debug)
{
	Instruction instruction_courante;
	
	int run = 1;
	
	while(run)
	{
		//	Récupérer l'instruction courante
		instruction_courante = pmach->_text[pmach->_pc++];
	
		//	Décoder et exécuter l'instruction courante
		//	elle retourne != 1 si HALT trouvé
		run = decode_execute(pmach, instruction_courante);
		
		trace("Executing:", pmach, instruction_courante, pmach->_pc);
		
		//	Demander le debug si debug en cours
		if(debug)
			debug_ask(pmach);
	}
}
