#include "gpio.h"
#include "adc.h"
#include "timer.h"
#include "pwm.h"
#include <math.h>

/**
 * main.c
 */

int main(void)
{
    /**< Initialize System Control [with some clocks] -> CPU1 | _LAUNCHXL_F28379D;
     * Configure Clock [CLKIN is a 10 MHz]
     * After Clock is defined as 10MHz * 40 / 2 = 200 MHz */
    InitSysCtrl();
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER0 = 1;
    EDIS;
    InitGpio();

    DINT;                               // Disable CPU interrupts (INTM)
    InitPieCtrl();                      // Initialize the PIE (peripherals) control registers to their default state [Clear all interrupt vector]
    IER = 0x0000;                       // Disable CPU interrupts
    IFR = 0x0000;                       // Clear all CPU interrupt flags
    /**< Initialize the PIE (peripherals) vector table [interrupts table [12 lines and each line with 16 colums]]
     * 12 interrupts
     * 16 peripherals
     * Peripheral Interrupts = 12*16 = 192
     *  */
    InitPieVectTable();

    Setup_GPIO();
    Setup_Timer0();
    Setup_ADC();
    Setup_PWM();
    GPIO_PWM();

    EALLOW;
    PieVectTable.TIMER0_INT = &isr_cpu_timer0;      //Mapping function to access this function when timing trigger
    PieVectTable.ADCA1_INT = &isr_adc;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;              //Enable Column 7 - line 1 [TINT0 - Timer0 interrupt]
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;              //ADC
    EDIS;
    IER |= M_INT1;                                  //Enable interrupt on line 1
    //{In case of add other interrupt on line 2 -> IER |= M_INT1 | M_INT2;}

    EINT;                               // Enable Global interrupt INTM
    ERTM;                               // Enable Global real time interrupt DBGM (Debug)

    GpioDataRegs.GPBDAT.bit.GPIO34 = 1; // Write on Pin - LED RED -> DISABLE
    GpioDataRegs.GPADAT.bit.GPIO31 = 0; // Write on Pin - LED BLUE -> Enable

    uint32_t count = 0;
    while(1){
//        for(count = 0; count < 0x00FFFFFF; count++);
//        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;  // Toggle pin
    }

	return 0;
}

