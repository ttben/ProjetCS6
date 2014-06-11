#include <stdio.h>
#include "instruction.h"

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

bool isImmediate(Instruction instr) {
    return instr.instr_generic._immediate;
}

bool isAbsolute(Instruction instr) {
    if(isImmediate(instr)) {
        return false;
    }
    return !instr.instr_generic._indexed;
}

bool isRCregister(Instruction instr) {
    const Code_Op COP = instr.instr_generic._cop;

    if(COP > SUB || COP < LOAD) {
        return false;
    }
    return true;
}

const Code_Op getInstrCop(Instruction instr) {
    const Code_Op COP = instr.instr_generic._cop;

    if(COP > LAST_COP)
        return 0;
    return COP;
}

bool noOpNeeded(Instruction instr) {
    const Code_Op COP = instr.instr_generic._cop;

    if(COP <= NOP || COP >= HALT || COP == RET) {
        return true;
    }
    return false;
}

bool needCondCOD(Code_Op cod) {
    if(cod == BRANCH || cod == CALL) {
        return true;
    }
    return false;
}

const char* getInstrOp(Instruction instr, unsigned addr) {
    if(noOpNeeded(instr)) {
        return "";
    }

    char op[20];
    unsigned regcond = instr.instr_generic._regcond;
    bool immediate = isImmediate(instr), absolute = isAbsolute(instr);
    unsigned val;

    if(immediate || absolute) {
        // registre ou condition + val
        if(immediate) val = instr.instr_immediate._value;
        else val = instr.instr_absolute._address;

        if(isRCregister(instr)) {
            sprintf(op,"R%02u, #%u",regcond, val);
        } else if (needCondCOD(getInstrCop(instr))) {
            sprintf(op,"%s, @0x%04x", condition_names[regcond], instr.instr_immediate._value);
        } else {
            sprintf(op,"@0x%04x", instr.instr_immediate._value);
        }
    } else {
            // registre ou condition + offset[registre RX]
            unsigned offset = instr.instr_indexed._offset;
            unsigned regindex = instr.instr_indexed._rindex;
            sprintf(op,"R%02u, %u[R%02u]", regcond, offset, regindex);
    }

    return op;
}

void print_instruction(Instruction instr, unsigned addr) {
    printf("%s %s", cop_names[getInstrCop(instr)], getInstrOp(instr, addr));
}

