#include <Wire.h>

#define GD32_I2C_ADDR    			0X41
#define CANCONFIG_SIZE				8 + 14 * 10																						// 14 filters per CAN channel, 
#define CANI2C      				Wire1
#define DELAY_TIME  				0


#define CAN0_MSG_RECV_NUM           0X04																							// CAN0 check number of buffered messages
#define CAN0_SEND_MSG               0X05
#define CAN0_RECV_INFO              0X06																							// id, rtr, ext
#define CAN0_RECV1              	0X07
#define CAN0_RECV2              	0X08
#define CAN0_CONFIG					0X09
#define CAN0_SLEEP              	0X0A
#define CAN0_WAKE			        0X0B


#define CAN1_MSG_RECV_NUM           0X14																							// CAN1 check number of buffered messages
#define CAN1_SEND_MSG               0X15
#define CAN1_RECV_INFO             	0X16
#define CAN1_RECV1             		0X17
#define CAN1_RECV2             		0X18
#define CAN1_CONFIG			 		0X19
#define CAN1_SLEEP              	0X1A
#define CAN1_WAKE		            0X1B


class CANBedDual
{
    private:
    unsigned long timerDelay = DELAY_TIME;
    int canNum;
    unsigned char canconfig[CANCONFIG_SIZE];
    
    unsigned long char2long(unsigned char *str);
    void long2char(unsigned long __t, unsigned char *str);
    void makeCanConfig();
    void sendConfig();
	
	
    public:
    CANBedDual(int num) { 
		canNum = num;
	}
    void initSpeed(unsigned long speed);
    void initMaskFilt(unsigned char num, unsigned long filt, unsigned char ext = 0, unsigned long mask = 0x7ff);
	void begin();
    void sendMsgBuf(unsigned long id, unsigned char len, unsigned char *dta, unsigned char ext = 0, unsigned char rtr = 0);
    byte checkRecv();
    byte readMsgBuf(unsigned long *id, int *len, unsigned char *str, int *ext = NULL, int *rtr = NULL);
	void sleep();
	void wake();
};