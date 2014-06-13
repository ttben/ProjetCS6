#include "debug.h"
#include "exec.h"
#include <stdio.h>

bool afficher = true;
//! Affiche le menu de débogage
/*! 
 * Permet d'afficher les différentes commandes disponibles dans
 * la phase de débogage.
 */
void affiche_commande() {
	printf("\th\tAffiche les différentes commandes disponibles.\n");
    printf("\tc\tQuitte le mode de debuggage.\n");
	printf("\ts\tExecute les instructions pas à pas.\n");
	printf("\tr\tAffiche les registres.\n");
	printf("\td\tAffiche la mémoire DATA.\n");
	printf("\tp et t\tAffiche le texte de la mémoire.\n");
	printf("\tm\tAffiche les registres et la mémoire DATA.\n\n");
}

bool debug_ask(Machine *pmach) {

	if(afficher){
		printf("\n\n**** Phase de débogage ****\n\n");
		affiche_commande();
		afficher = false;
	}

	switch(getchar()) {
	    case 'h': // Affiche les commandes disponibles
	        affiche_commande();
	        break;
	    case 'c':
	        exit(0);
	    case 's':
	        return false; // On passe à l'instruction suivante 
	    case 'r':
	        print_cpu(pmach);
	        break; // On demande l'affichage des registres
	    case 'd':
	        print_data(pmach);
	        break; // On demande l'affichage de la DATA
	    case 'p':
	    case 't':
	        print_program(pmach);
	        break; // Affiche le texte présent en mémoire
	    case 'm':
	        print_cpu(pmach);
	        print_data(pmach);
	        break; // Affiche les registres et la DATA
	    case '\n':
	    	return true;
	    default:
	    	return false;
	
	}
    printf("\n");
    return true;
}
