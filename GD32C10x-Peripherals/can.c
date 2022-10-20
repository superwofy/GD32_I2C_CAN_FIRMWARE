/*!
    \file    can.c
    \brief   CAN configuration file

*/

#include "gd32c10x.h"
#include "can.h"
#include <stdio.h>

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_CAN1);
    rcu_periph_clock_enable(RCU_AF);

	/* configure CAN0 GPIO */
    gpio_init(CAN0_RX_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, CAN0_RX_PIN);
    gpio_init(CAN0_TX_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, CAN0_TX_PIN);
	
    /* configure CAN1 GPIO */
    gpio_init(CAN1_RX_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, CAN1_RX_PIN);
    gpio_init(CAN1_TX_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, CAN1_TX_PIN);
}

