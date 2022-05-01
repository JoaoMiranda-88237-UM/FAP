/*
 * dac.c
 *
 *  Created on: 27/04/2022
 *      Author: JoaoMiranda
 */

/**< #########################################################################################
 * ##################################     Includes     #######################################
 * #########################################################################################*/
#include "dac.h"

/**< ########################################
 * - Setup DAC
 * DACOUTB -> PIN 70
 * ##########################################*/
void Setup_DAC() {
    EALLOW;
    CpuSysRegs.PCLKCR16.bit.DAC_B = 1;      //Habilitar CLK
    DacbRegs.DACCTL.bit.SYNCSEL = 0x07;     //SYNC with PWM7
    DacbRegs.DACCTL.bit.LOADMODE = 0x01;
    DacbRegs.DACCTL.bit.DACREFSEL = 0x01;
    DacbRegs.DACVALS.bit.DACVALS = 0; //12 bits
    DacbRegs.DACOUTEN.bit.DACOUTEN = 1;
    DacbRegs.DACLOCK.all = 0x00;
    EDIS;
}
