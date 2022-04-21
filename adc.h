/*
 * adc.h
 *
 *  Created on: 19/04/2022
 *      Author: JoaoMiranda
 */

#ifndef ADC_H_
#define ADC_H_

#include "F28x_Project.h"

void GPIO_ADC(void);
void Setup_ADC(void);
__interrupt void isr_adc(void);


#endif /* ADC_H_ */
