/*!
 * \file exec.c
 * \brief Exécution d'une instruction.
 */

#include "exec.h"
#include "error.h"
#include <stdio.h>
 
//! Modifie le code condition (pour LOAD, ADD et SUB) en fonction du signe du résultat
/*!
 * \param pmach machine en cours d'exécution
 * \param reg numéro du registre à analyser
 */
void changeCC(Machine *pmach, unsigned int regcond) {
    signed value = pmach->_registers[regcond];
    if (value < 0)
        pmach->_cc = CC_N;
    else if (value > 0)
        pmach->_cc = CC_P;
    else
        pmach->_cc = CC_Z;
}

//! Vérifie l'existance du numéro de registre.
/*!
 * \param instr instruction en cours
 * \param addr adresse de l'instruction
 */
void checkReg(Instruction instr, unsigned addr) {
    unsigned int reg = instr.instr_generic._regcond;
	if (reg < 0 || reg > NREGISTERS - 1)
		error(ERR_ILLEGAL, addr);
}

//! Vérifie l'espace restant dans SP (Stack Pointer)
/*!
 * \param pmach machine en cours d'exécution
 * \param addr adresse de l'instruction
 */
void checkSP(Machine *pmach, unsigned addr) {
	if (pmach->_sp < pmach->_dataend || pmach->_sp >= pmach->_datasize)
		error(ERR_SEGSTACK, addr);
}


//! Vérifie que l'instruction n'est pas immédiate.
/*!
 * \param instr instruction en cours
 * \param addr adresse de l'instruction
 */
void checkNotI(Instruction instr, unsigned addr) {
    unsigned int I = instr.instr_generic._immediate;
	if(I == 1)
		error(ERR_IMMEDIATE,addr);
}

//! Vérifie la validitée de la condition par rapport à CC (au code condition)
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 */
bool checkCC(Machine *pmach, Instruction instr, unsigned addr) {
    unsigned int cond = instr.instr_generic._regcond;
	switch (cond) {
        case NC: // Pas de condition
            return true;
        case EQ: // Egal à 0
            return (pmach->_cc == CC_Z);
        case NE: // Different de zero
            return (pmach->_cc != CC_Z);
        case GT: // Strictement positif
            return (pmach->_cc == CC_P);
        case GE: // Positif ou nul
            return (pmach->_cc == CC_P || pmach->_cc == CC_Z);
        case LT: // Strictement négatif
            return (pmach->_cc == CC_N);
        case LE: // Négatif ou nul
            return (pmach->_cc == CC_N || pmach->_cc == CC_Z);
        default:
            error(ERR_CONDITION, addr);
	}
}

//! Calcule l'adresse réelle en utilisant l'adressage indexé ou absolu.
/*!
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 */
unsigned int getAddr(Machine *pmach, Instruction instr) {
    signed X = instr.instr_generic._indexed;
	if (X) {
	    unsigned reg = instr.instr_indexed._rindex;
	    signed offset = instr.instr_indexed._offset;
		return pmach->_registers[reg] + offset;
	}
	return instr.instr_absolute._address;
}

//! Vérifie que l'adresse n'a pas dépassé la zone allouée (erreur de segmentation)
/*!
 * Notez que cette méthode suppose que la première adresse est 0
 * (ce qui est le cas dans notre simulation)
 * \param pmach machine en cours d'exécution
 * \param dataAddr adresse à tester
 * \param addr adresse de l'instruction en cours
 */
void checkDataAddr(Machine *pmach, unsigned int dataAddr, unsigned addr) {
	printf("DATA ADDR  %d > %d\n", dataAddr, pmach->_datasize);
	if (dataAddr > pmach->_datasize)
		error(ERR_SEGDATA, addr);
}

//! Décode et exécute l'instruction LOAD
/*!
 * LOAD accepte l'adressage immédiat, absolu et indexé pour la source.
 * Il faut indiquer un registre de destination.
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool load(Machine *pmach, Instruction instr, unsigned addr) {
	checkReg(instr, addr);

	unsigned int I = instr.instr_generic._immediate;
	unsigned int regcond = instr.instr_generic._regcond;

	if (I) { // Si I = 1 : immediat
		pmach->_registers[regcond] = instr.instr_immediate._value;
	} else { // sinon, absolu ou relatif
		unsigned int address = getAddr(pmach, instr);
		checkDataAddr(pmach, address, addr);
		pmach->_registers[regcond] = pmach->_data[address];
	}

	changeCC(pmach, regcond);
	return true;
}

//! Décode et exécute l'instruction STORE
/*!
 * STORE accepte l'adressage absolu et indexé pour la destination.
 * Il faut indiquer un registre pour la source.
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool store(Machine *pmach, Instruction instr, unsigned addr) {
	checkNotI(instr, addr);
	checkReg(instr, addr);

	unsigned regcond = instr.instr_generic._regcond;
	unsigned address = getAddr(pmach, instr);

	checkDataAddr(pmach, address, addr);

	pmach->_data[address] = pmach->_registers[regcond];
	return true;
}

//! Décode et exécute l'instruction ADD
/*!
 * ADD accepte l'adressage immédiat, absolu et indexé pour la source.
 * Il faut indiquer un registre pour la destination.
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool add(Machine *pmach, Instruction instr, unsigned addr) {
	checkReg(instr, addr);

	unsigned int I = instr.instr_generic._immediate;
	unsigned int regcond = instr.instr_generic._regcond;

	if (I) { // I=1, immediat
		pmach->_registers[regcond] += instr.instr_immediate._value;
	} else {				
		unsigned int address = getAddr(pmach, instr);
		checkDataAddr(pmach, address, addr);
		pmach->_registers[regcond] += pmach->_data[address];
	}

	changeCC(pmach, regcond);
	return true;
}

//! Décode et exécute l'instruction SUB.
/*!
 * SUB accepte l'adressage immédiat, absolu et indexé pour la source.
 * Il faut indiquer un registre pour la destination.
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool sub(Machine *pmach, Instruction instr,unsigned addr) {
	checkReg(instr, addr);

	unsigned int I = instr.instr_generic._immediate;
	unsigned int regcond = instr.instr_generic._regcond;

	if (I) { // I=1, immediat
		pmach->_registers[regcond] -= instr.instr_immediate._value;
	} else {				
		unsigned int address = getAddr(pmach, instr);
		checkDataAddr(pmach, address, addr);
		pmach->_registers[regcond] -= pmach->_data[address];
	}

	changeCC(pmach, regcond);
	return true;
}

//! Décode et exécute l'instruction BRANCH.
/*!
 * BRANCH n'accepte pas l'adressage immédiat
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool branch(Machine *pmach, Instruction instr, unsigned addr) {
	checkNotI(instr, addr);

	if (checkCC(pmach, instr, addr)) {
		unsigned int address = getAddr(pmach, instr);
		pmach->_pc = address;
	}

	return true;
}

//! Décode et exécute l'instruction CALL.
/*!
 * CALL n'accepte pas l'adressage immédiat
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool call(Machine *pmach, Instruction instr, unsigned addr) {
	checkNotI(instr, addr);
	checkSP(pmach, addr);
	
	if (checkCC(pmach, instr, addr)) {
		pmach->_data[pmach->_sp--] = pmach->_pc;
		unsigned int address = getAddr(pmach, instr);
		pmach->_pc = address;
	}

	return true;
}

//! Décode et exécute l'instruction RET
/*!
 * RET ne s'intéresse pas au contenu de l'opérande
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool ret(Machine *pmach, Instruction instr, unsigned addr) {
	++pmach->_sp;
	checkSP(pmach, addr);
	pmach->_pc = pmach->_data[pmach->_sp];

	return true;
}

//! Décode et exécute l'instruction PUSH
/*!
 * PUSH supporte l'immédiat, l'adressage indexé et absolu.
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool push(Machine *pmach, Instruction instr, unsigned addr) 
{
	checkSP(pmach, addr);

	unsigned int I = instr.instr_generic._immediate;

	if (I) { // I=1, immediat
		pmach->_data[pmach->_sp--] = instr.instr_immediate._value;
	} else {
		unsigned int address = getAddr(pmach, instr);
		checkDataAddr(pmach, address, addr);
		pmach->_data[pmach->_sp--] = pmach->_data[address];	
	}
	
	return true;
}

//! Décode et exécute l'instruction POP
/*!
 * POP ne supporte pas les valeurs immédiates
 * \param pmach machine en cours d'exécution
 * \param instr instruction en cours
 * \param addr adresse de l'instruction en cours
 */
bool pop(Machine *pmach, Instruction instr, unsigned addr) 
{
	checkNotI(instr,addr);

	unsigned int address = getAddr(pmach, instr);
	checkDataAddr(pmach, address, addr);

	++pmach->_sp;
	checkSP(pmach, addr);

	pmach->_data[address] = pmach->_data[pmach->_sp];

	return true;
}

bool decode_execute(Machine *pmach, Instruction instr) {
    unsigned COP = instr.instr_generic._cop;
	unsigned addr = pmach->_pc - 1;
	switch (COP) {
	    case ILLOP:
            error(ERR_ILLEGAL, addr);
        case NOP:
            return true;

        case LOAD:
            return load(pmach, instr, addr);
        case STORE:
            return store(pmach, instr, addr);
        case ADD:
            return add(pmach, instr, addr);
        case SUB:
            return sub(pmach, instr, addr);

        case BRANCH:
            return branch(pmach, instr, addr);
        case CALL:
            return call(pmach, instr, addr);
        case RET:
            return ret(pmach, instr, addr);

        case PUSH:
            return push(pmach, instr, addr);
        case POP:
            return pop(pmach, instr, addr);

        case HALT:
            warning(WARN_HALT, addr);
            return false;

        default:
            error(ERR_UNKNOWN, addr);
	}
}

void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr) {
	printf("TRACE: %s: 0x%04x: ", msg, addr);
	print_instruction(instr, addr);
	printf("\n");
}
