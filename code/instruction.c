/*!
 * \file instruction.c
 * \brief Désassemblage d'une instruction.
 */

#include <stdio.h>
#include <malloc.h>
#include "instruction.h"
#include <strings.h>

const char* cop_names[] = {
    "ILLOP",	//!< Instruction illégale
    "NOP",	    //!< Instruction sans effet
    "LOAD",	    //!< Chargement d'un registre
    "STORE",	//!< Rangement du contenu d'un registre
    "ADD",	    //!< Addition à un registre
    "SUB",	    //!< Soustraction d'un registre
    "BRANCH",	//!< Branchement conditionnel ou non
    "CALL",	    //!< Appel de sous-programme
    "RET",	    //!< Retour de sous-programme
    "PUSH", 	//!< Empilement sur la pile d'exécution
    "POP",	    //!< Dépilement de la pile d'exécution
    "HALT",	    //!< Arrêt (normal) du programme
};

const char* condition_names[] = {
    "NC",   //!< Pas de condition (nrachement inconditionnel)
    "EQ",	//!< Égal à 0
    "NE",	//!< Différent de 0
    "GT",   //!< Strictement positif
    "GE",  	//!< Positif ou nul
    "LT",   //!< Strictement négatif
    "LE",   //!< Négatif ou null
};

//! Renvoie la valeur de I (immédiat)
/*!
 * \param instr L'instruction à évaluer
 */
bool isImmediate(Instruction instr) {
    return instr.instr_generic._immediate;
}

//! Renvoie vrai si l'instruction est de type absolue
/*!
 * Equivaut à not(X) ou I
 * \param instr L'instruction à évaluer
 */
bool isAbsolute(Instruction instr) {
    if(isImmediate(instr)) {
        return false;
    }
    return !instr.instr_generic._indexed;
}

//! Renvoie le code opération de l'instruction
/*!
 * \param instr L'instruction à évaluer
 */
const Code_Op getInstrCop(Instruction instr) {
    const Code_Op COP = instr.instr_generic._cop;

    if(COP > LAST_COP)
        return 0;
    return COP;
}

//! Renvoie vrai si RC contient une valeur de registre
/*!
 * est fonction du code opération (> SUB & < LOAD)
 * \param instr L'instruction à vérifier
 */
bool isRCregister(Instruction instr) {
    const Code_Op COP = getInstrCop(instr);

    if(COP > SUB || COP < LOAD) {
        return false;
    }
    return true;
}

//! Renvoie vrai si le code opération ne nécessite pas d'opérandes
/*!
 * \param instr L'instruction à vérifier
 */
bool noOpNeeded(Instruction instr) {
    const Code_Op COP = getInstrCop(instr);

    if(COP <= NOP || COP >= HALT || COP == RET) {
        return true;
    }
    return false;
}

//! Renvoie vrai si le code opération passé en paramètre
/*!
 * fait partie des codes qui nécessitent une condition
 * soit: BRANCH et CALL
 * \param cop Code opération
 */
bool needCondCOP(Code_Op cop) {
    if(cop == BRANCH || cop == CALL) {
        return true;
    }
    return false;
}

//! Renvoie l'opérande de l'instruction
/*!
 * Fonction de I et X dans l'instruction
 * \param instr L'instruction à évaluer
 */
void getInstrOp(Instruction instr, unsigned addr, char* op) {
    if(noOpNeeded(instr)) {
    	
        return;
    }

    unsigned regcond = instr.instr_generic._regcond;
    bool immediate = isImmediate(instr), absolute = isAbsolute(instr);
    int val;

    if(immediate || absolute) {
        // registre ou condition + val
        if(immediate) val = instr.instr_immediate._value;
        else val = instr.instr_absolute._address;

        if(isRCregister(instr)) {
            if (getInstrCop(instr) != STORE) sprintf(op,"R%02u, #%i",regcond, val);
            // cas exceptionnel, pour le store
            else sprintf(op, "R%02u, @0x%04x", regcond, val);
        } else if (needCondCOP(getInstrCop(instr))) {
            sprintf(op,"%s, @0x%04x", condition_names[regcond], instr.instr_immediate._value);
        } else {
            sprintf(op,"@0x%04x", instr.instr_immediate._value);
        }
    } else {
            // registre ou condition + offset[registre RX]
            signed offset = instr.instr_indexed._offset;
            unsigned regindex = instr.instr_indexed._rindex;
            sprintf(op,"R%02u, %i[R%02u]", regcond, offset, regindex);
    }
}

void print_instruction(Instruction instr, unsigned addr) {
    unsigned codeOperation = getInstrCop(instr);
    char op[20];
    bzero(op, 20);
    getInstrOp(instr, addr,op);
    printf("%s %s", cop_names[codeOperation], op);
}

