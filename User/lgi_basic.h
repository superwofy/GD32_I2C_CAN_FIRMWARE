// Longan Labs basic
#ifndef __LGI_BASIC_H__
#define __LGI_BASIC_H__

#include "gd32c10x.h"
#include <stdio.h>
#include "systick.h"
#include "gd32c10x_eval.h"

#define PA0     0
#define PA1     1
#define PA2     2
#define PA3     3
#define PA4     4
#define PA5     5
#define PA6     6
#define PA7     7
#define PA8     8
#define PA9     9
#define PA10    10
#define PA11    11
#define PA12    12
#define PA13    13
#define PA14    14
#define PA15    15

#define PB0     20
#define PB1     21
#define PB2     22
#define PB3     23
#define PB4     24
#define PB5     25
#define PB6     26
#define PB7     27
#define PB8     28
#define PB9     29
#define PB10    30
#define PB11    31
#define PB12    32
#define PB13    33
#define PB14    34
#define PB15    35

#define PC0     40
#define PC1     41
#define PC2     42
#define PC3     43
#define PC4     44
#define PC5     45
#define PC6     46
#define PC7     47
#define PC8     48
#define PC9     49
#define PC10    50
#define PC11    51
#define PC12    52
#define PC13    53
#define PC14    54
#define PC15    55

#ifndef INPUT
#define INPUT   0
#endif

#ifndef OUTPUT
#define OUTPUT  1
#endif

#ifndef INPUT_PULLUP
#define INPUT_PULLUP 2
#endif

// ALL
void LGI_Init(void);

// TIMER
unsigned long millis(void);

// GPIO 

// mode: INPUT, OUTPUT, INPUT_PULLUP
void pinMode(int pin, int mode);
void digitalWrite(int pin, int mode);
int digitalRead(int pin);


// SERIAL
void Serial_begin(unsigned long baud);      // USART 0
int Serial_available(void);
unsigned char Serial_read(void);
void Serial_write(unsigned char c);
void Serial_print(char *s);
void Serial_println(char *s);
//int fputc(int ch, FILE *f);
// I2C


// SPI

// Watch dog

// Analog

//

#endif
// END FILE
