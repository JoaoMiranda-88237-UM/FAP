/*
 * pwm.c
 *
 *  Created on: 21/04/2022
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
    //Config gpio for ePWM
    InitEPwm2Gpio();
    InitEPwm7Gpio();

    EALLOW;

    //PWM 2A e 2B
    GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;

    //PWM 7A
    GpioCtrlRegs.GPEGMUX2.bit.GPIO157 = 0;
    GpioCtrlRegs.GPEMUX2.bit.GPIO157 = 1;
    GpioCtrlRegs.GPEPUD.bit.GPIO157 = 1;

    EDIS;
}

void Setup_PWM(void){
    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM7 = 1;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;

    //PWM_Period_StepUp = 200 MHz/40*2*2 kHz = 1250
    EPwm7Regs.TBPRD = 1250;                         // Set timer period
    EPwm7Regs.CMPA.bit.CMPA = EPwm1Regs.TBPRD >> 1;

    EPwm7Regs.TBPHS.bit.TBPHS = 0;                  // Phase is 0
    EPwm7Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
    EPwm7Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up/down
    EPwm7Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm7Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm7Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm7Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm7Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm7Regs.AQCTLA.bit.CAU = AQ_CLEAR;            // set actions for EPWM1A
    EPwm7Regs.AQCTLA.bit.CAD = AQ_SET;

    EPwm7Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // Active Hi complementary
    EPwm7Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  // enable Dead-band module
    EPwm7Regs.DBFED.bit.DBFED = 0;                // FED = 20 TBCLKs
    EPwm7Regs.DBRED.bit.DBRED = 0;                // RED = 20 TBCLKs

    //###########------PWM2A e B-------###########

    //PWM_Period_StepUp = 200 MHz/40 kHz = 5000
    EPwm2Regs.TBPRD = 250 ; // Period = 251 TBCLK counts
    EPwm2Regs.TBPHS.bit.TBPHS = 0; // Set Phase register to zero
    EPwm2Regs.TBCTR = 0; // clear TB counter

    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count mode up
    EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // Synchronize ePWM on CTR = zero
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE; // disable phase loading
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // TBCLK = SYSCLK
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW; // period autoreload

    // Setup shadow register load on ZERO
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR = Zero
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR = Zero

    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR; // clear PWM2A on event a, up count
    EPwm2Regs.AQCTLA.bit.CAD = AQ_SET; // set PWM2A on event ZRO, up count

    // Dead time config
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // Enabling rising and falling edge delay
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active High complementary
    EPwm2Regs.DBFED.bit.DBFED = 100; // set 100 clock ticks of rising edge dead time ( x us)
    EPwm2Regs.DBRED.bit.DBRED = 100; // set 100 clock ticks of falling edge dead time ( x us)

    //Reset to the previous configurations
    EPwm2Regs.AQCTLA.bit.CAU = AQ_NO_ACTION; // reset PWM2A on event a, up count
    EPwm2Regs.AQCTLA.bit.CAD = AQ_NO_ACTION; // reset PWM2A on event ZRO, up count

    //Atualizar
    EPwm2Regs.AQCTLA.bit.ZRO = AQ_CLEAR; // clear PWM2A on event a, up count

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

/*DBFED, DBRED | TBCLK = EPWMCLK/1 | TBCLK = EPWMCLK/2 | TBCLK = EPWMCLK/4
        1              0.01 uS          0.02 uS           0.04 uS
        5              0.05 uS          0.10 uS           0.20 uS
        10             0.10 uS          0.20 uS           0.40 uS
       100             1.00 uS          2.00 uS           4.00 uS
       200             2.00 uS          4.00 uS           8.00 uS
       400             4.00 uS          8.00 uS           16.00 uS
       500             5.00 uS          10.00 uS          20.00 uS
       600             6.00 uS          12.00 uS          24.00 uS
       700             7.00 uS          14.00 uS          28.00 uS
       800             8.00 uS          16.00 uS          32.00 uS
       900             9.00 uS          18.00 uS          36.00 uS
       1000            10.00 uS         20.00 uS          40.00 uS*/
