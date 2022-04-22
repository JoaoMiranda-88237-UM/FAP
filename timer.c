/*
 * timer.c
 *
 *  Created on: 21/04/2022
 *      Author: JoaoMiranda
 */

/**< #########################################################################################
 * ##################################     Includes     #######################################
 * #########################################################################################*/
#include "timer.h"

/**< #########################################################################################
 * ##################################     Defines     ########################################
 * #########################################################################################*/
#define CPUFREQ_MHZ 200
#define Timer0_PRD 1000

/**< ########################################
 * - TIMER 0
 * ##########################################*/
void Setup_Timer0(void)
{
    InitCpuTimers();
    //This function initializes all three CPU timers to a known state [fclock = 200 MHz]
    //void ConfigCpuTimer(struct CPUTIMER_VARS *Timer, float Freq (do CPU), float Period)
    ConfigCpuTimer(&CpuTimer0, CPUFREQ_MHZ, Timer0_PRD);     //The "Freq" is entered as "MHz" and the period in "uSeconds". The period is time at time to trigger
    CpuTimer0Regs.TCR.all = 0x4001;                  //Enable TIE [Time interrupt enable] {0100 0000 0000 0001 = 0x4001}
    //CpuTimer0Regs.TCR.bit.TIE = 1;
}
