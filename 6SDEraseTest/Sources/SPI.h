/*
 * SPI.h
 *
 *  Created on: Jun 8, 2013
 *      Author: Leenix
 */

#ifndef __SPI__
#define __SPI__

#include "IO_Map.h"

/* Typedefs */
typedef unsigned char UINT8; /*unsigned 8 bit definition */
typedef unsigned short UINT16; /*unsigned 16 bit definition*/
typedef unsigned long UINT32; /*unsigned 32 bit definition*/
typedef signed char INT8; /*signed 8 bit definition */
typedef short INT16; /*signed 16 bit definition*/
typedef long int INT32; /*signed 32 bit definition*/

enum ISR {
	SCI_Flag, 
	ADC_Flag
};

/**
 * SD Error Codes
 */
enum SD_ERRORS{
	OK,
	COMMAND_FAILS,
	INIT_FAILS,
	WRITE_COMMAND_FAILS,
	WRITE_DATA_FAILS,
	READ_COMMAND_FAILS,
	READ_DATA_FAILS,
	NO_SD_CARD
};

/**
 * SD Status Codes
 */
enum SD_STATUS{
	SD_OK, 
	SD_IDLE, 
	SD_ERASE_RESET, 
	SD_NOT_USED, 
	SD_ILLEGAL_COMMAND
};

/**
 * SD Card Standard Commands
 */
enum SD_COMMANDS{
	SD_CMD0, /* Resets the SD Memory Card */
	SD_CMD1, /* Sends host capacity support information and activates the card's 
	 initialization process. HCS is effective when card receives SEND_IF_COND 
	 command. Reserved bits shall be set to '0'. */
	SD_CMD2,
	SD_CMD3,
	SD_CMD4,
	SD_CMD5,
	SD_CMD6, /* Checks switchable function (mode 0) and switches card function (mode 1).*/
	SD_CMD7,
	SD_CMD8, /* Sends SD Memory Card interface condition that includes host supply voltage 
	 information and asks the accessed card whether card can operate in supplied 
	 voltage range. Reserved bits shall be set to '0'.*/
	SD_CMD9, /* Asks the selected card to send its cardspecific data (CSD)*/
	SD_CMD10, /* Asks the selected card to send its card identification (CID) */
	SD_CMD11,
	SD_CMD12, /* Forces the card to stop transmission in Multiple Block Read Operation */
	SD_CMD13, /* Asks the selected card to send its status register. */
	SD_CMD14,
	SD_CMD15,
	SD_CMD16, /* Sets a block length (in bytes) for all following block commands (read and
	 write) of a Standard Capacity Card. Block length of the read and write 
	 commands are fixed to 512 bytes in a High Capacity Card. The length of 
	 LOCK_UNLOCK command is set by this command in both capacity cards.*/
	SD_CMD17, /* Reads a block of the size selected by the SET_BLOCKLEN command.*/
	SD_CMD18, /* Continuously transfers data blocks from card to host until interrupted by a
	 STOP_TRANSMISSION command.*/
	SD_CMD19,
	SD_CMD20,
	SD_CMD21,
	SD_CMD22,
	SD_CMD23,
	SD_CMD24, /* Writes a block of the size selected by the SET_BLOCKLEN command. */
	SD_CMD25, /* Continuously writes blocks of data until �Stop Tran� token is sent
	 (instead �Start Block�).*/
	SD_CMD26,
	SD_CMD27, /* Programming of the programmable bits of the CSD. */
	SD_CMD28, /* If the card has write protection features, this command sets the write protection bit
	 of the addressed group. The properties of write protection are coded in the card 
	 specific data (WP_GRP_SIZE). The High Capacity Card does not support this command.*/
	SD_CMD29, /* If the card has write protection features, this command clears the write protection
	 bit of the addressed group. The High Capacity Card does not support this command. */
	SD_CMD30, /* If the card has write protection features, this command asks the card to send the 
	 status of the write protection bits.6 The High Capacity Card does not support this command. */
	SD_CMD31,
	SD_CMD32, /* Sets the address of the first write block to be erased.*/
	SD_CMD33, /* Sets the address of the last write block of the continuous range to be erased. */
	SD_CMD34,
	SD_CMD35,
	SD_CMD36,
	SD_CMD37,
	SD_CMD38, /* Erases all previously selected write blocks */
	SD_CMD39,
	SD_CMD40,
	SD_CMD41,
	SD_CMD42, /* Used to Set/Reset the Password or lock/unlock the card. A transferred data block includes 
	 all the command details - refer to Chapter 4.3.7. The size of the Data Block is defined 
	 with SET_BLOCK_LEN command. Reserved bits in the argument and in Lock Card Data Structure 
	 shall be set to 0. */
	SD_CMD43,
	SD_CMD44,
	SD_CMD45,
	SD_CMD46,
	SD_CMD47,
	SD_CMD48,
	SD_CMD49,
	SD_CMD50,
	SD_CMD51,
	SD_CMD52,
	SD_CMD53,
	SD_CMD54,
	SD_CMD55, /* Defines to the card that the next command is an application specific command 
	 rather than a standard command */
	SD_CMD56, /* Used either to transfer a Data Block to the card or to get a Data Block from the card 
	 for general purpose/application specific commands. In case of Standard Capacity SD 
	 Memory Card, the size of the Data Block shall be defined with SET_BLOCK_LEN command. 
	 Block length of this command is fixed to 512-byte in High Capacity Card. */
	SD_CMD57,
	SD_CMD58, /* Reads the OCR register of a card. CCS bit is assigned to OCR[30]. */
	SD_CMD59, /* Turns the CRC option on or off. A �1� in the CRC option bit will turn the option on,
	 a �0� will turn it off */
	SD_CMD60,
	SD_CMD61,
	SD_CMD62,
	SD_CMD63
};

// Definitions
#define	_OUT 1
#define	_IN 0

#define SPI_SS PTBD_PTBD3	// Slave Select (CS pin for the SD Card)               
#define _SPI_SS PTBDD_PTBDD3	// Slave Select DDR (CS pin for the SD Card)

#define ENABLE 0
#define DISABLE 1

/* Macros */
#define FLAG_SET(BitNumber, Register)        (Register |=(1<<BitNumber))
#define FLAG_CLR(BitNumber, Register)        (Register &=~(1<<BitNumber))
#define FLAG_CHK(BitNumber, Register)        (Register & (1<<BitNumber))

#define    _BGND    asm(BGND)
#define    _NOP     asm(NOP)
#define    _WAIT    asm(WAIT)
#define    False    0x00
#define    True     0x01


/* Timeout for initialisation */
#define SD_IDLE_WAIT_MAX 255

/* R1 response masks */
#define SD_IDLE_MASK 0x01

/* User definitions */
#define SD_BLOCK_512     
#define SD_WAIT_CYCLES 10

/* SD card Inserted detection Pin */
#define SD_PRESENT      PTCD_PTCD3     // Data bit
#define _SD_PRESENT     PTCDD_PTCDD3   // Data Direction bit
#define SD_PRESENT_PU   PTCPE_PTCPE3   // Internal Pull-Up Resistor Enable bit
/* Error Codes */


/* TypeDefs */
typedef union {
	UINT8 bytes[4];
	UINT32 lword;
} T32_8;

/* Stardard Definitions */
#ifdef  SD_BLOCK_512
#define SD_BLOCK_SIZE   (0x00000200)
#define SD_BLOCK_SHIFT  (9)
#define BLOCK_SIZE      512
#endif

void initialiseSDSerial(void);
void writeByteSDSerial(UINT8 u8Data);
UINT8 readByteSDSerial(void);
void setHighRateSDSerial(void);

UINT8 initialiseSD(void);
UINT8 sendCommandSdSerial(UINT8, UINT8);
UINT8 SD_SendCommandR7(UINT8, UINT8);
UINT8 SD_SendCommandR3(UINT8, UINT8, UINT8);
void SD_CLKDelay(UINT8);
UINT8 writeBlockSDSerial(UINT32, UINT8*);
UINT8 SD_Erase_Blocks(UINT32, UINT32);
UINT8 SD_Read_Block(UINT32, UINT8*);
void SPI_SS_Disable_Delay(void);
UINT8 SD_Read_CSD(void);
void SD_CBufferDataSectorWrite(void);

#endif 
