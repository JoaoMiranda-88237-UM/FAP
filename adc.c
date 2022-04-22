/*
 * adc.c
 *
 *  Created on: 21/04/2022
 *      Author: JoaoMiranda
 */

/**< #########################################################################################
 * ##################################     Includes     #######################################
 * #########################################################################################*/
#include "adc.h"

/**< ########################################
 * - Setup ADC
 * [Inicializar os 3 ADCs:
 *          - SOC0 -> ADCIN14 (23) Vgrid
 *          - SOC1 -> ADCIN3  (26) Iload
 *          - SOC2 -> ADCIN0  (30) Ifap
 * ##########################################*/
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

    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 14;          //SOC0 will convert pin ADCIN14 (23) Va
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;      //sample window is 15 SYSCLK cycles
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;  //trigger on ePWM7 SOCA

    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 3;          //SOC1 will convert pin ADCINA3 (26) Vb
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;

    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 0;          //SOC2 will convert pin ADCINA0 (30) Vc
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = TRIG_SEL_ePWM1_SOCA;

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0x02;       // end of SOC2 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;            // enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;          // make sure INT1 flag is cleared

    EDIS;
}
