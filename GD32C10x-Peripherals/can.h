/*!
    \file    can.h
    \brief   the header file of CAN

*/

#ifndef CAN_H
#define CAN_H

#include "gd32c10x.h"



#define CAN0_RX_PORT            GPIOA
#define CAN0_TX_PORT            GPIOA
#define CAN0_RX_PIN             GPIO_PIN_11
#define CAN0_TX_PIN             GPIO_PIN_12

#define CAN1_RX_PORT            GPIOB
#define CAN1_TX_PORT            GPIOB
#define CAN1_RX_PIN             GPIO_PIN_12
#define CAN1_TX_PIN             GPIO_PIN_13

/* function declarations */
/* configure the GPIO ports */
void can_gpio_config(void);


#endif /* CAN_H */
