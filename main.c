#include "Peripheral_Setup.h"
#include "math.h"

/**
 * main.c
 */
uint32_t count = 0, index = 0;
uint16_t sinetable[400];

uint16_t adc1 = 0;
uint16_t adc2 = 0;

uint16_t plot[400];
uint16_t *adc =  &adc1;


__interrupt void isr_cpu_timer0(void);
__interrupt void isr_adc(void);

int main(void){
    InitSysCtrl();                          // Initialize System Control:

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER0 = 1;
    EDIS;

    DINT;                                   // Disable CPU interrupts
    InitPieCtrl();                          // Initialize the PIE control registers to their default state
    IER = 0x0000;                           // Disable CPU interrupts
    IFR = 0x0000;                           // Clear all CPU interrupt flags:
    InitPieVectTable();                     // Initialize the PIE vector table

    Setup_GPIO();
    Setup_PWM();
    Setup_ADC();

    EALLOW;
    PieVectTable.TIMER0_INT = &isr_cpu_timer0;
    PieVectTable.ADCA1_INT = &isr_adc;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;      //Timer 0
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;      //ADC
    EDIS;
    IER |= M_INT1;

    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0, 200, 1000000);
    CpuTimer0Regs.TCR.all = 0x4001;

    for(index = 0; index < 400; index++){
        sinetable[index] = 500 + (uint16_t)(2000.0*(1.0 + sin(6.28318531/400.0*((float)index))));
    }
    index = 0;

    EINT;                                   // Enable Global interrupt INTM
    ERTM;                                   // Enable Global realtime interrupt DBGM
    GpioDataRegs.GPBDAT.bit.GPIO34 = 1;
    GpioDataRegs.GPADAT.bit.GPIO31 = 0;
    while(1){
        for(count = 0; count < 0x00FFFFFF; count++){

        }
        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
        //GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    }
    return 0;
}

__interrupt void isr_cpu_timer0(void){
    GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void isr_adc(void){
    GpioDataRegs.GPADAT.bit.GPIO14 = 1;

    adc1 =  AdcaResultRegs.ADCRESULT0;
    adc2 =  AdcaResultRegs.ADCRESULT1;

    index = (index == 400) ? 0 : (index+1);

    EPwm7Regs.CMPA.bit.CMPA = sinetable[index];
    EPwm8Regs.CMPA.bit.CMPA = sinetable[index];

    plot[index] = *adc;


    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;      //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    GpioDataRegs.GPADAT.bit.GPIO14 = 0;
}


