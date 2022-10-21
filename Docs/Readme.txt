# Working description


## Initialization

The system timer is initialized first with systick_config().
I/O in the form of GPIO ports is initialized next by LGI_Init(). Clocks for ports A,B,C and D are enabled.
If DEBUG is enabled, UART is initialized next.
I2C clock and GPIO is enabled followed by speed and addressing mode. I2C is configured in slave mode with address 0x41.
CAN clock and GPIO is enabled followed by initialization of the message reception structures.

-> The MCU is now ready and loops awaiting I2C commands.


## Main loop

If an I2C command is received, a switch statement is called on the first byte (0 in program) of the received buffer.
	CAN*_SEND_MSG: The received buffer is forwarded to CAN* and sent to the bus immediately in function CANX_Send_From_I2C().
		
	CAN*_MSG_RECV_NUM: When this command is received the number of stored CAN messages for the respective CAN peripheral is sent via I2C.
					   This should be done before requesting the message contents as the buffer could be empty.
					   
	CAN*_RECV_INFO: TODO
	REG*_ADDR_RECV1: TODO
	REG*_ADDR_RECV2: TODO
	
	
	
	CAN*_CONFIG:  The received buffer is copied into memory and then decoded by can_param_config(). 
				  The speed, wakeup, fifo and working mode paraeters are set. Masks and filters are initialized according to the configuration sent on I2C. Masks and filters are not initialized unless explicitly defined in the configuration.
				  By default CAN1 filters begin from the 15th filter (14 in program). I.e 1-14 inclusive for CAN0 and 15-28 inclusive for CAN1.
				  CAN-FD is disabled by default.
	
	CAN*_SLEEP: When this command is received, the working mode of the respective CAN peripheral is set to CAN_MODE_SLEEP.
	
	CAN*_WAKE: When this command is received, the working mode of the respective CAN peripheral is set to CAN_MODE_NORMAL.
				 

## CAN message reception

During CAN configuration interrupts are created for when the CAN* FIFO buffer is not empty. In the associated ISRs, a reception flag is set.
Flags are checked in the main loop. If the reception flag is set, canSaveData(CAN*) adds the message id, ext, rtr and data to the next position in the buffer.
Two respective counters are used to keep track of the buffer position. Once the buffer is full, the oldest message (0 in the program) is overwritten.
	
				 

## Arduino library

CAN0_CONFIG/CAN1_CONFIG should only be called when the configuration is fully created. I.e. speed is defined, masks and filters are set/zeroed. Calling the config command multiple times may result in unexpected behavior.