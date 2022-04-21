/*
 * Peripheral_Setup.c
 *
 *  Created on: 18/04/2022
 *      Author: JoaoMiranda
 */

#include "gpio.h"

/**< F2837xD GPIO Pin Registers
 * GPxDIR -> [0 - Input | 1 - Output]
 * GPxPUD -> [0 - Internal Pull-Up enable | 1 - Internal Pull-Up disable]
 * GPxODR -> [0 - Normal | 1 - Open Drain]
 * GPxMUX -> Define if that pin will be used to GPIO or to (ex: PWM / Serial Pin) -> [tms320f28379d.pdf [49/229]]
 * GPxCSEL -> Define which CPU will be used
 * */
void Setup_GPIO(void)
{
   EALLOW;                                  //Enable to edit

   GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;     //LED BLUE
   GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
   GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;     //LED RED
   GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;

   EDIS;                                    //Disable to edit
}





