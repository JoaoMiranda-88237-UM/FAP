/*
 * adc.c
 *
 *  Created on: 19/04/2022
 *      Author: JoaoMiranda
 */
#include "adc.h"

#define TRIG_SEL_ePWM7_SOCA 0X11

uint16_t index = 0;
uint16_t adc1 = 0;
uint16_t adc2 = 0;
uint16_t adc3 = 0;
uint16_t plot[400];
uint16_t *adc = &adc1;

/**< F2837xD GPIO Pin Registers
 * GPxDIR -> [0 - Input | 1 - Output]
 * GPxPUD -> [0 - Internal Pull-Up enable | 1 - Internal Pull-Up disable]
 * GPxODR -> [0 - Normal | 1 - Open Drain]
 * GPxMUX -> Define if that pin will be used to GPIO or to (ex: PWM / Serial Pin) -> [tms320f28379d.pdf [49/229]]
 * GPxCSEL -> Define which CPU will be used
 * */
void GPIO_ADC(void){
//    GpioCtrlRegs.GPADIR.bit.GPIO31 = 0;
//    GpioCtrlRegs.GPADIR.bit.GPIO31 = 0;
}

void Setup_ADC(void)
{
    Uint16  acqps;

    EALLOW;                                  //Enable to edit
    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;       //Enable ADC clock
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;       //Set ADCCLK divider to /4
    AdcaRegs.ADCCTL2.bit.RESOLUTION = ADC_RESOLUTION_12BIT;
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;    //Set oulso um ciclo antes do resultado
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;       //Power up the ADC

    DELAY_US(1000);

    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION)
        acqps = 14;
    else
        acqps = 63;

//    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 14;          //SOC0 will convert pin ADCIN14 (23) Va
//    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;      //sample window is 15 SYSCLK cycles
//    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = TRIG_SEL_ePWM7_SOCA;  //trigger on ePWM7 SOCA

    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 3;          //SOC1 will convert pin ADCINA3 (26) Vb
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps;
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = TRIG_SEL_ePWM7_SOCA;

//    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 0;          //SOC2 will convert pin ADCINA0 (30) Vc
//    AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps;
//    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = TRIG_SEL_ePWM7_SOCA;

    //Interrupt
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0x01;  //End of SOC1 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;       //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;     //make sure INT1 flag is clear

    EDIS;
}

__interrupt void isr_adc(void){
    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
    index = (index==124) ? 0 : (index+1);

    adc1 = AdcbResultRegs.ADCRESULT0;
    adc2 = AdcbResultRegs.ADCRESULT1;

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;      //make sure INT1 flag is clear
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;     // Acknowledge -> Clear flag of interrupt
}

