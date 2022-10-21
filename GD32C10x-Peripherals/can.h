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

/* configure the CAN0/1 speed and parameters */
void can_param_config(uint32_t can_periph, unsigned char *str);

/* Put CAN0/1 in sleep mode */
void can_sleep_mode(uint32_t can_periph);

/* Wake CAN0/1 from sleep mode */
void can_awake(uint32_t can_periph);

unsigned long char2long(unsigned char *str);

#endif /* CAN_H */
