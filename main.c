/*-----------------------------------------FAP---------------------------------------------*/

/**< #########################################################################################
 * ##################################     Includes     #######################################
 * #########################################################################################*/
#include "gpio.h"
#include "adc.h"
#include "timer.h"
#include "pwm.h"
#include <math.h>

/**< #########################################################################################
 * ##############################     Variáveis Globais     ##################################
 * #########################################################################################*/
uint32_t count = 0;
uint64_t index = 0;
uint16_t Vgrid = 0;
uint16_t Iload = 0;
uint16_t Ifap = 0;
//uint16_t plot[400];
//uint16_t *adc = &adc1;

uint16_t flag_periodic_sample = 0, flag_periodic_sample_low = 0;

/**< #########################################################################################
 * #############################      Protótipo Funções      #################################
 * #########################################################################################*/
__interrupt void isr_cpu_timer0(void);
__interrupt void isr_adc(void);

void period_sampling();

/**< #########################################################################################
 * ####################################      Main      #######################################
 * #########################################################################################*/
int main(void){
    // Step 1. Initialize System Control:
    // PLL, WatchDog , enable Peripheral Clocks InitSysCtrl
    // Step 2. Initialize Funcs
    InitGpio();


    EALLOW;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER0 = 1;
    EDIS;

    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    DINT;                                   // Disable CPU interrupts

    // Initialize PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags are cleared.
    InitPieCtrl();
    // Disable CPU interrupts and Clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;
    // Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).
    InitPieVectTable();

    Setup_GPIO();
    GPIO_PWM();
    Setup_PWM();
    Setup_ADC();

    EALLOW;
    PieVectTable.TIMER0_INT = &isr_cpu_timer0;
    PieVectTable.ADCA1_INT = &isr_adc;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;      //Timer 0
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;      //ADC
    EDIS;
    IER |= M_INT1;

    Setup_Timer0();

    EINT;                                   // Enable Global interrupt INTM
    ERTM;                                   // Enable Global realtime interrupt DBGM

    GpioDataRegs.GPBDAT.bit.GPIO34 = 1;
    GpioDataRegs.GPADAT.bit.GPIO31 = 0;
    while(1){
//        for(count = 0; count < 0xFFFFF ; count++);
//        //GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
//        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
        if(flag_periodic_sample)
            period_sampling();
        flag_periodic_sample = 0;
    }
    return 0;
}
/**< #########################################################################################
 * ####################################      ISRs      #######################################
 * #########################################################################################*/

/**< ########################################
 * - Timer 0
 * ##########################################*/
__interrupt void isr_cpu_timer0(void){
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

/**< ########################################
 * - ADC
 * ##########################################*/
__interrupt void isr_adc(void){
    Vgrid =  AdcaResultRegs.ADCRESULT0;
    Iload =  AdcaResultRegs.ADCRESULT1;
    Ifap =  AdcaResultRegs.ADCRESULT2;

//    if(index == 39000)
//        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    index = (index == 40000) ? 0 : (index+1);

    flag_periodic_sample = 1;

//    plot[index] = *adc;

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;      //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

/**< ########################################
 * - perid_sampling
 * ##########################################*/
void period_sampling(){
    if(flag_periodic_sample_low){
        EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET; // clear PWM2A on event a, up count
        flag_periodic_sample_low = 0;
    }
    else{
        EPwm2Regs.AQCTLA.bit.ZRO = AQ_CLEAR; // clear PWM2A on event a, up count
        flag_periodic_sample_low = 1;
    }
}


/**< #########################################################################################
 * ##################################      END MAIN      #####################################
 * #########################################################################################*/
