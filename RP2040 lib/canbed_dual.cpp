#include "canbed_dual.h"

unsigned long CANBedDual::char2long(unsigned char *str)
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

void CANBedDual::long2char(unsigned long __t, unsigned char *str)
{
    str[0] = (__t>>24) & 0xff;
    str[1] = (__t>>16) & 0xff;
    str[2] = (__t>>8) & 0xff;
    str[3] = (__t>>0) & 0xff;
}

void CANBedDual::makeCanConfig()
{       
    // Initialize masks and filters 0 - 14
	for (int x = 0; x < 14; x++) {
		canconfig[x * 10 + 8] = 0;
		canconfig[x * 10 + 9] = 0;
		long2char(0, &canconfig[x * 10 + 10]);
		long2char(0, &canconfig[x * 10 + 14]);
	}
	
	// set filt 0 always enabled
    canconfig[8] = 1;
}

void CANBedDual::sendConfig()
{
    CANI2C.beginTransmission(GD32_I2C_ADDR);
    CANI2C.write(canNum ? CAN1_CONFIG : CAN0_CONFIG);
    CANI2C.write(canconfig, CANCONFIG_SIZE);
    CANI2C.endTransmission();
    delay(10);
}

void CANBedDual::initSpeed(unsigned long speed)
{
    makeCanConfig();
    long2char(speed, &canconfig[0]);
}
    
void CANBedDual::initMaskFilt(unsigned char num, unsigned long filt, unsigned char ext, unsigned long mask)
{
    if(num > 13) {
		return;
	}
    canconfig[10 * num + 8] = 1;																									// Filter enabled 0 / 1.
    canconfig[10 * num + 9] = ext;
    long2char(mask, &canconfig[10 + 10 * num]);
    long2char(filt, &canconfig[14 + 10 * num]);
}

void CANBedDual::begin()
{
	sendConfig();
}
    
void CANBedDual::sendMsgBuf(unsigned long id, unsigned char len, unsigned char *dta, unsigned char ext, unsigned char rtr)
{
/*  if(micros()-timerDelay < DELAY_TIME)
    {
        delayMicroseconds(DELAY_TIME-(micros()-timerDelay));
    }
    timerDelay = micros();   
    */
    unsigned char dtaSendToGD32[100];

    dtaSendToGD32[0] = canNum ? CAN1_SEND_MSG : CAN0_SEND_MSG;
    dtaSendToGD32[4] = id & 0xff;
    dtaSendToGD32[3] = (id >> 8) & 0xff;
    dtaSendToGD32[2] = (id >> 16) & 0xff;
    dtaSendToGD32[1] = (id >> 24) & 0xff;

    dtaSendToGD32[5] = rtr;       // data or remote, 1:remote, 0:data
    dtaSendToGD32[6] = ext;       // standard or ext, 1 ext, 0 standard

    dtaSendToGD32[8] = len;

    for (uint8_t i = 0; i < len; i++) {
        dtaSendToGD32[i + 9] = dta[i];
    }

    CANI2C.beginTransmission(GD32_I2C_ADDR);
    CANI2C.write(dtaSendToGD32, len + 9);
    CANI2C.endTransmission();
}

byte CANBedDual::checkRecv()
{

    CANI2C.beginTransmission(GD32_I2C_ADDR);
    CANI2C.write(canNum ? CAN1_MSG_RECV_NUM : CAN0_MSG_RECV_NUM);
    CANI2C.endTransmission();

    CANI2C.requestFrom(GD32_I2C_ADDR, 1);
    int len = CANI2C.read();

    return len;
}

byte CANBedDual::readMsgBuf(unsigned long *id, int *len, unsigned char *str, int *ext, int *rtr)
{
 /*   if(micros()-timerDelay < DELAY_TIME)
    {
        delayMicroseconds(DELAY_TIME-(micros()-timerDelay));
    }
    timerDelay = micros();
    */
    if (!checkRecv()) {
		return 0;
	}

    CANI2C.beginTransmission(GD32_I2C_ADDR);
    CANI2C.write(canNum ? CAN1_RECV_INFO : CAN0_RECV_INFO);
    CANI2C.endTransmission();

    CANI2C.requestFrom(GD32_I2C_ADDR, 8);

    unsigned char strg[8];
	unsigned long _id = 0;

    for (uint8_t i = 0; i < 8; i++) {
        strg[i] = CANI2C.read();
    }

    _id = char2long(&strg[0]);
    
    *id = _id;
	*rtr = strg[5];
    *ext = strg[4];

    int dtaLen = strg[7];
    *len = dtaLen;

    if (dtaLen <= 32) {
        CANI2C.beginTransmission(GD32_I2C_ADDR);
        CANI2C.write(canNum ? CAN1_RECV1 : CAN0_RECV1);
        CANI2C.endTransmission();

        CANI2C.requestFrom(GD32_I2C_ADDR, 8);

        for (uint8_t i = 0; i < 8; i++) {
            str[i]= CANI2C.read();
        }
    }
    else {
        CANI2C.beginTransmission(GD32_I2C_ADDR);
        CANI2C.write(canNum ? CAN1_RECV1 : CAN0_RECV1);
        CANI2C.endTransmission();

        CANI2C.requestFrom(GD32_I2C_ADDR, 32);

        for (uint8_t i = 0; i < 32; i++) {
            str[i]= CANI2C.read();
        }

        CANI2C.beginTransmission(GD32_I2C_ADDR);
        CANI2C.write(canNum ? CAN1_RECV2 : CAN0_RECV2);
        CANI2C.endTransmission();

        CANI2C.requestFrom(GD32_I2C_ADDR, dtaLen - 32);

        for (uint8_t i = 0; i < (dtaLen - 32); i++) {
            str[32 + i]= CANI2C.read();
        }
    }

    return dtaLen;
}  
    
void CANBedDual::sleep()
{
    CANI2C.beginTransmission(GD32_I2C_ADDR);
	CANI2C.write(canNum ? CAN1_SLEEP : CAN0_SLEEP);
    CANI2C.endTransmission();
}

void CANBedDual::wake()
{
    CANI2C.beginTransmission(GD32_I2C_ADDR);
	CANI2C.write(canNum ? CAN1_WAKE : CAN0_WAKE);
    CANI2C.endTransmission();
}
