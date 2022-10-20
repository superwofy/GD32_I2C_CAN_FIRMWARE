 /*!
    \file    main.c
    \brief   dual CAN communication in normal mode

    \version 2020-12-31, V1.0.0, firmware for GD32C10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include <string.h>
#include <stdio.h>
#include "gd32c10x.h"
#include "gd32c10x_eval.h"
#include "i2c.h"
#include "lgi_basic.h"
#include "dual_dfs.h"

int flgCAN0Get = 0;
int flgCAN1Get = 0;

int NUM_CAN_GET0 = 0;
int NUM_CAN_GET1 = 0;

int index_can_get0 = 0;
int index_can_get1 = 0;

unsigned char CAN_DATA_CAN0[MAX_CAN_RECV][100];
unsigned char CAN_DATA_CAN1[MAX_CAN_RECV][100];

unsigned char can0config[CANCONFIG_SIZE];
unsigned char can1config[CANCONFIG_SIZE];

can_receive_message_struct g_receive_message0;
can_receive_message_struct g_receive_message1;
can_trasnmit_message_struct g_transmit_message;


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


void long2char(unsigned long __t, unsigned char *str)
{
    str[0] = (__t>>24)&0xff;
    str[1] = (__t>>16)&0xff;
    str[2] = (__t>>8)&0xff;
    str[3] = (__t>>0)&0xff;
}


void can0SaveData()
{
    unsigned long id = 0;
	
    index_can_get0++;
    if (index_can_get0 > (MAX_CAN_RECV - 1)) {
			index_can_get0 = 0;
		}

    if (g_receive_message0.rx_ff == CAN_FF_EXTENDED) {
        //CAN_DATA_CAN0[index_can_get0][]
        id = g_receive_message0.rx_efid;
        CAN_DATA_CAN0[index_can_get0][4] = 1;
    }
    else {
        id = g_receive_message0.rx_sfid;
        CAN_DATA_CAN0[index_can_get0][4] = 0;
    }

    long2char(id, &CAN_DATA_CAN0[index_can_get0][0]);

    CAN_DATA_CAN0[index_can_get0][5] = (g_receive_message0.rx_ft == CAN_FT_REMOTE) ? 1 : 0;
    CAN_DATA_CAN0[index_can_get0][6] = g_receive_message0.fd_flag;
    CAN_DATA_CAN0[index_can_get0][7] = g_receive_message0.rx_dlen;

    for (int i = 0; i < CAN_DATA_CAN0[index_can_get0][7]; i++) {
        CAN_DATA_CAN0[index_can_get0][8 + i] = g_receive_message0.rx_data[i];
    }

    NUM_CAN_GET0++;
    if (NUM_CAN_GET0 > MAX_CAN_RECV) {
			NUM_CAN_GET0 = MAX_CAN_RECV;
		}
}


void can1SaveData()
{
    unsigned long id = 0;

    index_can_get1++;
    if (index_can_get1 > (MAX_CAN_RECV - 1)) {
			index_can_get1 = 0;
		}

    if (g_receive_message1.rx_ff == CAN_FF_EXTENDED) {
        //CAN_DATA_CAN0[index_can_get0][]
        id = g_receive_message1.rx_efid;
        CAN_DATA_CAN1[index_can_get1][4] = 1;
    }
    else {
        id = g_receive_message1.rx_sfid;
        CAN_DATA_CAN1[index_can_get1][4] = 0;
    }

    long2char(id, &CAN_DATA_CAN1[index_can_get1][0]);

    CAN_DATA_CAN1[index_can_get1][5] = (g_receive_message1.rx_ft == CAN_FT_REMOTE) ? 1 : 0;
    CAN_DATA_CAN1[index_can_get1][6] = g_receive_message1.fd_flag;
    CAN_DATA_CAN1[index_can_get1][7] = g_receive_message1.rx_dlen;

    for (int i = 0; i < CAN_DATA_CAN1[index_can_get1][7]; i++) {
        CAN_DATA_CAN1[index_can_get1][8 + i] = g_receive_message1.rx_data[i];
    }

    NUM_CAN_GET1++;
    if (NUM_CAN_GET1 > MAX_CAN_RECV) {
			NUM_CAN_GET1 = MAX_CAN_RECV;
		}
}


void can_gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_CAN1);
    rcu_periph_clock_enable(RCU_AF);

		/* configure CAN0 GPIO */
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_11);    																													// CAN0 RX - PA11
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);																														// CAN0 TX - PA12
	
    /* configure CAN1 GPIO */
    gpio_init(GPIOB,GPIO_MODE_IPU, GPIO_OSPEED_50MHZ,GPIO_PIN_12);        																												// CAN1 RX - PB12
    gpio_init(GPIOB,GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_13); 																															// CAN1 TX - PB13
}


void can_config0(unsigned char *str)
{
    unsigned long __mask = 0;
    unsigned long __filt = 0;

    int __ext = 0;
	
    unsigned long __can20baud = char2long(&str[0]);
		can_parameter_struct can_parameter;

    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    /* initialize CAN register */
    can_deinit(CAN0);

    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = ENABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = ENABLE;
    can_parameter.rec_fifo_overwrite = ENABLE;
    can_parameter.trans_fifo_order = ENABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    /* initialize CAN */
    can_init(CAN0, &can_parameter);

    /* config CAN0 baud rate */
    can_frequency_set(CAN0, __can20baud);

#if CANFD_ENABLE
		unsigned long __canfdbaud = char2long(&str[4]);
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
#endif

    /* initialize filter */
    can1_filter_start_bank(14);

    for (int i = 0; i < FILTER_COUNT; i++) {
        if (str[8 + i * 10]) {
            __mask = char2long(&str[i * 10 + 10]);
            __filt = char2long(&str[i * 10 + 14]);
            __ext = str[i * 10 + 9] ? CAN_EXTENDED_FIFO0 : CAN_STANDARD_FIFO0;
            can_filter_mask_mode_init(__filt, __mask, __ext, i);
        }
    }

    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_RX0_IRQn, 0, 0);
    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INTEN_RFNEIE0);
}


void can_config1(unsigned char *str)
{
    unsigned long __mask = 0;
    unsigned long __filt = 0;

    int __ext = 0;
    unsigned long __can20baud = char2long(&str[0]);
		can_parameter_struct can_parameter;

    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    /* initialize CAN register */
    can_deinit(CAN1);

    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = ENABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = ENABLE;
    can_parameter.rec_fifo_overwrite = ENABLE;
    can_parameter.trans_fifo_order = ENABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;

		can_init(CAN1, &can_parameter);
    can_frequency_set(CAN1, __can20baud);

#if CANFD_ENABLE
		unsigned long __canfdbaud = char2long(&str[4]);
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
    can_fd_init(CAN1, &can_fd_parameter);

    can_fd_frequency_set(CAN1, __canfdbaud);
#endif

    /* initialize filter */
    can1_filter_start_bank(14);

    for (int i = 0; i < FILTER_COUNT; i++) {
        if (str[8 + i * 10]) {
            __mask = char2long(&str[i * 10 + 10]);
            __filt = char2long(&str[i * 10 + 14]);
            __ext = str[i * 10 + 9] ? CAN_EXTENDED_FIFO0 : CAN_STANDARD_FIFO0;
            can_filter_mask_mode_init(__filt, __mask, __ext, 15 + i);
        }
    }

    /* configure CAN1 NVIC */
    nvic_irq_enable(CAN1_RX0_IRQn, 1, 0);
    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN1, CAN_INTEN_RFNEIE0);
}


int getI2CDta(unsigned char *dta)
{
    int len = 0;
    unsigned long tout = 0;

    if (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) {
			return len;
		}

    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

#if DEBUG
    while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {
        __NOP();
        tout++;
        if (tout > 5000) {
            //i2c_config();
            //i2c_flag_clear(I2C0, I2C_FLAG_RBNE);
            return 0;
        }
    }
#else
    while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
#endif
    tout = 0;

    while (i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {
        dta[len++] = i2c_data_receive(I2C0);
        for (int i = 0; i < 3000; i++) {
					__NOP();
				}

        if (len > 73) {
            //i2c_flag_clear(I2C0, I2C_FLAG_RBNE);
            return 0;
        }
    }

#if DEBUG
    while (!i2c_flag_get(I2C0, I2C_FLAG_STPDET)) {
        __NOP();
        tout++;
        if (tout > 5000) {
            //i2c_config();
            //i2c_flag_clear(I2C0, I2C_FLAG_STPDET);
            return 0;
        }
    }
#else
    while(!i2c_flag_get(I2C0, I2C_FLAG_STPDET));
#endif
    i2c_enable(I2C0);
    return len;
}


int sendI2CDta(unsigned char *dta, int dlen)
{
    unsigned long tout = 0;
    /* wait until ADDSEND bit is set */
    //if(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))return 0;
    while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) {
        __NOP();
        tout++;
        if (tout > 5000) {
            //i2c_config();
            i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
            return 0;
        }
    }
    tout = 0;
    //if(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))return 0;
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

    /* wait until the transmission data register is empty */
    while (!i2c_flag_get(I2C0, I2C_FLAG_TBE)) {
        __NOP();
        tout++;
        if (tout > 5000) {
            i2c_flag_clear(I2C0, I2C_FLAG_TBE);
            return 0;
        }
    }
    tout = 0;

    for (int i = 0; i < dlen;i++) {
        /* send a data byte */
        i2c_data_transmit(I2C0, dta[i]);
        /* wait until the transmission data register is empty */
        while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)) {
            __NOP();
            tout++;
            if(tout > 5000) {
                i2c_flag_clear(I2C0, I2C_FLAG_TBE);
                return 0;
            }
        }
        tout = 0;
    }
    /* the master doesn't acknowledge for the last byte */
    while (!i2c_flag_get(I2C0, I2C_FLAG_AERR)) {
        __NOP();
        tout++;
        if (tout > 5000) {
            i2c_flag_clear(I2C0, I2C_FLAG_AERR);
            return 0;
        }
    }
    /* clear the bit of AERR */
    i2c_flag_clear(I2C0, I2C_FLAG_AERR);

    return dlen;
}


void I2C_SendData(uint32_t can_periph, unsigned char *str)
{
    unsigned long id = 0;

    id |= str[1];
    id <<= 8;
    id |= str[2];
    id <<= 8;
    id |= str[3];
    id <<= 8;
    id |= str[4];

    /* initialize transmit message */
    g_transmit_message.tx_sfid = id;
    g_transmit_message.tx_efid = id;
    g_transmit_message.tx_ft = str[5] ? CAN_FT_REMOTE : CAN_FT_DATA;
    g_transmit_message.tx_ff = str[6] ? CAN_FF_EXTENDED : CAN_FF_STANDARD;
    g_transmit_message.tx_dlen = str[8];
    g_transmit_message.fd_flag = str[7];
    g_transmit_message.fd_brs = 1;
    g_transmit_message.fd_esi = 0;

    for (int i = 0; i < str[8]; i++) {
        g_transmit_message.tx_data[i] = str[9 + i];
    }

    can_message_transmit(can_periph, &g_transmit_message);
}


int main(void)
{
		/* configure board */
    LGI_Init();
		
#if DEBUG
    Serial_begin();
#endif
	
		/* I2C configure */
		i2c_gpio_config();
    i2c_config();
	
#if DEBUG
		printf("\r\nI2C0 initialized.");
    printf("\r\nThe speed is %d KHz.", I2C_SPEED);
#endif

		/* configure CAN */
    can_gpio_config();

    /* initialize receive message */
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &g_receive_message0);
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &g_receive_message1);

    int i = 0;
    unsigned char i2cDta[CANCONFIG_SIZE], dtaSend[100] = {0};

    while(1) {
        int len = getI2CDta(i2cDta);
        if (len) {
            switch(i2cDta[0]) {
                case REG_ADDR_SEND:
									I2C_SendData(CAN0, i2cDta);
									break;

                case REG1_ADDR_SEND:
									I2C_SendData(CAN1, i2cDta);
									break;

                case REG_ADDR_RECV_NUM:
									dtaSend[0] = NUM_CAN_GET0;
									sendI2CDta(dtaSend, 1);
									break;

                case REG1_ADDR_RECV_NUM:
									dtaSend[0] = NUM_CAN_GET1;
									sendI2CDta(dtaSend, 1);
									break;

                case REG_ADDR_RECV0:
									if (NUM_CAN_GET0) {
											for (i = 0; i < 8; i++) {
													dtaSend[i] = CAN_DATA_CAN0[index_can_get0][i];
											}
											sendI2CDta(dtaSend, 8);
									}
									break;

                case REG_ADDR_RECV1:
									if (NUM_CAN_GET0) {
											if (CAN_DATA_CAN0[index_can_get0][7] <= 32) {
													for (i = 0; i<CAN_DATA_CAN0[index_can_get0][7]; i++) {
															dtaSend[i] = CAN_DATA_CAN0[index_can_get0][8 + i];
													}
													sendI2CDta(dtaSend, CAN_DATA_CAN0[index_can_get0][7]);
													index_can_get0--;
													if(index_can_get0 < 0) {
														index_can_get0 = 7;
													}
													NUM_CAN_GET0--;
											}
											else {
													for (i = 0; i < 32; i++) {
															dtaSend[i] = CAN_DATA_CAN0[index_can_get0][8 + i];
													}
													sendI2CDta(dtaSend, 32);
											}
									}
									break;

                case REG_ADDR_RECV2:
									if (NUM_CAN_GET0 && (CAN_DATA_CAN0[index_can_get0][7] > 32)) {
											for (i = 0; i < (CAN_DATA_CAN0[index_can_get0][7] - 32); i++) {
													dtaSend[i] = CAN_DATA_CAN0[index_can_get0][40 + i];
											}

											sendI2CDta(dtaSend, CAN_DATA_CAN0[index_can_get0][7] - 32);
											index_can_get0--;
											if (index_can_get0 < 0) {
												index_can_get0 = 7;
											}
											NUM_CAN_GET0--;
									}
									break;

                case REG1_ADDR_RECV0:
									if (NUM_CAN_GET1) {
											for(i = 0; i < 8; i++) {
													dtaSend[i] = CAN_DATA_CAN1[index_can_get1][i];
											}
											sendI2CDta(dtaSend, 8);
									}
									break;

                case REG1_ADDR_RECV1:
									if (NUM_CAN_GET1) {
											if (CAN_DATA_CAN1[index_can_get1][7] <= 32) {
													for (i = 0; i<CAN_DATA_CAN1[index_can_get1][7]; i++) {
															dtaSend[i] = CAN_DATA_CAN1[index_can_get1][8 + i];
													}
													sendI2CDta(dtaSend, CAN_DATA_CAN1[index_can_get1][7]);
													index_can_get1--;
													if(index_can_get1 < 0)index_can_get1 = 7;
													NUM_CAN_GET1--;
											}
											else {
													for (i = 0; i < 32; i++) {
															dtaSend[i] = CAN_DATA_CAN1[index_can_get1][8 + i];
													}
													sendI2CDta(dtaSend, 32);
											}
									}
									break;

                case REG1_ADDR_RECV2:
									if (NUM_CAN_GET1 && (CAN_DATA_CAN1[index_can_get1][7] > 32)) {
											for (i = 0; i < (CAN_DATA_CAN1[index_can_get1][7] - 32); i++) {
													dtaSend[i] = CAN_DATA_CAN1[index_can_get1][40 + i];
											}
											sendI2CDta(dtaSend, CAN_DATA_CAN1[index_can_get1][7] - 32);
											index_can_get1--;
											if(index_can_get1 < 0)index_can_get1 = 7;
											NUM_CAN_GET1--;
									}
									break;

                case REG_ADDR_CONFIG:
									memcpy(can0config, &i2cDta[1], CANCONFIG_SIZE);
									can_config0(can0config);
									can_config1(can1config);
									break;

                case REG1_ADDR_CONFIG:
									memcpy(can1config, &i2cDta[1], CANCONFIG_SIZE);
									can_config0(can0config);
									can_config1(can1config);
									break;
								
								case REG_ADDR_SLEEP:
									can_working_mode_set(CAN0, CAN_MODE_SLEEP);
									break;
								
								case REG1_ADDR_SLEEP:
									can_working_mode_set(CAN1, CAN_MODE_SLEEP);
									break;
								
								case REG_ADDR_WAKE:
									can_working_mode_set(CAN0, CAN_MODE_NORMAL);
									break;
								
								case REG1_ADDR_WAKE:
									can_working_mode_set(CAN1, CAN_MODE_NORMAL);
									break;

                default:
									break;
            }
        }

        if (flgCAN0Get) {
            flgCAN0Get = 0;
            can0SaveData();
        }

        if (flgCAN1Get) {
            flgCAN1Get = 0;
            can1SaveData();
        }
    }
}


#if DEBUG
/* retarget the C library printf function to the usart */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
#endif