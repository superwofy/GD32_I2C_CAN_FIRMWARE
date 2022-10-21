/*!
    \file    can.c
    \brief   CAN configuration file

*/

#include <stdio.h>
#include "gd32c10x.h"
#include "can.h"
#include "dual_dfs.h"


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

/*!
    \brief      configure the CAN0/1 speed and parameters
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_param_config(uint32_t can_periph, unsigned char *conf_str) 
{
    unsigned long __can20baud = char2long(&conf_str[0]);
		can_parameter_struct can_parameter;

    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    /* initialize CAN register */
    can_deinit(can_periph);

    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = ENABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = ENABLE;
    can_parameter.rec_fifo_overwrite = ENABLE;
    can_parameter.trans_fifo_order = ENABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    /* initialize CAN */
    can_init(can_periph, &can_parameter);
	
		can_fd_function_disable(can_periph);																																													// FD disable

    /* config can_periph baud rate */
    can_frequency_set(can_periph, __can20baud);

    /* Set CAN1 filter offset start */
    can1_filter_start_bank(14);
		
		
		for (int i = 0; i < 14; i++) {
				if (conf_str[8 + i * 10]) {
						unsigned long __mask = char2long(&conf_str[i * 10 + 10]);
						unsigned long __filt = char2long(&conf_str[i * 10 + 14]);
						int __ext = conf_str[i * 10 + 9] ? CAN_EXTENDED_FIFO0 : CAN_STANDARD_FIFO0;
					
						if (CAN0 == can_periph) {
								can_filter_mask_mode_init(__filt, __mask, __ext, i);
#if DEBUG
							printf("\r\nCAN0 filter %d initialized to %ld, mask set to %ld", i, __filt, __mask);
#endif
						}
						else {
								can_filter_mask_mode_init(__filt, __mask, __ext, i + 14);
#if DEBUG
								printf("\r\nCAN1 filter %d initialized to %ld, mask set to %ld", i, __filt, __mask);
#endif
						}
				} 
#if DEBUG
				else {
						printf("\r\n%s filter %d disabled", CAN0 == can_periph ? "CAN0" : "CAN1", i);
				}
#endif
		}
		
		if (CAN0 == can_periph) {
				/* configure CAN0 NVIC */
				nvic_irq_enable(CAN0_RX0_IRQn, 0, 0);
				/* enable can receive FIFO0 not empty interrupt */
				can_interrupt_enable(CAN0, CAN_INTEN_RFNEIE0);
		}
		else {
				/* configure CAN1 NVIC */
				nvic_irq_enable(CAN1_RX0_IRQn, 1, 0);
				/* enable can receive FIFO0 not empty interrupt */
				can_interrupt_enable(CAN1, CAN_INTEN_RFNEIE0);
		}
}


void can_sleep_mode(uint32_t can_periph)
{
		can_working_mode_set(can_periph, CAN_MODE_SLEEP);
#if DEBUG
		printf("\r\%s sleeping.", CAN0 == can_periph ? "CAN0" : "CAN1");
#endif
}


void can_awake(uint32_t can_periph)
{
		can_working_mode_set(can_periph, CAN_MODE_NORMAL);
#if DEBUG
		printf("\r\%s now awake.", CAN0 == can_periph ? "CAN0" : "CAN1");
#endif
}


unsigned long char2long(unsigned char *str)
{
    unsigned long __t = str[0];
    __t <<= 8;
    __t |= str[1];
    __t <<= 8;
    __t |= str[2];
    __t <<= 8;
    __t |= str[3];
    return __t;
}
