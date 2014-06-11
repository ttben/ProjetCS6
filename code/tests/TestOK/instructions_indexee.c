#include "../../machine.h"

//! Premier exemple de segment de texte.
/*!
 * \note On voit ici l'intérêt de l'initialisation d'une union avec nommage
 * explicite des champs.
 */

Instruction text[] = {
//   type		 cop	imm	ind	regcond	operand
//-------------------------------------------------------------
    {.instr_indexed =  {LOAD, 	false,  true, 	0, 	15, -3	}},
    {.instr_indexed =  {ADD,    false,  true, 	0, 	15, 2	}},
    {.instr_indexed =  {STORE,  false,  true, 	0, 	15, -4	}},
    {.instr_generic =  {HALT,					}},
};

//! Taille utile du programme
const unsigned textsize = sizeof(text) / sizeof(Instruction);

//! Premier exemple de segment de données initial
Word data[20] = {
    10, // 0: premier opérande
    5,  // 1: second opérande
    20, // 2: résultat
    0,  // 3
};

//! Fin de la zone de données utiles
const unsigned dataend = 5;

//! Taille utile du segment de données
const unsigned datasize = sizeof(data) / sizeof(Word);  
