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
#include "gd32c10x.h"
#include <stdio.h>
#include "gd32c10x_eval.h"
#include "lgi_basic.h"
#include "dual_dfs.h"

#define __DBG            0

#define I2C0_OWN_ADDRESS7      0x82
int st = 0;
int __cnt = 0;
uint8_t i2c_receiver[100];

int flgCAN0Get = 0;
int flgCAN1Get = 0;

#define MAX_CAN_RECV        8

int NUM_CAN_GET0 = 0;
int NUM_CAN_GET1 = 0;

int index_can_get0 = 0;
int index_can_get1 = 0;

unsigned char CAN_DATA_CAN0[MAX_CAN_RECV][100];
unsigned char CAN_DATA_CAN1[MAX_CAN_RECV][100];

unsigned char MASK_FILT0[4][10];
unsigned char MASK_FILT1[4][10];

unsigned char can0config[48];
unsigned char can1config[48];

void mask_filt_data_init()
{
    int i, j;

    for(i=0; i<10; i++)
    {
        for(j=0; j<10; j++)
        {
            MASK_FILT0[i][j] = 0;
            MASK_FILT1[i][j] = 0;
        }
    }

    MASK_FILT0[0][0] = 1;
    MASK_FILT1[0][0] = 1;
}

can_trasnmit_message_struct g_transmit_message;

can_receive_message_struct g_receive_message0;
can_receive_message_struct g_receive_message1;


unsigned long char2long(unsigned char *str)
{
    unsigned long __t = 0;

    __t = str[0];
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
    int i = 0;

    index_can_get0++;
    if(index_can_get0 > (MAX_CAN_RECV-1))index_can_get0 = 0;

    if(g_receive_message0.rx_ff == CAN_FF_EXTENDED)
    {
        //CAN_DATA_CAN0[index_can_get0][]
        id = g_receive_message0.rx_efid;
        CAN_DATA_CAN0[index_can_get0][4] = 1;
    }
    else
    {
        id = g_receive_message0.rx_sfid;
        CAN_DATA_CAN0[index_can_get0][4] = 0;
    }

    long2char(id, &CAN_DATA_CAN0[index_can_get0][0]);

    CAN_DATA_CAN0[index_can_get0][5] = (g_receive_message0.rx_ft == CAN_FT_REMOTE) ? 1 : 0;
    CAN_DATA_CAN0[index_can_get0][6] = g_receive_message0.fd_flag;
    CAN_DATA_CAN0[index_can_get0][7] = g_receive_message0.rx_dlen;

    for(i=0; i<CAN_DATA_CAN0[index_can_get0][7]; i++)
    {
        CAN_DATA_CAN0[index_can_get0][8+i] = g_receive_message0.rx_data[i];
    }

    NUM_CAN_GET0++;
    if(NUM_CAN_GET0 > MAX_CAN_RECV)NUM_CAN_GET0 = MAX_CAN_RECV;
}



void can1SaveData()
{
    unsigned long id = 0;
    int i = 0;

    index_can_get1++;
    if(index_can_get1 > (MAX_CAN_RECV-1))index_can_get1 = 0;

    if(g_receive_message1.rx_ff == CAN_FF_EXTENDED)
    {
        //CAN_DATA_CAN0[index_can_get0][]
        id = g_receive_message1.rx_efid;
        CAN_DATA_CAN1[index_can_get1][4] = 1;
    }
    else
    {
        id = g_receive_message1.rx_sfid;
        CAN_DATA_CAN1[index_can_get1][4] = 0;
    }

    long2char(id, &CAN_DATA_CAN1[index_can_get1][0]);

    CAN_DATA_CAN1[index_can_get1][5] = (g_receive_message1.rx_ft == CAN_FT_REMOTE) ? 1 : 0;
    CAN_DATA_CAN1[index_can_get1][6] = g_receive_message1.fd_flag;
    CAN_DATA_CAN1[index_can_get1][7] = g_receive_message1.rx_dlen;

    for(i=0; i<CAN_DATA_CAN1[index_can_get1][7]; i++)
    {
        CAN_DATA_CAN1[index_can_get1][8+i] = g_receive_message1.rx_data[i];
    }

    NUM_CAN_GET1++;
    if(NUM_CAN_GET1 > MAX_CAN_RECV)NUM_CAN_GET1 = MAX_CAN_RECV;

}

/*!
    \brief      configure the I2C0 interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    //i2c_deinit(I2C0);
    rcu_periph_clock_enable(RCU_I2C0);
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);        // i2c gpio config, PB7/6 - SDA/SCL
    /* I2C clock configure */
    i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
    /* I2C address configure */
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_CAN1);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_AF);

    /* configure CAN0 GPIO */
    gpio_init(GPIOB,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_12);        // CAN1 RX
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_13); // CAN1 TX

    /* configure CAN1 GPIO */
    gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_11);    // CAN0 RX
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);// CAN1 TX
}


/*!
    \brief      initialize CAN function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_config0(unsigned char *str)
{
    int i=0;
    unsigned long __mask = 0;
    unsigned long __filt = 0;

    int __ext = 0;

    can_parameter_struct can_parameter;
    can_fdframe_struct can_fd_parameter;
    can_fd_tdc_struct can_fd_tdc_parameter;

    unsigned long __can20baud = char2long(&str[0]);
    unsigned long __canfdbaud = char2long(&str[4]);;

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
    //can_fd_init(CAN1, &can_fd_parameter);

    can_fd_frequency_set(CAN0, __canfdbaud);

    /* initialize filter */
    can1_filter_start_bank(14);

    for(i=0; i<4; i++)
    {
        if(str[8+i*10])
        {
            __mask = char2long(&str[i*10+10]);
            __filt = char2long(&str[i*10+14]);
            __ext = str[i*10+9] ? CAN_EXTENDED_FIFO0 : CAN_STANDARD_FIFO0;
            can_filter_mask_mode_init(__filt, __mask, __ext, i);
        }
    }

    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_RX0_IRQn, 0, 0);
    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INTEN_RFNEIE0);
}

/*!
    \brief      initialize CAN function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_config1(unsigned char *str)
{
    int i=0;
    unsigned long __mask = 0;
    unsigned long __filt = 0;


    int __ext = 0;
    can_parameter_struct can_parameter;
    can_fdframe_struct can_fd_parameter;
    can_fd_tdc_struct can_fd_tdc_parameter;

    unsigned long __can20baud = char2long(&str[0]);
    unsigned long __canfdbaud = char2long(&str[4]);;

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
    /* initialize CAN */
    can_init(CAN1, &can_parameter);

    /* config CAN1 baud rate */
    can_frequency_set(CAN1, __can20baud);

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

    /* initialize filter */
    can1_filter_start_bank(14);

    for(i=0; i<4; i++)
    {
        if(str[8+i*10])
        {
            __mask = char2long(&str[i*10+10]);
            __filt = char2long(&str[i*10+14]);
            __ext = str[i*10+9] ? CAN_EXTENDED_FIFO0 : CAN_STANDARD_FIFO0;
            can_filter_mask_mode_init(__filt, __mask, __ext, 15+i);
        }
    }

    /* configure CAN1 NVIC */
    nvic_irq_enable(CAN1_RX0_IRQn, 1, 0);
    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN1, CAN_INTEN_RFNEIE0);
}


void canInit()
{

    int i=0;

    unsigned long can20_baud = 500000;
    unsigned long canfd_baud = 1000000;

    unsigned char mf_set0   = 1;
    unsigned char mf_ext0   = 0;
    unsigned long mf_mask0  = 0;
    unsigned long mf_filt0  = 0;

    unsigned char mf_set1   = 1;
    unsigned char mf_ext1   = 1;
    unsigned long mf_mask1  = 0;
    unsigned long mf_filt1  = 0;

    unsigned char mf_set2   = 0;
    unsigned char mf_ext2   = 0;
    unsigned long mf_mask2  = 0;
    unsigned long mf_filt2  = 0;

    unsigned char mf_set3   = 0;
    unsigned char mf_ext3   = 0;
    unsigned long mf_mask3  = 0;
    unsigned long mf_filt3  = 0;


    for(i=0; i<48; i++)
    {
        can0config[i] = 0;
        can1config[i] = 0;
    }

    long2char(can20_baud, &can0config[0]);
    long2char(canfd_baud, &can0config[4]);
    long2char(can20_baud, &can1config[0]);
    long2char(canfd_baud, &can1config[4]);

    // set mask&filt 0
    can0config[8] = mf_set0;
    can0config[9] = mf_ext0;
    long2char(mf_mask0, &can0config[10]);
    long2char(mf_filt0, &can0config[14]);

    can1config[8] = mf_set0;
    can1config[9] = mf_ext0;
    long2char(mf_mask0, &can1config[10]);
    long2char(mf_filt0, &can1config[14]);

    // set mask&filt 1
    can0config[18] = mf_set1;
    can0config[19] = mf_ext1;
    long2char(mf_mask1, &can0config[20]);
    long2char(mf_filt1, &can0config[24]);

    can1config[18] = mf_set1;
    can1config[19] = mf_ext1;
    long2char(mf_mask1, &can1config[20]);
    long2char(mf_filt1, &can1config[24]);

    // set mask&filt 2
    can0config[28] = mf_set2;
    can0config[29] = mf_ext2;
    long2char(mf_mask2, &can0config[30]);
    long2char(mf_filt2, &can0config[34]);

    can1config[28] = mf_set2;
    can1config[29] = mf_ext2;
    long2char(mf_mask2, &can1config[30]);
    long2char(mf_filt2, &can1config[34]);

    // set mask&filt 3
    can0config[38] = mf_set3;
    can0config[39] = mf_ext3;
    long2char(mf_mask3, &can0config[40]);
    long2char(mf_filt3, &can0config[44]);

    can1config[38] = mf_set3;
    can1config[39] = mf_ext3;
    long2char(mf_mask3, &can1config[40]);
    long2char(mf_filt3, &can1config[44]);



    can_config0(can0config);
    can_config1(can1config);
}

#define DBUG  1

int getI2CDta(unsigned char *dta)
{
    int len = 0;
    int i=0;

    unsigned long tout = 0;

    if(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))return len;

    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

#if DBUG
    while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE))
    {
        __NOP;
        tout++;
        if(tout > 5000)
        {
            //i2c_config();
            //i2c_flag_clear(I2C0, I2C_FLAG_RBNE);
            return 0;
        }
    }
#else
    while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
#endif
    tout = 0;

    while(i2c_flag_get(I2C0, I2C_FLAG_RBNE))
    {
        dta[len++] = i2c_data_receive(I2C0);
        for(i=0; i<3000; i++)
        __NOP;

        if(len > 73)
        {
            //i2c_flag_clear(I2C0, I2C_FLAG_RBNE);
            return 0;
        }
    }

#if DBUG
    while(!i2c_flag_get(I2C0, I2C_FLAG_STPDET))
    {
        __NOP;
        tout++;
        if(tout > 5000)
        {
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
int cnt = 0;


int sendI2CDta(unsigned char *dta, int dlen)
{

    int i=0;
    unsigned long tout = 0;
    /* wait until ADDSEND bit is set */
    //if(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))return 0;
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
    {
        __NOP;
        tout++;
        if(tout > 5000)
        {
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
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE))
    {
        __NOP;
        tout++;
        if(tout > 5000)
        {
            i2c_flag_clear(I2C0, I2C_FLAG_TBE);
            return 0;
        }
    }
    tout = 0;

    for(i=0;i<dlen;i++){
        /* send a data byte */
        i2c_data_transmit(I2C0, dta[i]);
        /* wait until the transmission data register is empty */
        while(!i2c_flag_get(I2C0, I2C_FLAG_TBE))
        {
            __NOP;
            tout++;
            if(tout > 5000)
            {
                i2c_flag_clear(I2C0, I2C_FLAG_TBE);
                return 0;
            }
        }
        tout = 0;
    }
    /* the master doesn't acknowledge for the last byte */
    while(!i2c_flag_get(I2C0, I2C_FLAG_AERR))
    {
        __NOP;
        tout++;
        if(tout > 5000)
        {
            i2c_flag_clear(I2C0, I2C_FLAG_AERR);
            return 0;
        }
    }
    /* clear the bit of AERR */
    i2c_flag_clear(I2C0, I2C_FLAG_AERR);

    return dlen;
}

void I2C_SendData(uint32_t can_periph, unsigned char *str);                // can0 send

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
unsigned char dtaSend[100];

int main(void)
{

    /* configure board */
    int i=0;
    unsigned char i2cDta[100];

    for(i=0; i<100; i++)dtaSend[i] = i;

    mask_filt_data_init();

    LGI_Init();
    Serial_begin(115200);

        /* configure GPIO */
    can_gpio_config();
    canInit();

    /* I2C configure */
    i2c_config();

    /* initialize transmit message */
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &g_transmit_message);
    g_transmit_message.tx_sfid = 0x55;
    g_transmit_message.tx_efid = 0x55;
    g_transmit_message.tx_ft = CAN_FT_DATA;
    g_transmit_message.tx_ff = CAN_FF_STANDARD;
    g_transmit_message.tx_dlen = 8;
    g_transmit_message.fd_flag = 1;
    g_transmit_message.fd_brs = 1;
    g_transmit_message.fd_esi = 0;
    /* initialize receive message */
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &g_receive_message0);
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &g_receive_message1);

    while(1)
    {
        int len;

        len = getI2CDta(i2cDta);
        if(len)
        {

            switch(i2cDta[0])
            {
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


                if(NUM_CAN_GET0)
                {
                    for(i=0; i<8; i++)
                    {
                        dtaSend[i] = CAN_DATA_CAN0[index_can_get0][i];

                    }
                    sendI2CDta(dtaSend, 8);
                }
                break;

                case REG_ADDR_RECV1:
                if(NUM_CAN_GET0)
                {
                    if(CAN_DATA_CAN0[index_can_get0][7] <= 32)
                    {
                        for(i=0; i<CAN_DATA_CAN0[index_can_get0][7]; i++)
                        {
                            dtaSend[i] = CAN_DATA_CAN0[index_can_get0][8+i];
                        }
                        sendI2CDta(dtaSend, CAN_DATA_CAN0[index_can_get0][7]);
                        index_can_get0--;
                        if(index_can_get0 < 0)index_can_get0 = 7;
                        NUM_CAN_GET0--;
                    }
                    else
                    {
                        for(i=0; i<32; i++)
                        {
                            dtaSend[i] = CAN_DATA_CAN0[index_can_get0][8+i];
                        }
                        sendI2CDta(dtaSend, 32);
                    }
                }

                break;

                case REG_ADDR_RECV2:

                if(NUM_CAN_GET0 && (CAN_DATA_CAN0[index_can_get0][7]>32))
                {
                    for(i=0; i<(CAN_DATA_CAN0[index_can_get0][7]-32); i++)
                    {
                        dtaSend[i] = CAN_DATA_CAN0[index_can_get0][40+i];
                    }

                    sendI2CDta(dtaSend, CAN_DATA_CAN0[index_can_get0][7]-32);
                    index_can_get0--;
                    if(index_can_get0 < 0)index_can_get0 = 7;
                    NUM_CAN_GET0--;
                }

                break;

                case REG1_ADDR_RECV0:

                if(NUM_CAN_GET1)
                {
                    for(i=0; i<8; i++)
                    {
                        dtaSend[i] = CAN_DATA_CAN1[index_can_get1][i];
                    }
                    sendI2CDta(dtaSend, 8);
                }
                break;

                case REG1_ADDR_RECV1:
                if(NUM_CAN_GET1)
                {
                    if(CAN_DATA_CAN1[index_can_get1][7] <= 32)
                    {
                        for(i=0; i<CAN_DATA_CAN1[index_can_get1][7]; i++)
                        {
                            dtaSend[i] = CAN_DATA_CAN1[index_can_get1][8+i];
                        }
                        sendI2CDta(dtaSend, CAN_DATA_CAN1[index_can_get1][7]);
                        index_can_get1--;
                        if(index_can_get1 < 0)index_can_get1 = 7;
                        NUM_CAN_GET1--;
                    }
                    else
                    {
                        for(i=0; i<32; i++)
                        {
                            dtaSend[i] = CAN_DATA_CAN1[index_can_get1][8+i];
                        }
                        sendI2CDta(dtaSend, 32);
                    }
                }

                break;

                case REG1_ADDR_RECV2:

                if(NUM_CAN_GET1 && (CAN_DATA_CAN1[index_can_get1][7]>32))
                {
                    for(i=0; i<(CAN_DATA_CAN1[index_can_get1][7]-32); i++)
                    {
                        dtaSend[i] = CAN_DATA_CAN1[index_can_get1][40+i];
                    }

                    sendI2CDta(dtaSend, CAN_DATA_CAN1[index_can_get1][7]-32);
                    index_can_get1--;
                    if(index_can_get1 < 0)index_can_get1 = 7;
                    NUM_CAN_GET1--;
                }

                break;

                case REG_ADDR_CONFIG:

                memcpy(can0config, &i2cDta[1], 48);

                can_config0(can0config);
                can_config1(can1config);

                break;


                case REG1_ADDR_CONFIG:
                memcpy(can1config, &i2cDta[1], 48);

                can_config0(can0config);
                can_config1(can1config);

                break;

                default:;

            }
        }

        if(flgCAN0Get)
        {
            flgCAN0Get = 0;
            can0SaveData();
        }

        if(flgCAN1Get)
        {
            flgCAN1Get = 0;
            can1SaveData();
        }
    }
}


void I2C_SendData(uint32_t can_periph, unsigned char *str)
{
    unsigned long id = 0;
    int i=0;

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

    for(i=0; i<str[8]; i++)
    {
        g_transmit_message.tx_data[i] = str[9+i];
    }

    can_message_transmit(can_periph, &g_transmit_message);
}


/* retarget the C library printf function to the usart */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}

// END FILE
