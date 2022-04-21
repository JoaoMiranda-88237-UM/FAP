/*
 * timer.c
 *
 *  Created on: 20/04/2022
 *      Author: JoaoMiranda
 */

#include "timer.h"

void Setup_Timer0(void)
{
    InitCpuTimers();                                 //This function initializes all three CPU timers to a known state [fclock = 200 MHz]
    //void ConfigCpuTimer(struct CPUTIMER_VARS *Timer, float Freq (do CPU), float Period)
    ConfigCpuTimer(&CpuTimer0, 200, 1000000);        //The "Freq" is entered as "MHz" and the period in "uSeconds". The period is time at time to trigger
    CpuTimer0Regs.TCR.all = 0x4001;                  //Enable TIE [Time interrupt enable] {0100 0000 0000 0001 = 0x4001}
    //CpuTimer0Regs.TCR.bit.TIE = 1;
}

__interrupt void isr_cpu_timer0(void){
    GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;      // Toggle pin
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;     // Acknowledge -> Clear flag of interrupt
}

