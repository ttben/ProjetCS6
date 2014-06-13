#include "error.h"
#include <stdio.h>

void error(Error err, unsigned addr) {
	switch(err) {
		case ERR_NOERROR : 
				printf("**** Aucune erreur ****");
				break;
		case ERR_UNKNOWN : 
				printf("**** ERREUR 1 **** : Instruction inconnue");
				printf("\tà l'adresse 0x%04x\n", addr);
				exit(1);
		case ERR_ILLEGAL : 
				printf("**** ERREUR 2 **** : Instruction illégale");
				printf("\tà l'adresse 0x%04x\n", addr);
				exit(1);
		case ERR_CONDITION : 
				printf("**** ERREUR 3 **** : Condition illégale");
				printf("\tà l'adresse 0x%04x\n", addr);
				exit(1);
		case ERR_IMMEDIATE : 
				printf("**** ERREUR 4 **** : Valeur immédiate interdite");
				printf("\tà l'adresse 0x%04x\n", addr);
				exit(1);
		case ERR_SEGTEXT : 
				printf("**** ERREUR 5 **** : Violation de taille du segment de texte");
				printf("\tà l'adresse 0x%04x\n", addr);
				exit(1);
		case ERR_SEGDATA : 
				printf("**** ERREUR 6 **** : Violation de taille du segment de données");
				printf("\tà l'adresse 0x%04x\n", addr);
				exit(1);
		case ERR_SEGSTACK : 
				printf("**** ERREUR 7 **** : Violation de taille du segment de pile");
				printf("\tà l'adresse 0x%04x\n", addr);
				exit(1);
		default: 
				printf("**** ERREUR **** : Erreur inconnue \n");
				printf("\tà l'adresse 0x%04x\n", addr);
				exit(1);
	}
}

void warning(Warning warn, unsigned addr){
		if(WARN_HALT == warn) {
				printf("**** WARNING **** : HALT atteind à l'adresse 0x%04x\n", addr);
		}
}
