/**
 Parser de Arquivos WAV
 Autores: Gustavo Pinheiro e Bruno Pessoa
 Data: Abril de 2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wave.h"

#define LINE_HEIGHT 513
#define BLACK " 0 "
#define WHITE " 255 "
#define TESTING 0

/// Global variables
FILE *pgmptr = NULL;

///Capturar as amostras APENAS quando o arquivo wav.h for RELATIVAMENTE pequeno(10KB para baixo)!
///2879 == Length do áudio jump8.wav (3 KB)
byte samples[2879];

/* Function Prototypes */
///Creates the PGM file
void createPGM();

///Captura os pontos da onda WAV para serem plotados no PGM.
void fillSamples();

///Fills the PGM file with the chosen samples
void fillPGM();

///Plots a row with a top and a bottom value
void plotValue(short value);

/** Implementations */

int main(int argc, char** argv) {

    /// Unsupported sampleRate / resolution
    if (chNumber > 1 || resolution != 8) {
        printf("ERROR: Only works with 8-bit mono files\n");
        return 403;
    }

    createPGM();
    fillSamples();
    fillPGM();

    return 0;
}

void createPGM() {
    pgmptr = fopen("C:\\Users\\Bruno Pessoa\\Desktop\\parser\\parser_project\\wave.pgm", "w");
    fprintf(pgmptr, "P2 %d %d 255", LINE_HEIGHT, dataSize);
}

void fillSamples() {
    int i;
    for(i=0; i < dataSize; i++) {
        samples[i] = data[i];
    }
}

void fillPGM(){
    int i;
    for (i = 0; i < dataSize; i++) {
        plotValue(samples[i]);
    }
}

void plotValue(short value) {
    int range = (LINE_HEIGHT - 1)/2, i = 0;

    /// Normalizing between 1 and 128
    byte flagUp = 1;

    if (value < 128) {
        value = 128 - value;
        flagUp = 0;
    } else {
        value -= 127;
    }

    /// Normalizing between 1 and range
    int value_mod = value * range/128;

    // Paddings
    int padBottom = range, padTop = range;
    if (flagUp == 1)
        padTop -= value_mod;
    else
        padBottom -= value_mod;

    //Plot Bottom Paddding
    for (i = 0; i < padBottom; i++) {
        fprintf(pgmptr, WHITE);
    }

    if(flagUp == 0) {
        // Plot bottom value
        for (i = 0; i < value_mod; i++) {
            fprintf(pgmptr, BLACK);
        }
        //Center point
        fprintf(pgmptr, BLACK);
    } else {
        fprintf(pgmptr, BLACK);
        // Plot top value
        for (i = 0; i < value_mod; i++){
            fprintf(pgmptr, BLACK);
        }
    }

    //Plot Top Paddding
    for (i = 0; i < padTop; i++) {
        fprintf(pgmptr, WHITE);
    }
//    printf("Range: %d\n", range);
//    printf("Values : %4d and %3d\n", min, max);
//    printf("Abs    : %4d and %3d\n", down, up);
//    printf("Modules: %4d and %3d\n", down_mod, up_mod);
//    printf("Padding: %4d and %3d\n", padBottom, padTop);
//    printf("Total  : %4d\n", padBottom+down_mod+1+up_mod+padTop);
}
