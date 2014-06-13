#include "debug.h"
#include "exec.h"
#include <stdio.h>

bool debug_ask(Machine *pmach) {
    int c;
    bool result = false;
    while(true) {
        printf("**** Phase de debuggage ****");
        c = getchar(); // Récupère le caractère suivant.

        if(c == '\n') {
            return true; // Si on est sur une fin de ligne on a fini.
		}

        switch(c) {
            case 'h': // Affiche les commandes disponibles
                printf("\th\tAffiche les différentes commandes disponibles.\n");
                printf("\tc\tQuitte le mode de debuggage.\n");
				printf("\ts\tExecute les instructions pas à pas.\n");
				printf("\tr\tAffiche les registres.\n");
				printf("\td\tAffiche la mémoire DATA.\n");
				printf("\tt\tAffiche le texte de la mémoire.\n");
				printf("\tp\tAffiche le texte de la mémoire.\n");
				printf("\tm\tAffiche les registres et la mémoire DATA.\n");
                break;
            case 'c':
                result = false; // Commandes pour quitter le programme
				break;
            case 's':
                result = true; // On passe à l'instruction suivante 
                break;
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
            default:
                break;
        }
    }
    printf("\n");
    return result;
}
