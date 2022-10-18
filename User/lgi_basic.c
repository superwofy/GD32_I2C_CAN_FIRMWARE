#include "lgi_basic.h"


uint8_t rxbuffer[64];
__IO uint16_t rxcount = 0; 

unsigned long cnt_millis = 0;

void LGI_Init(void)
{
    systick_config();                   																																													// Initialization always, external 8M
    rcu_periph_clock_enable(RCU_GPIOA);																																														// RCU = Reset Control Unit. Activate GPIO clocks.
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
}

// void timer

unsigned long millis(void)
{
    return cnt_millis;
}


// ----------------------------------------------------------
// --------------------------DIGITAL IO----------------------
// ----------------------------------------------------------
void pinMode(int pin, int mode)
{
    uint8_t ioMode;
    
    if(mode == INPUT)ioMode = GPIO_MODE_IN_FLOATING;
    else if(mode == INPUT_PULLUP)ioMode = GPIO_MODE_IPU;
    else if(mode == OUTPUT)ioMode = GPIO_MODE_OUT_PP;
    
    printf("iomode = %d\r\n", ioMode);
    printf("pinum  = %d\r\n", pin);
    if(pin < 20)        																																																					// GPIOA
    {
        gpio_init(GPIOA, ioMode, GPIO_OSPEED_50MHZ, BIT(pin%20));
    }
    else if(pin < 40)
    {
        gpio_init(GPIOB, ioMode, GPIO_OSPEED_50MHZ, BIT(pin%20));
    }
    else if(pin < 60)
    {
        gpio_init(GPIOC, ioMode, GPIO_OSPEED_50MHZ, BIT(pin%20));
    }
}

void digitalWrite(int pin, int mode)
{
    if(pin < 20)        																																																					// GPIOA
    {
        if(mode)gpio_bit_set(GPIOA, BIT(pin%20));
        else gpio_bit_reset(GPIOA,BIT(pin%20));
    }
    else if(pin < 40)
    {
        if(mode)gpio_bit_set(GPIOB, BIT(pin%20));
        else gpio_bit_reset(GPIOB, BIT(pin%20));
    }
    else if(pin < 60)
    {
        if(mode)gpio_bit_set(GPIOC, BIT(pin%20));
        else gpio_bit_reset(GPIOC, BIT(pin%20));
    }
}



int digitalRead(int pin)
{

    if(pin < 20)        																																																					// GPIOA
    {
        return gpio_input_bit_get(GPIOA, BIT(pin%20));
    }
    else if(pin < 40)
    {
        return gpio_input_bit_get(GPIOB, BIT(pin%20));
    }
    else if(pin < 60)
    {
        return gpio_input_bit_get(GPIOC, BIT(pin%20));
    }
    
    return 0;
}



// ----------------------------------------------------------
// --------------------------SERIAL--------------------------
// ----------------------------------------------------------
void Serial_begin(unsigned long baud)      																																												// USART 0
{
    nvic_irq_enable(USART0_IRQn, 2, 2);
    gd_eval_com_init(EVAL_COM0, baud);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
}

int Serial_available(void)
{
    return rxcount;
}

unsigned char Serial_read(void)
{
    if(rxcount)
    {
        rxcount--;
		return rxbuffer[rxcount];
    }
	return 0;
}

void Serial_write(unsigned char c)
{
    printf("%c", c);
}

void Serial_print(char *s)
{
    printf("%s", s);
}

void Serial_println(char *s)
{
    printf("%s\r\n", s);
}

/* retarget the C library printf function to the USART */
/*int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
*/
