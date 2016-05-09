/**
 Parser de Arquivos WAV
 Autores: Gustavo Pinheiro e Bruno Pessoa
 Data: Abril de 2016
*/

/**
    The WAV Files Parser is an application to PIC18 that, in short, reads a WAV file format and generate, as output,
    prints of values to be used by a PGM file (the result is the approximate waveform from the WAV file drawn).
    The WAV file reading is currently done only statically on the PIC ROM memory.
    The output is generated through multiples prints that can only be captured by the serial in the moment. Than, gathering all
    the values printed in the serial (through a terminal) and creating a PGM file, the waveform drawn can be seen.
*/

#include <stdio.h>                         //For printf in Proteus
#include <p18f4550.h>
#include <usart.h>
#include <delays.h>
#include "wave.h"

#pragma config FOSC  = HS
#pragma config MCLRE = OFF

/**Points Per Second: Defines the quantity of intervals for each second of the song.
In each interval, we catch the maximum and the minimum bytes.
In other words, we use (2*PPS) number of bytes for each second.*/
#define PPS                 1200

/**Line Height: Defines how many pixels we are going to plot for each value
Each byte that we plot, represents one line of the PGM file.*/
#define LINE_HEIGHT         201 //Must be an odd value
#define BAUD                500000
#define BLACK               " 110 "
#define WHITE               " 255 "

///Configures and opens the serial communication
void openSerialComm(){
    OpenUSART(USART_TX_INT_OFF  &
              USART_RX_INT_OFF  &
              USART_ASYNCH_MODE &
              USART_EIGHT_BIT   &
              USART_BRGH_HIGH, (1000000-(2*BAUD))/(2*BAUD));
    Delay10KTCYx(1);
}

/**
Writes in the serial the header of the PGM file.
Currentky, the PGM has the WIDTH equals to our LINE_HEIGHT, and the HEIGHT equals to 255
(Our WAVEFORM is drawn vertically and not horizontally as ordinary, because the write in PGM is done vertically)
*/
void writeHeader(){
    printf("P2\n\r%d %d\n\r255\n\r", LINE_HEIGHT, dataSize/(sampleRate/PPS));
}


/**
  Takes the min and max values captured in a certain interval of bytes in the wav file, and prints the values used by the
  PGM file using the min value to plot the black pixels below the central axis, and the max value to plot the black pixels
  above the central axis.
*/
void plotValue(byte min, byte max){
    int range = (LINE_HEIGHT - 1)/2, i = 0;

    /// Normalizing between 1 and 128
    int down = 128 - min, up = max - 127;

    // 1: true, 0: false
    byte debug = 0;

    /// Normalizing between 1 and range
    int down_mod = ((float) down/128) * range; ///How many black pixels to be plotted below the central axis
    int up_mod = ((float) up/128) * range;     ///How many black pixels to be plotted above the central axis

    /// Paddings are the white pixels to be plotted int the same line
    int padBottom = range - down_mod;
    int padTop = range - up_mod;

    if(debug){
        printf("min: %3d down:%3d down_mod:%3d    max: %3d up:%3d up_mod:%3d", min, down, down_mod, max, up, up_mod);
    }

    //Plot Bottom Paddding
    for (i = 0; i < padBottom; i++) {
        if(!debug) printf(WHITE);
    }

    // Plot Bottom value
    for (i = 0; i < down_mod; i++) {
        if(!debug) printf(BLACK);
    }

    //Center point - is always a black pixel
    if(!debug) printf(BLACK);

    // Plot Top value
    for (i = 0; i < up_mod; i++){
        if(!debug) printf(BLACK);
    }

    //Plot Top Paddding
    for (i = 0; i < padTop; i++) {
        if(!debug) printf(WHITE);
    }
    printf("\n\r");
}

void chooseSamples(){
    /// STEP: Defines the length (in bytes) of each interval of bytes to catch the minimum and maximum values to be plotted.
    int i = 0, step = sampleRate/PPS;
    byte buffer, max = 128, min = 127;

    //Iterate over all bytes from the array that contains the song data in wave.h
    for(i=0; i < dataSize; i++) {
        //Whenever the (i) value reaches the length(step) of the interval, the current max and min are plotted and reseted.
        if(i%step == 0 && i != 0) {
            plotValue(min, max);
            max = 128;
            min = 127;
        }
        if(data[i] < min) //MIN
            min = data[i];
        if(data[i] > max) // MAX
            max = data[i];
    }
}

void main() {
    openSerialComm();
    writeHeader();
    chooseSamples();

    while(1);
}
