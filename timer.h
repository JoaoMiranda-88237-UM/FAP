/*
 * timer.h
 *
 *  Created on: 20/04/2022
 *      Author: JoaoMiranda
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "F28x_Project.h"

void Setup_Timer0(void);
__interrupt void isr_cpu_timer0(void);

#endif /* TIMER_H_ */
