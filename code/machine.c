#include "machine.h"
#include <stdlib.h>
#include <stdio.h>
#include "exec.h"
#include "debug.h"
bool dumping_in_progress =0;
static uint32_t *raws;
static uint32_t index_raws = 0;

void load_program(Machine *pmach,
                  unsigned textsize, Instruction text[textsize],
                  unsigned datasize, Word data[datasize],  unsigned dataend)
{
	//	Sauvegarder les paramètres
	pmach->_text = text;
	pmach->_textsize = textsize;
	
	pmach->_data = data;
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
    	return;
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
    
    if(fclose(pfd) == -1)
       	perror("read_program Can not close opened file");
}

void dump_memory(Machine *pmach)
{
	//	Réinitialiser les variables de dump
	index_raws = 0;
	dumping_in_progress = 1;
	
	int taille_donnees_raws = pmach->_textsize + pmach->_datasize + 3;
	
	/*	
		(Ré)Allouer la place du binaire a générer
		(nb de text, nb de data + 3 int indiquant
	 	la taille de text & data et donnant dataend 
	*/
	raws = malloc(sizeof(uint32_t) * taille_donnees_raws);
	
	//	Renseigner les champs de tête/d'informations
	raws[0] = pmach->_textsize;
	raws[1] = pmach->_datasize;
	raws[2] = pmach->_dataend;
	index_raws += 3;
	
	//	Afficher le contenu des segments
	print_program(pmach);
	print_data(pmach);
	
	FILE *fp = NULL; 
	if( (fp =fopen("dump.bin", "w+")) == NULL)
	{
		perror("Dump : Impossible de créer/ouvrir le fichier dump.prog");
		exit(-1);
	}
		
	//	Ecrire raws dans le dump.prog
	fwrite(raws, sizeof(uint32_t), taille_donnees_raws, fp);
	
	//	Fermer le fdescriptor
	if( fclose(fp) == -1)
	{
		perror("Dump : Impossible de fermer dump.prog");
		exit(-1);
	}
	
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
			//	Sauvegarder les valeurs brutes (bin) des datas
			raws[index_raws + i] = (pmach->_text[i])._raw;
			index_raws ++;
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
			raws[index_raws + i] = (pmach->_data[i]);
			index_raws ++;
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
