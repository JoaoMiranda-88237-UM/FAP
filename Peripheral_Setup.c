/*
 * Peripheral_Setup.c
 *
 *  Created on: 28/02/2022
 *      Author: duart
 */

#include "Peripheral_Setup.h"

void Setup_GPIO() {
    EALLOW;
    // LED 31 A, 2
    // LED 34 B, 1
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;

    GpioCtrlRegs.GPBGMUX1.bit.GPIO34 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;

    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 1;

    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;

    GpioCtrlRegs.GPBCSEL1.bit.GPIO34 = GPIO_MUX_CPU1;
    GpioCtrlRegs.GPACSEL4.bit.GPIO31 = GPIO_MUX_CPU1;

    GpioCtrlRegs.GPBCSEL1.bit.GPIO34 = GPIO_MUX_CPU1;
    GpioCtrlRegs.GPACSEL4.bit.GPIO31 = GPIO_MUX_CPU1;

    //PWM 1A e 1B
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;

    GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;

    //PWM 7A e 8A
    GpioCtrlRegs.GPEGMUX2.bit.GPIO157 = 0;
    GpioCtrlRegs.GPEMUX2.bit.GPIO157 = 1;
    GpioCtrlRegs.GPEPUD.bit.GPIO157 = 1;

    GpioCtrlRegs.GPEGMUX2.bit.GPIO159 = 0;
    GpioCtrlRegs.GPEMUX2.bit.GPIO159 = 1;
    GpioCtrlRegs.GPEPUD.bit.GPIO159 = 1;

    //GPIO 14
    GpioCtrlRegs.GPAGMUX1.bit.GPIO14 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO14 = 1;
    GpioCtrlRegs.GPACSEL2.bit.GPIO14 = GPIO_MUX_CPU1;

    EDIS;
}

void Setup_PWM() {
    EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM7 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM8 = 1;

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;

    EPwm1Regs.TBPRD = 5000;                         // Set timer period
    EPwm1Regs.CMPA.bit.CMPA = EPwm1Regs.TBPRD >> 1;

    EPwm1Regs.TBPHS.bit.TBPHS = 0;                  // Phase is 0
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
    EPwm1Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up/down
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;         // Disable phase loading
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm1Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;            // set actions for EPWM1A
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;

    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;       // Active Hi complementary
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;  // enable Dead-band module
    EPwm1Regs.DBFED.bit.DBFED = 100;                // FED = 20 TBCLKs
    EPwm1Regs.DBRED.bit.DBRED = 100;                // RED = 20 TBCLKs

    //Trigger ADC
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;                 // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_PRDZERO;   // Dispara ADC no topo
    EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;            // Trigger on every event


    // PWM 7A
    EPwm7Regs.TBPRD = 5000;                         // Set timer period
    EPwm7Regs.CMPA.bit.CMPA = 0;
    EPwm7Regs.TBPHS.bit.TBPHS = 0;                  // Phase is 0
    EPwm7Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwm7Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up/down
    EPwm7Regs.TBCTL.bit.PHSEN = TB_ENABLE;          // Disable phase loading
    EPwm7Regs.TBCTL.bit.PHSDIR = TB_DOWN;           // Phase UP/DOWN
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;

    EPwm7Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm7Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm7Regs.AQCTLA.bit.CAU = AQ_CLEAR;            // set actions for EPWM1A
    EPwm7Regs.AQCTLA.bit.CAD = AQ_SET;

    // PWM 7B
    EPwm8Regs.TBPRD = 5000;                         // Set timer period
    EPwm8Regs.CMPA.bit.CMPA = 0;
    EPwm8Regs.TBPHS.bit.TBPHS = 0;                  // Phase is 0
    EPwm8Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwm8Regs.TBCTR = 0x0000;                       // Clear counter
    EPwm8Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up/down
    EPwm8Regs.TBCTL.bit.PHSEN = TB_ENABLE;          // Disable phase loading
    EPwm8Regs.TBCTL.bit.PHSDIR = TB_DOWN;           // Phase UP/DOWN
    EPwm8Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;        // Clock ratio to SYSCLKOUT
    EPwm8Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm8Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;     // Load registers every ZERO
    EPwm8Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;

    EPwm8Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm8Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm8Regs.AQCTLA.bit.CAU = AQ_CLEAR;            // set actions for EPWM1A
    EPwm8Regs.AQCTLA.bit.CAD = AQ_SET;

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

void Setup_ADC() {
    Uint16 acqps;
    //determine minimum acquisition window (in SYSCLKS) based on resolution
    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION)
        acqps = 14;                             // 75ns
    else                                        // resolution is 16-bit
        acqps = 63;                             // 320ns

    EALLOW;
    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;          // set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;       // Set pulse um ciclo antes do resultado
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;          // power up the ADC
    DELAY_US(1000);                             // delay for 1ms to allow ADC time to power up

    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 3;
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;          //sample window is 15 SYSCLK cycles
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 0x05; //trigger on ePWM2 SOCA

    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 4;
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 0x05;

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0x01;       // end of SOC1 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;            // enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;          // make sure INT1 flag is cleared

    EDIS;

}

