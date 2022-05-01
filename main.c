/*-----------------------------------------FAP---------------------------------------------*/

/**< #########################################################################################
 * ##################################     Includes     #######################################
 * #########################################################################################*/
#include "gpio.h"
#include "adc.h"
#include "timer.h"
#include "pwm.h"
#include "dac.h"
#include <math.h>

/**< #########################################################################################
 * ##################################     Defines     #######################################
 * #########################################################################################*/
#define uint8_t unsigned char
#define int8_t char

/**< #########################################################################################
 * ##############################     Variáveis Globais     ##################################
 * #########################################################################################*/
volatile uint8_t flag_adc = 0;
volatile long ValorMedio_soma = 0;
volatile int Vgrid_Med = 0;
volatile int Igrid_Med = 0;
float debug_vv=0.0;

/**< #########################################################################################
 * #############################      Protótipo Funções      #################################
 * #########################################################################################*/
__interrupt void isr_cpu_timer0(void);
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
    Setup_DAC();
    Setup_Timer0();

    CpuTimer0Regs.TCR.all = 0x4001; // Start timer

    EALLOW;
    PieVectTable.TIMER0_INT = &isr_cpu_timer0;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;      //Timer 0
    EDIS;
    IER = 0x101;                            //IER |= M_INT1;

    EINT;                                   // Enable Global interrupt INTM
    ERTM;                                   // Enable Global realtime interrupt DBGM

    GpioDataRegs.GPBDAT.bit.GPIO34 = 1;     // blue led
    GpioDataRegs.GPADAT.bit.GPIO31 = 0;     // red led

    //Calculo Valor Médio
    int i = 0;
    for(i = 0; i < 800 ; i++)
    {
        flag_adc = 0;
        while(!flag_adc);
        while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0){}
           AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
       ValorMedio_soma += AdcaResultRegs.ADCRESULT0;        //Vgrid
    }
    Vgrid_Med = (int)(ValorMedio_soma/800.0);
    ValorMedio_soma = 0;

    for(i = 0; i < 800 ; i++)
    {
        flag_adc = 0;
        while(!flag_adc);
        while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0){}
       AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
       ValorMedio_soma += AdcaResultRegs.ADCRESULT1;        //Igrid
    }
    Igrid_Med = (int)(ValorMedio_soma/800.0);

    while(1){
        flag_adc = 0;
        period_sampling();
        while(!flag_adc);
    }//end while

}// end main()


/**< #########################################################################################
 * ####################################      ISRs      #######################################
 * #########################################################################################*/

/**< ########################################
 * - Timer 0
 * ##########################################*/
__interrupt void isr_cpu_timer0(void){

    AdcaRegs.ADCSOCFRC1.all = 0x07;   //SOC0, SOC1 and SOC2

    flag_adc = 1;

    //AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;      //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

/**< ########################################
 * - perid_sampling
 * ##########################################*/
void period_sampling(){

    static uint8_t flag_up = 0;
    static uint8_t flag_up_ant = 0;

    static int voltageM[4] = {0};

    static int Vgrid = 0;
    static long Vgrid_quad = 0;
    static long long Vgrid_quad_sum = 0;
    static long Vmedio_quad = 0;

    static long Pinst = 0;
    static long long Psum = 0;
    static long Pmedio = 0;

    static int Iload = 0;
    static int Ifap = 0;

    static float iG = 0.0;
    static float iref = 0.0;

    static float G = 0.0;



    //######-Plot-#####
//    static uint index = 0;
//    static long plot[200];
//    static long *adc1 = &Pmedio;

    //wait for ADCA to complete, then acknowledge flag
    while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0){}
        AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

    Vgrid =  (int)(AdcaResultRegs.ADCRESULT0 - Vgrid_Med);
    Iload =  AdcaResultRegs.ADCRESULT1 - Igrid_Med;
    Ifap =  AdcaResultRegs.ADCRESULT2;

//    EALLOW;
//    DacbRegs.DACVALS.bit.DACVALS = AdcaResultRegs.ADCRESULT0;
//    EDIS;

    voltageM[3] = Vgrid;

    //GpioDataRegs.GPATOGGLE.bit.GPIO14 = 1;

    if( ((voltageM[0]+voltageM[1])<0) && ((voltageM[2]+voltageM[3])>0) && !flag_up ){
        flag_up = 1;
        //GpioDataRegs.GPASET.bit.GPIO14 = 1;
        //GpioDataRegs.GPATOGGLE.bit.GPIO14 = 1;
    }
    else{
        if( ((voltageM[0]+voltageM[1]+voltageM[2]+voltageM[3])<-100) ){
            flag_up = 0;
            //GpioDataRegs.GPACLEAR.bit.GPIO14 = 1;
        }
    }

    GpioDataRegs.GPASET.bit.GPIO14 = 1;

    voltageM[0] = voltageM[1];
    voltageM[1] = voltageM[2];
    voltageM[2] = voltageM[3];

    Vgrid_quad = (long)(Vgrid) * (long)(Vgrid);
    Vgrid_quad_sum += Vgrid_quad;
    Pinst = (long)(Vgrid) * (long)(Iload);
    Psum += Pinst;

    if (flag_up == 1 && flag_up_ant == 0)//->Sincronizar à frequencia da rede
    {
        GpioDataRegs.GPATOGGLE.bit.GPIO14 = 1;
        Vmedio_quad = Vgrid_quad_sum / 800;
        Pmedio = Psum / 800;
        Vgrid_quad_sum = 0;
        Psum = 0;

        G = (float)(Pmedio) / (float)Vmedio_quad;

    }
    flag_up_ant = flag_up;

    iG = (float)(Vgrid) * G;
    //iref = (float)(Iload) - iG;
    iref = 14;


    if(iref >= (float)Ifap)
        EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET; // clear PWM2A on event a, up count //-------->Q1
    else
        EPwm2Regs.AQCTLA.bit.ZRO = AQ_CLEAR; // clear PWM2A on event a, up count

    EALLOW;
        //DacbRegs.DACVALS.bit.DACVALS = AdcaResultRegs.ADCRESULT0;
        DacbRegs.DACVALS.bit.DACVALS = iref;
    EDIS;

//    index = (index == 200) ? 0 : (index+1);
//    plot[index] = *adc1;
}

/**< #########################################################################################
 * ##################################      END MAIN      #####################################
 * #########################################################################################*/
