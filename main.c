/**
    WAVE to PGM
    April 2016, IFCE - Embedded Systems
    Gustavo Pinheiro and Bruno Pessoa

    Description: A PIC18 application that reads 8-bit mono audio data and outputs the audio waveform in a PGM file
    Input      : An array (in program memory) with the LPCM data from a WAVE file
    Output     : The content of a plain text PGM file via USART representing the audio waveform

    Requirements: A "wave.h" file containing the following
    - An array of unsigned char named "data", containing the Linear PCM samples
    - An int named "dataSize", representing the lenght of the array
    - An int "sampleRate", representing the audio sampling rate (in Hz)
    - The audio samples must be from a 8-BIT MONO file

    USART settings: 8-bit @ 500000 baud rate
*/

#include <stdio.h>                                      // For printf in Proteus
#include <p18f4550.h>                                   // PIC18F4550 defines & stuff
#include <usart.h>                                      // Serial communication
#include <delays.h>                                     // Delays

/// The header file with the LPCM data to be rendered in the image
#include "wave.h"

/// PIC configuration
#pragma config FOSC  = HS
#pragma config MCLRE = OFF

/// Pixels Per Second: The number of audio samples to be captured in a second
#define PPS                 1200

/// The line height (in pixels) for the PGM file
#define LINE_HEIGHT         201                         // Must be an odd value, because of the zero-line

/// Baud Rate for USART
#define BAUD                500000

/// Colors to be printed on PGM
#define BLACK               " 110 "
#define WHITE               " 255 "

/// Function prototypes
void low_ISR();
void prepareMeasurement();
void openSerialComm();
void writeHeader();
void chooseSamples();

/// Global variables
byte running = 0;
int  counter = 0;

/// Interrupt stuff
#pragma code low_vector = 0x08
void low_vector_code(){ _asm goto low_ISR _endasm }
#pragma code

#pragma interrupt low_ISR
void low_ISR(){
    if (running) counter++;
    INTCONbits.T0IF = 0x00;                                                     // Resetting interruption flag (so the interrupt happens again)
    //TMR0L           = 0x00;                                                   // Starting count again (necessary?)
}

void prepareMeasurement(){
    INTCONbits.GIE  = 0x01;                                                     // Enabling interruptions
    INTCONbits.T0IE = 0x01;                                                     // Enabling TMR0 overflow interrupt
    T0CON           = 0b11001000;                                               // Configuring T0CON to enable Timer0
                                                                                // 8-bits, Internal clock, 1:256 prescaler
}

/// Configures and opens the serial communication
void openSerialComm(){
    OpenUSART(USART_TX_INT_OFF  &
              USART_RX_INT_OFF  &
              USART_ASYNCH_MODE &
              USART_EIGHT_BIT   &
              USART_BRGH_HIGH, (1000000-(2*BAUD))/(2*BAUD));
    Delay10KTCYx(1);
}

/**
    Writes in the serial the header of the PGM file
    Currentky, the PGM has the WIDTH equal to our LINE_HEIGHT, and the HEIGHT depends on dataSize
    (Our WAVEFORM is drawn vertically and not horizontally as ordinary, because the write in PGM is done vertically)
*/
void writeHeader(){
    printf("P2\n\r%d %d\n\r255\n\r", LINE_HEIGHT, dataSize/(sampleRate/PPS));
}

/**
  Takes the min and max values captured in a certain interval of bytes in the data array, and prints the values used by the
  PGM file using the min value to plot the black pixels below the central axis, and the max value to plot the black pixels
  above the central axis.
*/
void plotValue(byte min, byte max){
    int range = (LINE_HEIGHT - 1)/2, i = 0;

    // Normalizing between 1 and 128
    int down = 128 - min, up = max - 127;

    // 1: true, 0: false
    byte debug = 1;

    // Normalizing between 1 and range
    int down_mod = ((float) down/128) * range; ///How many black pixels to be plotted below the central axis
    int up_mod = ((float) up/128) * range;     ///How many black pixels to be plotted above the central axis

    // Paddings are the white pixels to be plotted in the same line
    int padBottom = range - down_mod;
    int padTop = range - up_mod;

    if(debug){
        //  printf("min: %3d down:%3d down_mod:%3d    max: %3d up:%3d up_mod:%3d", min, down, down_mod, max, up, up_mod);
    }

    // Plot Bottom Paddding
    for (i = 0; i < padBottom; i++) {
        if(!debug) printf(WHITE);
    }

    // Plot Bottom value
    for (i = 0; i < down_mod; i++) {
        if(!debug) printf(BLACK);
    }

    //Central axis - is always a black pixel
    if(!debug) printf(BLACK);

    // Plot Top value
    for (i = 0; i < up_mod; i++){
        if(!debug) printf(BLACK);
    }

    //Plot Top Paddding
    for (i = 0; i < padTop; i++) {
        if(!debug) printf(WHITE);
    }
    if(!debug) printf("\n\r");
}


/**
    Iterates on the data array and chooses 'PPS' samples per second (or less, if the file is smaller than 1 sec)
    Accumulates the maximum and minimum values for each set and calls 'plotValue' when the max and min values for that interval are found
*/
void chooseSamples(){

    /// STEP: Defines the length (in bytes) of each interval of bytes to catch the minimum and maximum values to be plotted.
    int i = 0, step = sampleRate/PPS;

    // Negative values range: 127..0
    // Positive values range: 128..255
    byte max = 128, min = 127; // Max and Min assuming the smallest values for positive and negative samples
    byte buffer;

    // Iterate over all bytes from the array that contains the song data in 'wave.h'
    for(i = 0; i < dataSize; i++) {

        // Whenever 'i' reaches the length(step) of the interval, the current max and min are plotted and reseted
        if(i%step == 0 && i != 0) {
            plotValue(min, max);
            max = 128;
            min = 127;
        }

        // Checking if the read value is smaller or greater than min and max, respectively
        if(data[i] < min) min = data[i];
        if(data[i] > max) max = data[i];
    }
}


/// Starts here
void main() {
    openSerialComm();
    prepareMeasurement();
    
    // Started code execution
    printf("Started Measurement: Height = %d and PPS = %d\n\r", LINE_HEIGHT, PPS);
    running = 1;
    TMR0L   = 0;                                                        // Starting TMR0 on 0

    //writeHeader();
    chooseSamples();
    
    // Finished code execution
    running = 0;   
    printf("Interrupted %d times in %lu us\n\r", counter, (unsigned long int)counter*(128));
    
    // Preventing main() from being called again
    while(1);
}
