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

/*!
    \brief      configure the CAN0/1 speed and parameters
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_param_config(uint32_t can_periph, unsigned char *conf_str, int fd_disable) 
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

    /* config can_periph baud rate */
    can_frequency_set(can_periph, __can20baud);

		if (!fd_disable) {
				unsigned long __canfdbaud = char2long(&conf_str[4]);
				can_fdframe_struct can_fd_parameter;
				can_fd_tdc_struct can_fd_tdc_parameter;

				can_struct_para_init(CAN_FD_FRAME_STRUCT, &can_fd_parameter);
				can_fd_parameter.fd_frame = ENABLE;
				can_fd_parameter.excp_event_detect = ENABLE;
				can_fd_parameter.delay_compensation = ENABLE;
				can_fd_tdc_parameter.tdc_filter = 0x04;
				can_fd_tdc_parameter.tdc_mode = CAN_TDCMOD_CALC_AND_OFFSET;
				can_fd_tdc_parameter.tdc_offset = 0x04;
				can_fd_parameter.p_delay_compensation = &can_fd_tdc_parameter;
				can_fd_parameter.iso_bosch = CAN_FDMOD_ISO;
				can_fd_parameter.esi_mode = CAN_ESIMOD_HARDWARE;
				
				can_fd_init(CAN0, &can_fd_parameter);
				can_fd_frequency_set(CAN0, __canfdbaud);
		}

    /* Set CAN1 filter offset start */
    can1_filter_start_bank(14);
		
		
		for (int i = 0; i < 14; i++) {
				if (conf_str[8 + i * 10]) {
						unsigned long __mask = char2long(&conf_str[i * 10 + 10]);
						unsigned long __filt = char2long(&conf_str[i * 10 + 14]);
						int __ext = conf_str[i * 10 + 9] ? CAN_EXTENDED_FIFO0 : CAN_STANDARD_FIFO0;
					
						if (CAN0 == can_periph) {
								can_filter_mask_mode_init(__filt, __mask, __ext, i);
						}
						else {
								can_filter_mask_mode_init(__filt, __mask, __ext, i + 14);
						}
				}
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