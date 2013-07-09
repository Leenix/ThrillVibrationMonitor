/******************************************************************************
 *                                                  
 *
 *  File Name:   Vib_Cont_01.c
 *
 *  Project:     Thrill vibration sensor
 *               
 *                                                                          
 *  Description: Main Operating Functions.
 *
 *  Processor:   MC9S08JM60CLD 
 *                                            
 *  Version:     1.0                                                         
 *                                                                                                                                                         
 *  Author:      Karl Mohring                                                                                                                                   
 *                                                  
 * UPDATED HISTORY:
 *
 * REV   YYYY.MM.DD  AUTHOR        DESCRIPTION OF CHANGE
 * ---   ----------  ------        --------------------- 
 * 0.0   2011.01.15  Peter Grabau  Initial version
 * 1.0	 2013.07.09	 Karl Mohring  Adapted code
 *
 ******************************************************************************/

#include "Fat.h"
#include "Vib_Cont_01.h"
#include "Cpu.h"   // for Cpu_DisableInt()
#include "IO_Map.h"  // for port operation
#include "SD.h"

/******************** CONSTANTS ******************************/
#define CBufferSize 2048  // # bytes in the circular buffer 

/*** Set up the FAT to a file of 11,000 clusters which is approx
			 11,000 clusters * 32768 bytes/cluster = 360,448kB, or
			 360,448kB / 31 bytes/sample = 11,627,354 samples, or
			 11,627,354 samples / 2,000 samples/sec = 5,814 seconds of sampling, or
			 approx 1.61 hours of samples. ***/
// Number of clusters for project file.
const int NUM_CLUSTERS = 11000;

//Number of bytes per cluster for the project file
const int CLUSTER_SIZE = 32768;

// Number of accelerometer channels being sampled.
const byte NUM_CHANNELS = 3;

// Size of record counter in bytes
const byte RECORD_COUNTER_SIZE = 2;

// Size of divisor (comma) in bytes
const byte DIVISOR_SIZE = 1;

// Size of line delimiter characters (CR,LF) in bytes
const byte LINE_DELIMETER_SIZE = 2;

// Size of an accelerometer sample from a single channel in bytes
const byte SAMPLE_SIZE = 2;

// Size of each record in CSV format in bytes
const int RECORD_SIZE = RECORD_COUNTER_SIZE + DIVISOR_SIZE + NUM_CHANNELS*(SAMPLE_SIZE + DIVISOR_SIZE) + LINE_DELIMETER_SIZE;

const byte RUNTIME_HOURS = 0;
const byte RUNTIME_MINUTES = 10;
const byte RUNTIME_SECONDS = 0;

// Runtime of the sampling session in seconds
const long TOTAL_RUNTIME = 3600*RUNTIME_HOURS + 60*RUNTIME_MINUTES + RUNTIME_SECONDS;

const TIME_FACTOR = 2;
/******************** USED VARIABLES *************************/

UINT8 u8sd_status; /* Variable to indicate SD status */
UINT8 u8sd_command; /* Variable to indicate commands for SD */
UINT8 u8irda_status; /* Variable to indicate commands for IrDA */
UINT8 u8buffer[512]; /* Array to store information of the SD */
UINT16 u16buffer_index; /* Variable to indicate position of buffer */
UINT16 u16file_counter; /* Variable to indicate remaining bytes of a file */

/* Variables for storing Accelerometer readings */
UINT16 Accel[4]; /* Stores the record number and the 3 accelerometer readings */
UINT16 RecordNumber; /* Record Number: the first column of a sample record */

/* Variables for controlling the sampling operation */
//UINT8  Sampling;          /* True when sampling is in progress */
UINT8 samplingStatus;
UINT8 System_Status; /* System is currently sampling */
UINT8 u8CircularBuffer[CBufferSize]; // Sample buffer
UINT16 u16CBufferPutter; // points to next vacant byte location in the buffer
UINT16 u16CBufferGetter; // points to the next available byte in the buffer
UINT16 u16CBufferBytes; // # bytes in the buffer
UINT16 u16TempCBufferBytes;
UINT8 ReadDataStop; // Blocks reading sensor daya while writing to the SD card
//UINT16 LostReads;
UINT8 BufferEmpty;
UINT8 u8AccelSamples[6]; // 3 x 16 bit results, MSB first
UINT8 u8Channel; // The channel that is being sampled

long sessionRuntime;

extern UINT16 u16FAT_Data_BASE;
extern WriteRHandler WHandler;
extern UINT32 u32DataSector;
extern UINT16 u16FAT_Data_BASE;
extern UINT16 u16FAT_Cluster_Size;

/**
 * Pause the program for a specified amount of time.
 * @param milliseconds Delay period in milliseconds.
 */
void delay(unsigned long milliseconds) {
	unsigned long i;
	for (i = 0; i < (milliseconds * TIME_FACTOR); i++) {
	}
}



/**
 * Initialise the thrill sensor board and associated components.
 * The SD card and interface are also configured and a project file is created.
 */
void initialiseSensorBoard(void) {

	// Disable timer until after sampling starts
	sampleTimer_Disable();
	
	initialiseLEDs();
	pulsePowerLED(5);
	
	/* Initialise sampling buffers */
	u16CBufferPutter = 0;
	u16CBufferGetter = 0;
	u16CBufferBytes = 0;
	ReadDataStop = FALSE;
	
	sessionRuntime = 0;

	/* Vibration Controller Board System Status */
	System_Status = NOT_SAMPLING; // Sample switch is not set


	/* Configure SPI module to handle a SD card. Return OK if successful. */
	u8sd_status = SD_Init();
	
	// Continuously loop error code if SD initialisation is unsuccessful
	if (u8sd_status != OK) {
		u8sd_status -= 0x10;
		for(;;){
			pulseReceiveLED(u8sd_status);
			pulseTransmitLED(1);
		}
	}
	pulseStatusLED(2); //SD init OK
	
	// Check that the SD Card is present and it contains the file "FILE.TXT" and initialise the File System (FAT)
	vfnSDwelcome();
	pulseReceiveLED(2);

	/* Initialise the sample number */
	RecordNumber = 0x0001;
	InterruptNumber = 0x0001;
}

/**
 * Set up the boards LED indicators
 */
void initialiseLEDs(){
	/* Set Sensor 1 LED pin as an output */
		_powerLED = _OUT; // DDR to output
		powerLED = OFF; // pin low, LED off

		/* Set Sensor 2 LED pin as an output */
		_statusLED = _OUT; // DDR to output
		statusLED = OFF; // pin low, LED off

		/* Set Sensor 3 LED pin as an output */
		_receiveLED = _OUT; // DDR to output
		receiveLED = OFF; // pin low, LED off

		/* Set Sensor 4 LED pin as an output */
		_transmitLED = _OUT; // DDR to output
		transmitLED = OFF; // pin low, LED off
}

/**
 * Pulse the power LED a specified number of times
 * @pulses number of times to pulse LED
 */
void pulsePowerLED(UINT8 pulses) {
	UINT16 Count, j;

	while (pulses) {

		/* j = 20 gives 1mSec delay */

		/* LED1 off for 1 Sec */
		powerLED = OFF;
		for (j = 0; j < 1000; j++) {
			for (Count = 0; Count < 250; Count++) {}
		}
		
		/* LED1 ON for 0.1 sec */
		powerLED = ON;
		for (j = 0; j < 200; j++) {
			for (Count = 0; Count < 500; Count++) {}
		}
		
		powerLED = OFF;

		pulses--;
	}

}

/**
 * Pulse the status LED a specified number of times
 * @pulses number of times to pulse LED
 */
void pulseStatusLED(UINT8 pulses) {
	UINT16 Count, j;

	while (pulses) {

		/* j = 200 gives 10mSec delay */

		/* LED2 off for 1 Sec */
		statusLED = OFF;
		for (j = 0; j < 1000; j++) {
			for (Count = 0; Count < 250; Count++) {
				//        i += 1;
			}
		}

		/* LED2 ON for 0.1 sec */statusLED = ON;
		for (j = 0; j < 200; j++) {
			for (Count = 0; Count < 500; Count++) {
				//        i += 1;
			}
		}
		statusLED = OFF;

		pulses--;
	}

}

/**
 * Pulse the receive LED a specified number of times
 * @pulses number of times to pulse LED
 */
void pulseReceiveLED(UINT8 pulses) {
	UINT16 Count, j;

	while (pulses) {

		/* j = 200 gives 10mSec delay */

		/* LED3 off for 1 Sec */
		receiveLED = OFF;
		for (j = 0; j < 1000; j++) {
			for (Count = 0; Count < 250; Count++) {
				//        i += 1;
			}
		}

		/* LED3 ON for 0.1 sec */receiveLED = ON;
		for (j = 0; j < 200; j++) {
			for (Count = 0; Count < 500; Count++) {
				//        i += 1;
			}
		}
		receiveLED = OFF;

		pulses--;
	}

}

/**
 * Pulse the transmit LED a specified number of times
 * @pulses number of times to pulse LED
 */
void pulseTransmitLED(UINT8 pulses) {
	UINT16 Count, j;

	while (pulses) {

		/* j = 200 gives 10mSec delay */

		/* LED4 off for 1 Sec */
		transmitLED = OFF;
		for (j = 0; j < 1000; j++) {
			for (Count = 0; Count < 250; Count++) {
				//        i += 1;
			}
		}

		/* LED4 ON for 0.1 sec */transmitLED = ON;
		for (j = 0; j < 200; j++) {
			for (Count = 0; Count < 500; Count++) {
				//        i += 1;
			}
		}
		transmitLED = OFF;

		pulses--;
	}

}

/***********************************************************************
 *
 *********** VibrationControl ***********
 *
 *  Description: Main operating function for the Vibration Controller Board.
 *               
 *                  
 *                  
 *                  
 *
 *               Called from main() in SD_Card_Writer.c
 *
 *************************************************************************/
void startSampling(void) {
	UINT8 count = 0;
	samplingStatus = SAMPLING;
	
	sampleTimer_Enable();
	
	while (samplingStatus == SAMPLING){

		__DI();
		u16TempCBufferBytes = u16CBufferBytes;
		__EI();
		if (u16TempCBufferBytes >= BLOCK_SIZE) {
			ReadDataStop = TRUE; // Stop data read while writing to SD card
			SD_CBufferDataSectorWrite(); // write block to SD card
			ReadDataStop = FALSE;

		}

	}
}

/***********************************************************************
 *
 *    vfnSDwelcome
 *
 *  Description: Checks if SD Card present - if not ?????
 *               If Card is present: 
 *                  If file "FILE.TXT" exists it is opened,
 *                  If file does not exist it is created and opened.
 *                  File is then closed.
 *
 *               Called from main() in SD_Card_Writer.c
 *
 *************************************************************************/
void vfnSDwelcome(void) {
	UINT8 result;
	UINT16 i;

	/* Check if SD card present - if not ????? */
	if (u8sd_status == NO_SD_CARD) // Status set by SD_Init() in SD.c.
			{
		/* SD Card not present */
		Cpu_DisableInt(); // Disable interrupts
		for (;;){
			pulseReceiveLED(1);
		}

	} else {
		pulsePowerLED(1);
		/* SD Card present, read the Master Block (Boot Sector) of the memory */
		FAT_Read_Master_Block(); // in Fat.c

		pulseStatusLED(1);
		/* Make sure file "FILE.TXT" exists */
		u8sd_status = FAT_FileOpen("RESULTS.TXT", MODIFY); // Attempt to open file

		pulseReceiveLED(1);

		/* Does file exist? */
		if (u8sd_status == FILE_NOT_FOUND) {
			/* If the file does not exist, it is created */
			u8sd_status = FAT_FileOpen("RESULTS.TXT", CREATE);
			
			/* Set "u32DataSector" to the first data sector to be written.
			 u32DataSector is used in function FAT_DataSectorWrite() in Fat.c
			 for writing a block of data to the SD card. */

			// Create the FAT for the project data file
			for (i = 0; i < NUM_CLUSTERS; i++) {
				(void) FAT_Entry(WHandler.CurrentFatEntry, WHandler.CurrentFatEntry + 1, WRITE_ENTRY);
				WHandler.CurrentFatEntry++;
				WHandler.File_Size += CLUSTER_SIZE; // update the file size, 32768 bytes per cluster
			}

		}

		/* Use this to append to an existing file */
//       u32DataSector= u16FAT_Data_BASE + WHandler.ClusterIndex + (WHandler.CurrentFatEntry-2)*u16FAT_Cluster_Size;
		/* Use this to place the first data block at the start of the SD Card data region. */
		u32DataSector = u16FAT_Data_BASE;

		/* Erase the SD Card data area. Return OK if successful. */
//    result = SD_Erase_Blocks(u16FAT_Data_BASE,u16FAT_Data_BASE + 8192); // 4MB
		result = SD_Erase_Blocks(u16FAT_Data_BASE, u16FAT_Data_BASE + 4000000); // 2GB
		if (result != OK) {
			FAT_FileClose();
			pulseReceiveLED(5);
			for (;;)
				;
		}

		/* Close file */
		FAT_FileClose();

		//This part sets the initial values of these variables
		u16buffer_index = 0;
		u8sd_command = SD_INACTIVE;
	}
}

/***********************************************************************
 *
 *    vfnWriteFile
 *
 *  Description: Writes the contents of the buffer "u8buffer" to the file "FILE.TXT".
 *                
 *
 *************************************************************************/
void vfnWriteFile(void) {
	UINT8 i = 0;

	u8sd_status = FAT_FileOpen("FILE.TXT", MODIFY);
	if (u8sd_status == FILE_FOUND) {

		//Write the u8buffer to the file
		FAT_FileWrite(u8buffer, u16buffer_index);
		FAT_FileClose();

		//Sets these variables to inactive states 
		u16buffer_index = 0;
		u8sd_command = SD_INACTIVE;
	} else {
		//If the file was not found, debug trap
		i += 1;
		;
	}

}

/***********************************************************************
 *
 *    StringToFile
 *
 *  Description: Writes the contents of the buffer "u8buffer" to the file "FILE.TXT".
 *                
 *
 *************************************************************************/
void StringToFile(void) {

	UINT8 i = 0, *str = "Hello World"; // String to write to file

	/* Transfer the string to the u8buffer */
	u16buffer_index = 0; // Zero buffer pointer

	while (str[i]) {
		u8buffer[u16buffer_index++] = str[i++];
	}

	/* Write string to file */
	vfnWriteFile();

}

/***********************************************************************
 *
 *    Uint16ToString
 *
 *  Description: Converts Num to a 5 char string and places it in 
 *                 the buffer "u8bufferEvan/u8bufferOdd".
 *               0.1mSec execution time.
 *
 *  Input:       Num - the UINT16 number to convert.
 *               pu8Buffer - the buffer where the data is to be saved.
 *               putter    - pointer to the buffer putter.
 *                
 *
 *************************************************************************/
void Uint16ToString(UINT16 Num, UINT8 *pu8Buffer, UINT16 *putter) {

	UINT8 result8, digit;
	UINT16 result16;

	result16 = Num;

	/* 10 thousands */
	digit = result16 / 10000;
	pu8Buffer[(*putter)++] = (digit + 0x30);
	result16 = result16 % 10000;

	/* thousands */
	digit = result16 / 1000;
	pu8Buffer[(*putter)++] = (digit + 0x30);
	result16 = result16 % 1000;

	/* hundreds */
	digit = result16 / 100;
	pu8Buffer[(*putter)++] = (digit + 0x30);
	result8 = result16 % 100;

	/* tens */
	digit = result8 / 10;
	pu8Buffer[(*putter)++] = (digit + 0x30);
	digit = result8 % 10;

	/* units */
	pu8Buffer[(*putter)++] = (digit + 0x30);

}

/***********************************************************************
 *
 *    ToBuffer
 *
 *  Description: Places Digit in the buffer "u8buffer".
 *               u16buffer_index is post incremented.
 *                
 *
 *************************************************************************/
void ToBuffer(UINT8 Digit) {

	u8buffer[u16buffer_index++] = Digit;

}

/***********************************************************************
 *
 *    *** ReadData ***
 *
 *  Description: Read sampled data from the sensor boards and save in
 *               the circular buffer "u8CircularBuffer".
 *               Only if there is space in the buffer and 
 *               not writing to the SD card.
 *
 *  Calling fn:  Called from SamplingDelay_OnInterrupt() in Events.c 
 *               as part of the SamplingDelay Interrupt Service Routine.
 *                
 *
 *************************************************************************/
void ReadData(void) {

	/* Check there is space in the buffer for a sample ("SampleSize" bytes) */
	if ((u16CBufferBytes < (CBufferSize - RECORD_SIZE)) && (ReadDataStop == FALSE))
		if (u16CBufferBytes < (CBufferSize - RECORD_SIZE)){
			/* Place a sample line in the circular buffer */
			writeRecordToBuffer();
		} else {
			//LostReads++;
		}
}

/***********************************************************************
 *
 *    *** DataTest ***
 *
 *  Description: Place a sample line of demo accelerometer data in the
 *               circular buffer "u8CircularBuffer".
 *               Uses accelerometer data from Sensor board 1, rest
 *               is dummy data.
 *
 *  Input:       void.
 *                
 *
 *************************************************************************/
void DataTest(void) {
	UINT8 i;
	UINT8 j;

	/* Load demo accelerometer result data */
	Accel[0] = InterruptNumber; // 16 bit S/H pulse sequential counter

	/* Write accelerometer data to circular buffer in csv format */
	for (i = 0; i <= 8; i++) { // write first 8 data words + "," to buffer
		ToCBuffer((UINT8) (Accel[i] >> 8)); // MSB
		ToCBuffer((UINT8) (Accel[i] & 0xFF)); // LSB
		ToCBuffer(','); // add comma seperator
	}
	// write last data word to buffer
	ToCBuffer((UINT8) (Accel[9] >> 8)); // MSB
	ToCBuffer((UINT8) (Accel[9] & 0xFF)); // LSB

	ToCBuffer(CR); // add Carriage Return terminator 0x0D
	ToCBuffer(LF); // add Line Feed terminator 0x0A

	RecordNumber++; // Increment the record number count

}

/**
 * Write the latest record to the circular buffer in CSV format.
 * The format is as follows: [Record number],[Z1],[Z2],[Z3][CR][LF]
 */
void writeRecordToBuffer() {
	UINT8 i;
	UINT8 recordUpper;
	UINT8 recordLower;

	// Write record number to buffer, MSB first
	recordUpper = RecordNumber >> 8;
	recordLower = RecordNumber && 8;
	ToCBuffer(recordUpper);
	ToCBuffer(recordLower);
	
	// Write accelerometer data to buffer in csv format. (again, MSB first)
	for(i = 0; i < 2*NUM_CHANNELS; i+=2){
		ToCBuffer(',');
		ToCBuffer(u8AccelSamples[i]);
		ToCBuffer(u8AccelSamples[i + 1]);
	}

	// New line
	ToCBuffer(CR); // add Carriage Return terminator 0x0D
	ToCBuffer(LF); // add Line Feed terminator 0x0A
	RecordNumber++;
}

/********************************************************************
 *
 *          *** ToCBuffer ***
 *
 *   Description: Place the byte "u8byte"in the circular buffer 
 *                 "u8CircularBuffer" at "u16CBufferPutter".
 *                Already know there is space in the buffer
 *
 *   Exit:        Byte in the buffer
 *                u16CBufferPutter incremented.
 *                # of bytes in the buffer "u16CBufferBytes" incremented
 *
 **********************************************************************/
void ToCBuffer(UINT8 u8byte) {
	if (u16CBufferPutter >= CBufferSize) // check for buffer wrap around
		u16CBufferPutter = 0x0000;

	u8CircularBuffer[u16CBufferPutter++] = u8byte;
	u16CBufferBytes++; // inc # bytes in buffer
}

/********************************************************************
 *
 *          *** FromCBuffer ***
 *
 *   Description: Get the next byte from the circular buffer
 *                 "u8CircularBuffer" at "u16CBufferGetter".
 *                Already know there is a byte available in the buffer
 *
 *   Exit:        Byte returned
 *                u16CBufferGetter incremented.
 *                # of bytes in the buffer "u16CBufferBytes" decremented
 *
 **********************************************************************/
UINT8 FromCBuffer(void) {
	UINT8 returnbyte;

	__DI();

	if (u16CBufferGetter >= CBufferSize){ // check for buffer wrap around
		u16CBufferGetter = 0x0000;
	}
	
	u16CBufferBytes--;
	
	returnbyte = u8CircularBuffer[u16CBufferGetter++];
	__EI();
	return (returnbyte);
}

/***********************************************************************
 *
 *    *** Sample_Accel ***
 *
 *  Description: Sample the three accelerometer channels
 *               This function will be called every 500uSec..
 *               Pulses the LED every 2 sec.   
 *                  
 *                  
 *
 *               Called from EInt1_OnInterrupt() in Events.c
 *
 *************************************************************************/
void Sample_Accel(void) {

	UINT8 adc_result;
	UINT8 i;
	
	// Sequentially perform ADC conversions on each channel
	for (u8Channel = 0; u8Channel < NUM_CHANNELS; u8Channel++) {
		adc_result = AD1_MeasureChan(FALSE, u8Channel); // Don't wait for sample complete

		// Halt CPU while conversion is taking place to reduce noise. Wait is removed upon conversion interrupt.
		asm {
			wait
		}

	}
	
	RecordNumber++;

}

/**
 * Interrupt service routine for ADC conversion complete
 * Data is copied to a temporary buffer
 */
void ADC_ISR(void) {
	u8AccelSamples[u8Channel << 1] = ADCRH; // MSB
	u8AccelSamples[(u8Channel << 1) + 1] = ADCRL; // LSB
}

/**
 * Toggle the power LED (LED1)
 */
void togglepowerLED(void) {
	if (powerLED == ON) {
		powerLED = OFF;
	} else {
		powerLED = ON;
	}
}

/**
 * Toggle the status LED (LED2)
 */
void toggleStatusLED(void) {
	if (statusLED == ON) {
		statusLED = OFF;
	} else {
		statusLED = ON;
	}
}

/**
 * Increment the time on the session clock by one second.
 * If the session limits are reached, sampling is stopped
 */
void incrementRunningClock(void){
	sessionRuntime++;
	
	if (sessionRuntime > TOTAL_RUNTIME){
		stopSampling();
	}
	
}

/**
 * Stop recording samples by stopping periodic interrupts.
 */
void stopSampling(void){
	samplingStatus = NOT_SAMPLING;
	statusLED = OFF;
}
