/*****************************************************************************
 *
 **  Setup:
 **
 ***         Serial channel              : SPI2
 **
 **         Protocol
 **             Init baud rate          : 375kHz (low speed)
 **             Clock edge              : falling
 **             Width                   : 8 bits
 **             Empty character         : 0
 **             Empty char. on input    : RECEIVED
 **
 **         Registers
 **             Input buffer            : SPI2DL    [$0075]
 **             Output buffer           : SPI2DL    [$0075]
 **             Control register        : SPI2C1    [$0070]
 **             Mode register           : SPI2C2    [$0071]
 **             Baud setting reg.       : SPI2BR    [$0072]
 **
 **
 **
 **         Used pins                   :
 **         ----------------------------------------------------------
 **              Function    | On package |    Name
 **         ----------------------------------------------------------
 **               Input      |     23     |  PTB0_MISO2_ADP0
 **               Output     |     24     |  PTB1_MOSI2_ADP1
 **               Clock      |     25     |  PTB2_SPSCK2_ADP2
 **         ----------------------------------------------------------

 * 
 ******************************************************************************/

#include "SPI.h"

T32_8 sdArgument;
static UINT8 sdResponse[5];
static UINT8 gu8SD_CSD[16];
extern UINT32 dataSector;
const uint8_t SD_POWER_UP_DELAY = 1; // Delay required after SD power up in ms.
const uint8_t SD_START_BLOCK = 0;
const uint32_t SD_END_BLOCK = 396000;

/**
 * Initialise the SPI port for use with the on-board SD Card reader.
 */
void initialiseSDSerial(void) {
	// Initialise SD Card Chip Select - Connected to PTB3/SS2
	SPI_SS = DISABLE; // SD Card disabled (CS high)
	_SPI_SS = _OUT; // PTB3 as an output

	// Set up SPI2 for > 6MHz operation
	SOPT2_SPI2FE = 0; // Disable SPI2 input filters
	PTBDS_PTBDS1 = 1; // High output drive strength for PTB1/MOSO2/
	PTBDS_PTBDS2 = 1; // PTB2/SPSCK2

	// WHY???
	(void) SPI2S; /* Dummy read of the status register */
	(void) SPI2DL; /* Dummy read of the device register */

	/* SPI2BR: 
	 * NULL = 0
	 * SPPR2 = 0	// Baud prescaler divisor
	 * SPPR1 = 0
	 * SPPR0 = 0
	 * NULL = 0
	 * SPR2 = 1		// Baud rate divisor = 64
	 * SPR1 = 0
	 * SPR0 = 1
	 */
	//Set the baud rate register to the initialisation rate of 375kHz.
	SPI2BR = 0x05;

	/* SPI2C1: 
	 * SPIE = 0		// Receive buffer & mode fault interrupts disabled
	 * SPE = 0		// SPI system inactive
	 * SPTIE = 0	// Transmit buffer empty interrupts disabled
	 * MSTR = 1		// Operate in master mode
	 * CPOL = 0		// Active high (idles low)
	 * CPHA = 0		// Data sampled on rising edge
	 * SSOE = 0		// Slave select output disabled
	 * LSBFE = 0	// Transfer starts with most significant bit
	 */
	// Configure port for master mode, MSB first, Clock mode 0
	SPI2C1 = 0x10;

	/* SPI2C2:
	 * SPMIE = 0	// Interrupts disabled (polling)
	 * SPIMODE = 0	// 8-bit mode
	 * NULL = 0
	 * MODFEN = 0	// Mode fault function disabled
	 * BIDIROE = 0	// Bidirectional mode disabled
	 * NULL = 0
	 * SPISWAI = 0	// Continue operation in wait mode
	 * SPC0 = 0 	// Separate pins for I/O
	 */
	// Configure port in 4-pin mode
	SPI2C2 = 0x00;

	SPI2C1_SPE = 1; /* Enable device */
}

/**
 * Enable the SD Card SPI link
 */
void enableSDSerial(void) {
	SPI2C1_SPE = 1;
}

/**
 * Disable the SD Card SPI link
 */
void disableSDSerial(void) {
	SPI2C1_SPE = 0;
}

/**
 * Wait until the transmit data register is empty
 * Caution - This is a blocking function
 */
void waitForSerialEmpty(void) {
	while (!SPI2S_SPTEF) {
	}
}

/**
 * Transmit a byte over the SD serial link (SPI2)
 */
void writeByteSDSerial(UINT8 data) {
	waitForSerialEmpty();
	SPI2DL = data; // Write data to transmit register.
	waitForSerialEmpty();
}

/**
 * Receive a byte over the SD serial link (SPI2)
 */
UINT8 readByteSDSerial(void) {
	(void) SPI2DL; // Clear the receive data register
	SPI2DL = 0xFF; // Transmit dummy byte (0xFF)

	// Wait for byte to be received
	while (!SPI2S_SPRF) {
	}

	return (SPI2DL); // Read received byte
}

/**
 * Set the SD serial link clock rate to 6MHz
 */
void setHighRateSDSerial(void) {
	disableSDSerial();

	SPI2BR = 0x00; // SPI2 clock rate 12MHz for 24 Mhz bus clock      		

	enableSDSerial();
}

void formatSDCard(){
	SD_Erase_Blocks(SD_START_BLOCK, 8192);
}

/***********************************************************************
 *
 *    SD_Init
 *
 *  Description: Configures the SPI2 module and the File system to handle
 *                 an SD Card.
 *
 *  Returns:     OK if SPI configured, SD card is present, CMD0 true, 
 *                  SD card initialised, block length set to 512, 
 *                  SPI clock set to 4MHz.
 *               NO_SD_CARD if no SD card is present.
 *               INIT_FAILS if CMD0 (idle) fails, block length set fails.
 *               
 *               .
 *
 *************************************************************************/
UINT8 initialiseSD(void) {
	UINT8 Count;
	UINT8 res;
	UINT8 CRC7_BYTE;
	UINT8 j;
	UINT8 i = 0;
	uint8_t errorFlag;

	// Insert detection pin as an input
	_SD_PRESENT = _IN;

	// Enable internal pull-up resistor
	SD_PRESENT_PU = 1;

	initialiseSDSerial(); // SS high, 375kHz clock.

	// SD requires a 1ms delay after power-up.
	delay(SD_POWER_UP_DELAY);

	/* Check for SD */
	if (SD_PRESENT) // Pin is low when SD Card present
		return (NO_SD_CARD);
//??		return (0x11);

	/*** Start SD card Init ***/

	/* Minimum of 74 dummy clock cycles with MOSI high & SS enabled (low) */
	SD_CLKDelay(10); // Send 80 clocks with SS disabled
	SPI_SS_Disable_Delay();

	/* CMD0 - GO_IDLE_STATE
	 Reset the SD card & place it in SPI mode.
	 CRC must be valid.
	 Valid reset command is 0x40, 0x00, 0x00, 0x00, 0x00, 0x95.
	 Correct response is R1 (1 byte) = SD_IDLE (0x01).  
	 Note that the command frame starts with bits 47:46=0:1 (start & transmission bits) 
	 so the CMD byte has b6 set. */SPI_SS = ENABLE;
	sdArgument.lword = 0; // CMD0 argument is 0x00, 0x00, 0x00, 0x00.
	errorFlag = sendCommandSdSerial(SD_CMD0 | 0x40, SD_IDLE);
	if (errorFlag) {
		SPI_SS = DISABLE;
//        return(INIT_FAILS);
		return (0x12);
	}
	/* Card is in Idle State */
	SPI_SS_Disable_Delay();
	SPI_SS = DISABLE;

	SD_CLKDelay(1); // 8 dummy clocks

	/* Check if card is Ver2 compliant.
	 Send a SEND_IF_COND (CMD8) with supply voltage (VHS) set to 2.7-3.6volts.
	 Argument will be 0x000001AA and CRC7 + end bit 0x87.
	 CMD8 response is R7 (5 bytes - R1 + argument). */

	SPI_SS = ENABLE;
	sdArgument.lword = 0x000001AA;
	CRC7_BYTE = 0x87;
	if (SD_SendCommandR7(SD_CMD8, CRC7_BYTE)) {
		SPI_SS = DISABLE;
//        return(INIT_FAILS);
		return (0x13);
	}
	/* Card is version 2 compliant */
	SPI_SS_Disable_Delay();
	SPI_SS = DISABLE;

	SD_CLKDelay(1); // 8 dummy clocks

	/* Read the Operation Conditions Register OCR & check for correct voltage range.
	 Valid voltage range of 2.7-3.6 volts is indicated by gu8SD_Response[2:3] = 0xFF:0x80.
	 Reference Table 5-1 P92.
	 Send READ_OCR CMD58.
	 CMD58 response R3 (5bytes - R1 + OCR). */SPI_SS = ENABLE;
	sdArgument.lword = 0x00000000;
	CRC7_BYTE = 0xFD;
	if (SD_SendCommandR3(SD_CMD58, SD_IDLE, CRC7_BYTE)) // Correct R1 is SD_IDLE (0x01)
			{
		SPI_SS = DISABLE;
//        return(INIT_FAILS);
		return (0x14);
	}

	/* Valid response received */
	SPI_SS_Disable_Delay();
	SPI_SS = DISABLE;
	SD_CLKDelay(1); // 8 dummy clocks
	/* Check for correct voltage range */
	if ((sdResponse[2] != 0xFF) && (sdResponse[3] != 0x80)) {
//      return(INIT_FAILS);
		return (0x15);
	}

	/* Initialise Card */
	/* Send continuous SD_SEND_OP_COND (ACMD41) until R1 b0 clears. */

	j = 0; // Clear loop counter
	do {
		j++;
		/* The next command is an application-specific command */
		do { // Wait for a successful CMD55
			/* The next command is an application-specific command */
			SPI_SS = ENABLE; // Enable SS
			sdArgument.lword = 0x00000000;
//        res = SD_SendCommand(SD_CMD55|0x40,0x80);  // returns OK (0x00) if  
			res = sendCommandSdSerial(SD_CMD55 | 0x40, SD_IDLE); // returns OK (0x00) if
			//  any valid response received
			SPI_SS_Disable_Delay();
			SPI_SS = DISABLE;
			SD_CLKDelay(1); // 8 dummy clocks
		} while (res);

		/* Send SD_SEND_OP_COND (ACMD41) */
		do {

			SPI_SS = ENABLE; // Enable SS
			/* High Capacity Support (HCS - argument b30) is set if host supports
			 SDHC or SDXC, wgich it does not in this case. */
//        gu8SD_Argument.lword = 0x40000000;            // HCS set
			sdArgument.lword = 0x00000000; // HCS clear
			res = sendCommandSdSerial(SD_CMD41 | 0x40, 0x80); // returns OK (0x00) if
//        res = SD_SendCommand(SD_CMD41|0x40,SD_OK);     // returns OK (0x00) if
					//  any valid response received
			SPI_SS_Disable_Delay();
			SPI_SS = DISABLE;
			SD_CLKDelay(1); // 8 dummy clocks
		} while (res);
	} while ((sdResponse[0] != SD_OK) && (j < SD_IDLE_WAIT_MAX));

	/* Assume we're OK if we didn't hit the timeout */
	if (j >= SD_IDLE_WAIT_MAX)
//       return(INIT_FAILS);
		return (0x16);

	/* Read the Operation Conditions Register OCR & 
	 check the Card Power Up Status bit b31 and Card Capacity Status (CCS) b30. 
	 b31 & b30 of the OCR which is gu8SD_Response[1] b6. 
	 Power Up Status Bit will be 1 when the card is finished powering up.
	 CCS is 0 for a SDSC. Ref Table 5-1 P92.
	 Send READ_OCR CMD58.
	 CMD58 response R3 (5bytes - R1 + OCR). */SPI_SS = ENABLE;
	sdArgument.lword = 0x00000000;
	CRC7_BYTE = 0xFD;
	if (SD_SendCommandR3(SD_CMD58, SD_OK, CRC7_BYTE)) // Correct R1 response is SD_OK (0x00)
			{
		SPI_SS = DISABLE;
//        return(INIT_FAILS);
		return (0x17);
	}
	/* Valid response received */
	SPI_SS_Disable_Delay();
	SPI_SS = DISABLE;
	SD_CLKDelay(1); // 8 dummy clocks

	/* Check Power Up Status */
	if ((sdResponse[1] & 0x80) == 0x00)
		return (0x19); // return Power Up not finished.

	/* Check CCS  */
	if (sdResponse[1] & 0x40)
		return (0x1A); // return Card is not SDSC.

	/*  Set the Block Length to 512 butes */SPI_SS = ENABLE;

	sdArgument.lword = SD_BLOCK_SIZE; // 512 bytes
	if (sendCommandSdSerial(SD_CMD16 | 0x40, SD_OK)) {
		SPI_SS = DISABLE;
//        return(INIT_FAILS);
		return (0x18);
	}

	SPI_SS_Disable_Delay();
	SPI_SS = DISABLE;
	SD_CLKDelay(1); // 8 dummy clocks

//    WriteSPIByte(0x00);
//    WriteSPIByte(0x00);

	/* Read the Card-Specific Data register (CSD) to gu8SD_CSD[16] */
	if (SD_Read_CSD()) {
		return (0x1B); // error 11. CSD read fail
	}

	setHighRateSDSerial();

	return (OK);
}

/*****************************************************************************
 *
 *   SPI_SS_Disable_Delay
 *
 *  Description:  Provides a short delay to allow the completion of a character 
 *                transmission before disabling the slave select.
 *
 ******************************************************************************/
void SPI_SS_Disable_Delay(void) {
	UINT8 Count, i = 0;
	for (Count = 0; Count < 50; Count++) {
		i += 1;
	}

}

/***************************************************************************************
 *
 *     *** SD_CBufferDataSectorWrite ***
 *
 *     Description: Write a 512 bytes from the circular buffer as a data sector.
 *                  Assume:
 *                         - SD Card has been previously erased.
 *                         - Write a full sector at a time.
 *
 *     Entry: void. 
 *
 ****************************************************************************************/
void SD_CBufferDataSectorWrite(void) {
	UINT16 u16Counter;

	SPI_SS = ENABLE;

	sdArgument.lword = dataSector++; // Set WRITE_BLOCK command argument to the current data segment
	sdArgument.lword = sdArgument.lword << SD_BLOCK_SHIFT; // Convert sector number to byte address

	// WRITE_BLOCK command
	if (sendCommandSdSerial(SD_CMD24 | 0x40, SD_OK)) {
		SPI_SS = DISABLE;
		return;
	}

	writeByteSDSerial(0xFE); // Start Block Token

	for (u16Counter = 0; u16Counter < BLOCK_SIZE; u16Counter++) // Write the block
		writeByteSDSerial(FromCBuffer());

	writeByteSDSerial(0xFF); // checksum Bytes not needed
	writeByteSDSerial(0xFF);

	// Correct response is 0x05
	// Foust p5.
	while ((readByteSDSerial() & 0x0F) != 0x05)
		;

	while (readByteSDSerial() == 0x00)
		; // Wait while the card is busy

	SPI_SS = DISABLE;

	SD_CLKDelay(1); // 8 dummy clocks

	return;

}

/*****************************************************************************
 *
 **************SD_Write_Block************
 *
 *      Description: Write a 512 byte sector (block) to the SD card.
 *
 *      Inputs:      u16SD_Block - the sector (block) number where the block is
 *                                 to be written.
 *                   pu8DataPointer - the 512 byte buffer that is to be written
 *                                    to the SD card.
 *
 *****************************************************************************/
UINT8 writeBlockSDSerial(UINT32 blockNumber, UINT8 *dataPointer) {
	UINT16 i;

	SPI_SS = ENABLE;

	sdArgument.lword = blockNumber; // Set WRITE_BLOCK command argument
	sdArgument.lword = sdArgument.lword << SD_BLOCK_SHIFT; // Convert sector number to
														   // byte address

	if (sendCommandSdSerial(SD_CMD24 | 0x40, SD_OK)) // WRITE_BLOCK command
			{
		SPI_SS = DISABLE;
		return (WRITE_COMMAND_FAILS);
	}

	writeByteSDSerial(0xFE); // Start Block Token

	for (i = 0; i < BLOCK_SIZE; i++) // Write the block
		writeByteSDSerial(*dataPointer++);

	writeByteSDSerial(0xFF); // checksum Bytes not needed
	writeByteSDSerial(0xFF);

	for (i = 0; i < BLOCK_SIZE; i++)
		;

	if ((readByteSDSerial() & 0x0F) != 0x05) // Read the status token.
			// Correct response is 0x05
			// Foust p5.
			{
		SPI_SS = DISABLE;
		return (WRITE_DATA_FAILS);
	}

	while (readByteSDSerial() == 0x00)
		; // Wait while the card is busy

	SPI_SS = DISABLE;

	SD_CLKDelay(1); // 8 dummy clocks

	return (OK);
}

/*****************************************************************************
 *
 ************* SD_Erase_Blocks ***********
 *
 *      Description: Erase multiple sectors (blocks) no the SD card.
 *
 *      Inputs:      u32SD_Start_Block - the sector (block) number of the first
 *                                 block to be erased.
 *                   u32SD_End_Block - the sector (block) number of the last
 *                                    block to be erased.
 *
 *****************************************************************************/
UINT8 SD_Erase_Blocks(UINT32 startBlock, UINT32 endBlock) {
	UINT8 res;
	UINT16 u16Counter;

	SPI_SS = ENABLE;

	/* Set the address of the first write block to be erased */
	sdArgument.lword = startBlock;
	sdArgument.lword = sdArgument.lword << SD_BLOCK_SHIFT; // Convert sector number to byte address

	if (sendCommandSdSerial(SD_CMD32 | 0x40, SD_OK)) // ERASE_WR_BLK_START_ADDR command
			{
		SPI_SS = DISABLE;
		return (WRITE_COMMAND_FAILS);
	}

	SD_CLKDelay(1); // 8 dummy clocks

	/* Set the address of the last write block to be erased */
	sdArgument.lword = endBlock; // Set ERASE_WR_BLK_END_ADDR 
								 //   command argument
	sdArgument.lword = sdArgument.lword << SD_BLOCK_SHIFT; // Convert sector number to byte address

	if (sendCommandSdSerial(SD_CMD33 | 0x40, SD_OK)) // ERASE_WR_BLK_END_ADDR command
			{
		SPI_SS = DISABLE;
		return (WRITE_COMMAND_FAILS);
	}

	SD_CLKDelay(1); // 8 dummy clocks

	/* Send the erase command */
	sdArgument.lword = 0x00000000; // stuff bits 

	if (sendCommandSdSerial(SD_CMD38 | 0x40, SD_OK)) // ERASE command
			{
		SPI_SS = DISABLE;
		return (WRITE_COMMAND_FAILS);
	}

	/* Wait while card is busy. Indicated by reading 0x00 from SD Card */
	u16Counter = 0; // Counter to see if it is doing something
	do {
		res = readByteSDSerial();
		u16Counter++;
	} while (res == 0x00); // Wait while the card is busy

	SPI_SS = DISABLE;

	SD_CLKDelay(1); // 8 dummy clocks

	return (OK);
}

/*****************************************************************************
 *
 *     *** SD_Read_CSD ***
 *
 *     Read the SD Card Card-Specific Data Register (CSD).
 *
 *     The CSD is a 16 byte SD Card register that is read with the 
 *       CMD9 SEND_CSD command.
 *
 *     Output: If read successful return OK with CSD register in gu8SD_CSD[0-16]
 *                with MSB in gu8SD_CSD[0].
 *             Else return READ_COMMAND_FAILS.
 *
 *
 ******************************************************************************/
UINT8 SD_Read_CSD(void) {
	UINT8 u8Temp = 0, u8Counter;

	SPI_SS = ENABLE;

	sdArgument.lword = 0x00000000;

	if (sendCommandSdSerial(SD_CMD9 | 0x40, SD_OK)) {
		SPI_SS = DISABLE;
		return (READ_COMMAND_FAILS);
	}

	while (u8Temp != 0xFE)
		u8Temp = readByteSDSerial();

	for (u8Counter = 0; u8Counter < 16; u8Counter++)
		gu8SD_CSD[u8Counter] = readByteSDSerial();

	(void) readByteSDSerial(); // Dummy SPI cycle for CRC
	(void) readByteSDSerial();

	SPI_SS = DISABLE;

	SD_CLKDelay(1); // 8 dummy clocks

	return (OK);
}

UINT8 SD_Read_Block(UINT32 u16SD_Block, UINT8 *pu8DataPointer) {
	UINT8 u8Temp = 0;
	UINT16 u16Counter;

	SPI_SS = ENABLE;

	sdArgument.lword = u16SD_Block;
	sdArgument.lword = sdArgument.lword << SD_BLOCK_SHIFT;

	if (sendCommandSdSerial(SD_CMD17 | 0x40, SD_OK)) {
		SPI_SS = DISABLE;
		return (READ_COMMAND_FAILS);
	}

	while (u8Temp != 0xFE)
		u8Temp = readByteSDSerial();

	for (u16Counter = 0; u16Counter < BLOCK_SIZE; u16Counter++)
		*pu8DataPointer++ = readByteSDSerial();

	(void) readByteSDSerial(); // Dummy SPI cycle, read CRC
	(void) readByteSDSerial();

	SPI_SS = DISABLE;

	SD_CLKDelay(1); // 8 dummy clocks

	return (OK);
}

/*****************************************************************************
 *
 *   SD_SendCommand
 *
 *  Description:  Sends a command to the SD card and receives the response.
 *                All data is sent MSB first, and MSb first.
 *                NB: Does not handle SS signal.
 *
 *  Input:        u8SDCommand - The command to send with start bit (b7) clear
 *                              and transmission bit (b6) set.
 *                u8SDResponse - The correct R1 response byte.
 *                               If 0x80 don't care what the response is so
 long as its a byte with b7 = 0.
 *                
 *  Response:     Return OK (0x00) if a response is received, 
 *                       COMMAND_FAILS (0x01) if not.
 *                Response is stored in response[] with the number of response
 *                     bytes controlled by response_type.
 *
 *****************************************************************************/
UINT8 sendCommandSdSerial(UINT8 sdCommand, UINT8 u8SDResponse) {
	uint8_t i;
	uint8_t CRC7;
	volatile UINT8 temp = 0;

	writeByteSDSerial(sdCommand);

	/* Send Argument, MSB first. */
	for (i = 0; i < 4; i++) {
		writeByteSDSerial(sdArgument.bytes[i]);
	}

	/* Send CRC */
	switch (sdCommand) {
	case (SD_CMD55 | 0x40): // Application-specific block
		CRC7 = 0x65;
		break;
	case (SD_CMD41 | 0x40): // ACMD41 with argument 0x40000000
		CRC7 = 0x77;
		break;
	default:
		CRC7 = 0x95;
	}
	writeByteSDSerial(CRC7);

	/* Response Handler */
	i = SD_WAIT_CYCLES; // 10 byte reads
	if (u8SDResponse == 0x80) {
		while ((temp & 0x80) && i >= 0) {
			temp = readByteSDSerial();
			i--;
		}

	} else {
		/* Get a response that matches u8SDResponse */
		while ((temp != u8SDResponse) && i >= 0) {
			temp = readByteSDSerial(); // Transmit 0xFF
			i--;
		}
	}

	/* Save the response */
	sdResponse[0] = temp;
	sdResponse[1] = i;

	/* Set up return conditions */
	if (i > 0) {
		return (OK); // 0x00
	} else {
		return (COMMAND_FAILS); // 0x01
	}
}

/*****************************************************************************
 *
 *   SD_SendCommandR7
 *
 *  Description:  Sends a command to the SD card that requires an R7 (5 byte) response. 
 *                Used by SEND_IF_COND (CMD8) to check if card is Ver2 compliant.
 *
 *  Input:        u8SDCommand - the command.
 *                u8CRC7 - the CRC7 + end bit for the command (with Tx bit set)
 *                             and argument.
 *                The 4 byte argument "gu8SD_Argument" must be set before 
 *                     calling this function.
 *
 *  Response:     OK (0x00) if valid R7 response received else COMMAND_FAILS (0x01).
 *
 *****************************************************************************/
UINT8 SD_SendCommandR7(UINT8 u8SDCommand, UINT8 u8CRC7) {
	UINT8 u8Counter, mlCounter, i;
	volatile UINT8 u8Temp = 0;

	/* Debug checks */
	for (i = 0; i <= 4; i++) { // Initialise gu8SD_Response[] so you can be sure its changed
		sdResponse[i] = 0xAA;
	}

	/* Main loop - Send the command and wait for a valid response */
	mlCounter = 5; // loop max of 5 times
	do {
		mlCounter--;
		/* Send command byte with set transmission bit (b6) set */
		writeByteSDSerial(u8SDCommand | 0x40);

		/* Send Argument */
		for (u8Counter = 0; u8Counter < 4; u8Counter++) {
			i = sdArgument.bytes[u8Counter];
			writeByteSDSerial(sdArgument.bytes[u8Counter]);
		}

		/* Send CRC */
		writeByteSDSerial(u8CRC7);

		/* Response Handler. 
		 Do up to SD_WAIT_CYCLES card reads looking for a valid response.
		 Valid response is 5 bytes which are stored in gu8SD_Response[].
		 Valid response is received MSB first.
		 The five bytes are: 
		 - gu8SD_Response[0]; MSB is a normal R1 response which is SD_IDLE (0x01).
		 - gu8SD_Response[1-4]; is the argument that was transmitted.
		 The card responds with 0xFF until it transmits the valid response. */
		u8Counter = SD_WAIT_CYCLES; // Do up to SD_WAIT_CYCLES card reads

		/* Response loop - Do up to SD_WAIT_CYCLES card reads looking for a valid response */
		do // Look for a R1 response byte
		{
			u8Temp = readByteSDSerial(); // Transmit 0xFF. MOSI line must be kept high
			u8Counter--;
		} while ((u8Temp & 0xC0) && u8Counter > 0); // Response loop test
													// - exit on valid response or timeout

	} while ((u8Temp & 0xC0) && mlCounter > 0); // Main loop test
												// - exit on valid response or timeout

	/* Place the 5 byte response in gu8SD_Response[] */
	sdResponse[0] = u8Temp; // R1
	for (i = 1; i <= 4; i++) { // Argument
		sdResponse[i] = readByteSDSerial();
	}

	/* Set up return condition */
	if ((sdResponse[0] == SD_IDLE) && (sdResponse[1] == sdArgument.bytes[0])
			&& (sdResponse[2] == sdArgument.bytes[1])
			&& (sdResponse[3] == sdArgument.bytes[2])
			&& (sdResponse[4] == sdArgument.bytes[3]))
		return (OK); // 0x00

	else
		return (COMMAND_FAILS); // 0x01
}

/*****************************************************************************
 *
 *   SD_SendCommandR3
 *
 *  Description:  Sends a command to the SD card that requires an R3 (5 byte) response. 
 *                Used by SEND_IF_COND (CMD58) to read the OCR.
 *
 *  Input:        u8SDCommand - the command.
 *                u8SDResponse - The correct R1 response for this command.
 *                u8CRC7 - the CRC7 + end bit for the command (with Tx bit set)
 *                             and argument.
 *                The 4 byte argument "gu8SD_Argument" must be set before 
 *                     calling this function.
 *
 *  Response:     OK (0x00) if valid R3 response received else COMMAND_FAILS (0x01).
 *                The R1 response is available in  gu8SD_Response[0].
 *                The 32 bit Operation Conditions Register (OCR) is available in
 *                    gu8SD_Response[1-4] with MSB in gu8SD_Response[1].
 *
 *****************************************************************************/
UINT8 SD_SendCommandR3(UINT8 u8SDCommand, UINT8 u8SDResponse, UINT8 u8CRC7) {
	UINT8 u8Counter, mlCounter, i;
	volatile UINT8 u8Temp = 0;

	/* Debug checks */
	for (i = 0; i <= 4; i++) { // Initialise gu8SD_Response[] so you can be sure its changed
		sdResponse[i] = 0xAA;
	}

	/* Main loop - Send the command and wait for a valid response */
	mlCounter = 5; // loop max of 5 times
	do {
		mlCounter--;
		/* Send command byte with set transmission bit (b6) set */
		writeByteSDSerial(u8SDCommand | 0x40);

		/* Send Argument */
		for (u8Counter = 0; u8Counter < 4; u8Counter++) {
			i = sdArgument.bytes[u8Counter];
			writeByteSDSerial(sdArgument.bytes[u8Counter]);
		}

		/* Send CRC */
		writeByteSDSerial(u8CRC7);

		/* Response Handler. 
		 Do up to SD_WAIT_CYCLES card reads looking for a valid response.
		 Valid response is 5 bytes which are stored in gu8SD_Response[].
		 Valid response is received MSB first.
		 The five bytes are: 
		 - gu8SD_Response[0]; MSB is a normal R1 response which is SD_IDLE (0x01).
		 - gu8SD_Response[1-4]; is the argument that was transmitted.
		 The card responds with 0xFF until it transmits the valid response. */
		u8Counter = SD_WAIT_CYCLES; // Do up to SD_WAIT_CYCLES card reads

		/* Response loop - Do up to SD_WAIT_CYCLES card reads looking for a valid response */
		do // Look for a R1 response byte
		{
			u8Temp = readByteSDSerial(); // Transmit 0xFF. MOSI line must be kept high
			u8Counter--;
		} while ((u8Temp != u8SDResponse) && u8Counter > 0); // Response loop test
//    } while((u8Temp & 0xC0) && u8Counter > 0);   // Response loop test
		// - exit on valid response or timeout

	} while ((u8Temp != u8SDResponse) && mlCounter > 0); // Main loop test
//  } while((u8Temp & 0xC0) && mlCounter > 0);     // Main loop test
	// - exit on valid response or timeout

	/* Place the 5 byte response in gu8SD_Response[] */
	sdResponse[0] = u8Temp; // R1
	for (i = 1; i <= 4; i++) { // OCR
		sdResponse[i] = readByteSDSerial();
	}

	/* Set up return condition */
	if (sdResponse[0] == u8SDResponse)
		return (OK); // 0x00

	else
		return (COMMAND_FAILS); // 0x01
}

void SD_CLKDelay(UINT8 frames) {
	while (frames > 0) {
		writeByteSDSerial(0xFF);
		frames--;
	}
}

