/**
 Parser de Arquivos WAV
 Autores: Gustavo Pinheiro e Bruno Pessoa
 Data: Abril de 2016
*/
#include <p18f4550.h>
#include <usart.h>
#include <delays.h>
#include <stdio.h>
#include "wave.h"

#pragma config FOSC  = HS
#pragma config MCLRE = OFF

#define LINE_HEIGHT 513
#define BLACK " 0 "
#define WHITE " 255 "
#define TESTING 0

void jumpLineSerial(){
    printf("\n\r");
}

void printInSerial(char* str) {
    putrsUSART(str);
}

///Plots a row with a top and a bottom value
void plotValue(short value) {
    int range = (LINE_HEIGHT - 1)/2, i = 0, value_mod, padBottom, padTop;

    /// Normalizing between 1 and 128
    byte flagUp = 1;

    if (value < 128) {
        value = 128 - value;
        flagUp = 0;
    } else {
        value -= 127;
    }

    /// Normalizing between 1 and range
    value_mod = value * range/128;

    // Paddings
    padBottom = range;
    padTop = range;
    
    if (flagUp == 1)
        padTop -= value_mod;
    else
        padBottom -= value_mod;

    //Plot Bottom Paddding
    for (i = 0; i < padBottom; i++) {
        printInSerial(WHITE);
    }

    if(flagUp == 0) {
        // Plot bottom value
        for (i = 0; i < value_mod; i++) {
            printInSerial(BLACK);
        }
        //Center point
        printInSerial(BLACK);
    } else {
        printInSerial(BLACK);
        // Plot top value
        for (i = 0; i < value_mod; i++){
            printInSerial(BLACK);
        }
    }

    //Plot Top Paddding
    for (i = 0; i < padTop; i++) {
        printInSerial(WHITE);
    }
}

///Fills the PGM file with the chosen samples
void printPGMInSerial(){
//    int i;    
    putrsUSART("P2");            
    jumpLineSerial();
    putrsUSART("2 3 255");
    jumpLineSerial();
    putrsUSART("255 180 130 80 50 0");
    
//    for (i = 0; i < dataSize; i++) {
//        plotValue(data[i]);
//    }
}

void openSerialComm(){
    OpenUSART(USART_TX_INT_OFF  &
              USART_RX_INT_OFF  & 
              USART_ASYNCH_MODE &
              USART_EIGHT_BIT   &
              USART_BRGH_HIGH, 51);        
    Delay10KTCYx(1);
}

void main() {    
    openSerialComm();
   
    printPGMInSerial();
    
    while(1);
}