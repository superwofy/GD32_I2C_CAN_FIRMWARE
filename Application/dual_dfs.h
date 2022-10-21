#ifndef __DUAL_DFS_H__
#define __DUAL_DFS_H__


#define CANCONFIG_SIZE							8 + 14 * 10																																										// 14 filters per peripheral. I.e 14 for CAN0, 14 for CAN1.
#define UNDERCLOCK									1																																															// Sets MCU speed to 48MHz to save power.
#define DEBUG												1
#define MAX_CAN_RECV								8


#define CAN0_MSG_RECV_NUM           0X04																																													// CAN0 functions
#define CAN0_SEND_MSG               0X05
#define CAN0_RECV_INFO              0X06																																													// id, rtr, ext, fd
#define REG_ADDR_RECV1              0X07
#define REG_ADDR_RECV2              0X08
#define CAN0_DISABLE_FD							0X09
#define CAN0_CONFIG									0X0A
#define CAN0_SLEEP              		0X0B
#define CAN0_WAKE			          		0X0C


#define CAN1_MSG_RECV_NUM           0X14																																													// CAN1 functions
#define CAN1_SEND_MSG               0X15
#define CAN1_RECV_INFO              0X16
#define REG1_ADDR_RECV1             0X17
#define REG1_ADDR_RECV2             0X18
#define CAN1_DISABLE_FD							0X19
#define CAN1_CONFIG			 						0X1A
#define CAN1_SLEEP             			0X1B
#define CAN1_WAKE		          			0X1C

#endif
