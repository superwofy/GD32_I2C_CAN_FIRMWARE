#ifndef __DUAL_DFS_H__
#define __DUAL_DFS_H__


#define FILTER_COUNT					16
#define CANCONFIG_SIZE				8 + FILTER_COUNT * 10
#define DEBUG						1
#define CANFD_ENABLE		1
#define I2C0_OWN_ADDRESS7			0x41																	// Match with CANBED library
#define MAX_CAN_RECV				8


#define REG_ADDR_20_BAUD            0X02
#define REG_ADDR_FD_BAUD            0X03
#define REG_ADDR_RECV_NUM           0X04
#define REG_ADDR_SEND               0X05
#define REG_ADDR_RECV0              0X06
#define REG_ADDR_RECV1              0X07
#define REG_ADDR_RECV2              0X08
#define REG_ADDR_CONFIG				0X09


#define REG1_ADDR_20_BAUD            0X12
#define REG1_ADDR_FD_BAUD            0X13
#define REG1_ADDR_RECV_NUM           0X14
#define REG1_ADDR_SEND               0X15
#define REG1_ADDR_RECV0              0X16
#define REG1_ADDR_RECV1              0X17
#define REG1_ADDR_RECV2              0X18
#define REG1_ADDR_CONFIG			 0X19

#endif
