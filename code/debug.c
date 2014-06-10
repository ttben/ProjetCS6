#include "debug.h"
#include "exec.h"
#include <stdio.h>

bool debug_ask(Machine *pmach) {
    int c, t;
    while(true) {
        printf("**** Phase de debuggage ****");
        c = getchar();

        if(c == '\n') {
            return true;
		}

        do {
            t = getchar();
        } while(t != '\n' && t != EOF);

        switch(c) {
            case 'h':
                printf("\th\tAffiche les différentes commandes disponibles.\n");
				printf("\tc\tQuitte le mode de debuggage.\n");
				printf("\ts\tExecute les instructions pas à pas.\n");
				printf("\tr\tAffiche les registres.\n");
				printf("\td\tAffiche la mémoire DATA.\n");
				printf("\tt\tprint text (program) memory\n");
				printf("\tp\tprint text (program) memory\n");
				printf("\tm\tAffiche les registres et la mémoire DATA.\n");
				printf("\tRET\tExecute les instructions pas à pas.\n");
                break;
            case 'c':
                return false;
            case 's':
                return true;
            case 'r':
                print_cpu(pmach);
                break;
            case 'd':
                print_data(pmach);
                break;
            case 'p':
            case 't':
                print_program(pmach);
                break;
            case 'm':
                print_cpu(pmach);
                print_data(pmach);
                break;
            default:
                break;
        }
    }
    printf("\n");
    return false;
}
