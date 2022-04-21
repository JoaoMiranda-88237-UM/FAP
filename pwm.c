/*
 * pwm.c
 *
 *  Created on: 20/04/2022
 *      Author: JoaoMiranda
 */

#include "pwm.h"

/**< F2837xD GPIO Pin Registers
 * GPxDIR -> [0 - Input | 1 - Output]
 * GPxPUD -> [0 - Internal Pull-Up enable | 1 - Internal Pull-Up disable]
 * GPxODR -> [0 - Normal | 1 - Open Drain]
 * GPxMUX -> Define if that pin will be used to GPIO or to (ex: PWM / Serial Pin) -> [tms320f28379d.pdf [49/229]]
 * GPxCSEL -> Define which CPU will be used
 * */
void GPIO_PWM(void)
{
    EALLOW;

//    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;       //Disable pull-up on GPIO2 (EPWM2A)
//    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;       //Disable pull-up on GPIO3 (EPWM2B)
//    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;      //Configure GPIO2 as EPWM2A
//    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;      //Configure GPIO3 as EPWM2B

    //PWM 7A
    //GpioCtrlRegs.GPEGMUX2.bit.GPIO157 = 0;
    GpioCtrlRegs.GPEMUX2.bit.GPIO157 = 1;
    GpioCtrlRegs.GPEPUD.bit.GPIO157 = 1;

    EDIS;
}

void Setup_PWM(void){
    // pg 1978 spruhm8i.pdf - Technical reference
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    CpuSysRegs.PCLKCR2.bit.EPWM7 = 1;
    EDIS;

    EPwm7Regs.TBPRD = 3255;                         // Set timer period
    EPwm7Regs.CMPA.bit.CMPA = EPwm7Regs.TBPRD >> 1;
    EPwm7Regs.TBCTR = 0x0000;                       // Clear counter

    EPwm7Regs.TBPHS.bit.TBPHS = 0;                  // Phase is 0
    EPwm7Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm7Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading
    EPwm7Regs.TBCTL.bit.PHSDIR = 0;
    EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up/down
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;

    EPwm7Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm7Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm7Regs.AQCTLA.bit.CAU = AQ_CLEAR;            // set actions for EPWM1A
    EPwm7Regs.AQCTLA.bit.CAD = AQ_SET;

    //Trigger ADC
    EPwm7Regs.ETSEL.bit.SOCAEN = 1;                 // Enable SOC on A group
    EPwm7Regs.ETSEL.bit.SOCASEL = ET_CTR_PRDZERO;   // Dispara ADC no topo
    EPwm7Regs.ETPS.bit.SOCAPRD = ET_1ST;            // Trigger on every event

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

