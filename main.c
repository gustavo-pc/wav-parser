/**
 Parser de Arquivos WAV
 Autores: Gustavo Pinheiro e Bruno Pessoa
 Data: Abril de 2016
*/

#include <stdio.h>                         //For printf in Proteus
#include <p18f4550.h>
#include <usart.h>
#include <delays.h>
#include "wave.h"

#pragma config FOSC  = HS
#pragma config MCLRE = OFF

#define PPS 50
#define LINE_HEIGHT 51
#define BLACK " 100 "
#define WHITE " 255 "
#define TESTING 0

//byte chosenSamples[2879/(8000/PPS)][2];
unsigned samplesCount = 0;

void openSerialComm(){
    OpenUSART(USART_TX_INT_OFF  &
              USART_RX_INT_OFF  &
              USART_ASYNCH_MODE &
              USART_EIGHT_BIT   &
              USART_BRGH_HIGH, 51);
    Delay10KTCYx(1);
}

void writeHeader(){
    printf("P2\n\r%d %d\n\r255\n\r", LINE_HEIGHT, dataSize/(sampleRate/PPS));
}

void plotValue(byte min, byte max){
    int range = (LINE_HEIGHT - 1)/2, i = 0;

    /// Normalizing between 1 and 128
    int down = 128 - min, up = max - 127;

    byte debug = 1;

    /// Normalizing between 1 and range
    int down_mod = (float) (down * range)/128;
    int up_mod = (float) (up * range)/128;

    // Paddings
    int padBottom = range - down_mod;
    int padTop = range - up_mod;
    
    if(debug){
        printf("Gonna print %d down and %d up", down_mod, up_mod);
    }    

    //Plot Bottom Paddding
    for (i = 0; i < padBottom; i++) {
        if(!debug) printf(WHITE);
    }

    // Plot bottom value
    for (i = 0; i < down_mod; i++) {
        if(!debug) printf(BLACK);
    }

    //Center point
    if(!debug) printf(BLACK);

    // Plot bottom value
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
    int i = 0, step = sampleRate/PPS;
    byte buffer, max = 128, min = 127;
    
    for(i=0; i < dataSize; i++) {
        if(i%step == 0 && i != 0){
            plotValue(min, max);
            //printf("Calling with: %3d, %3d\n\r", min, max);
            max = 128;
            min = 127;
            samplesCount++;
        }
        if(data[i] < min) //MIN
            max = data[i];
        if(data[i] > max) // MAX
            min = data[i];
    }
    //printf("\n\nInt nosso: %d\n\r", (dataSize/step) );
    //printf("\nVerdadeiro: %d\n\r", samplesCount);;;
}

void main() {
    openSerialComm();
    writeHeader();
    chooseSamples();

    while(1);
}
