/**
 Parser de Arquivos WAV
 Autor: Gustavo Pinheiro Correia
 Data: Abril de 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wavefile.h"

#define PPS 80
#define LINE_HEIGHT 257

/// Global variables
FILE *pgmptr = NULL;
FILE *wavptr = NULL;
WaveHeader header = {0};
FormatChunk format = {0};
DataChunk data = {0};

/// Function Prototypes
void plotRow(short int value);
void fillHeader();
void fillFormat();
void fillData();
void createPGM();
void getSamplesVector();
char* lil_e_to_big_e_2(const char *input);
char* lil_e_to_big_e_4(byte *input);


int main(int argc, char** argv) {
    //Checking given filepath
    if (argc != 2) { //argc != 2
        char *progname = strrchr(argv[0], '/');
        printf("Uso correto: %s \"nomedoarquivo\"\n", &progname[1]);
        
        return 0;
    } else {
        char *filename = argv[1];
//        char *filename = "C:\\Users\\Bruno Pessoa\\Downloads\\wav-parser-master\\animals.wav";
//        char *filename = "C:\\Users\\gustavo\\sample.wav";
//        char *filename = "/Users/gustavo/Downloads/Silent8.wav";
//        char *filename = "/Users/gustavo/Developer/SEMB/8k8bitpcm.wav";
//        char *filename = "/Users/gustavo/Developer/SEMB/JumpMono.wav";
        
//        printf("Your file is %s\n", filename);
        
        wavptr = fopen(filename, "r");
        
        if (wavptr == NULL) return 1;
        
        fillHeader();
        fillFormat();
        
        if (format.numChannels > 1 || format.bitsPerSample != 8) {
            printf("Only works with 8-bit mono files\n");
            return 1;
        }
        
        fillData();
        createPGM();
        getSamplesVector();
        
        fclose(pgmptr);
        fclose(wavptr);
    }
    
    return 0;
}

/// Fills the WaveHeader Structure
void fillHeader(){
    char littleEndian[4];
    
    rewind(wavptr);
    
    fread(header.groupID, sizeof(header.groupID), 1, wavptr);
    
    fread(littleEndian, sizeof(header.fileLength), 1, wavptr);
    header.fileLength = lil_e_to_big_e_4(littleEndian);//littleEndian[0] | (littleEndian[1]<<8) | (littleEndian[2]<<16) | (littleEndian[3]<<24);
    
    fread(header.sRiffType, 4, 1, wavptr);
}

/// Fills the FormatChunk Structure
void fillFormat(){
    
    byte littleEndian4[4];
    byte littleEndian2[2];
    
    // Going to format-chunk position on file, just for making sure
    fseek(wavptr, 12, SEEK_SET);
    
    // groupId
    fread(format.groupID, sizeof(format.groupID), 1, wavptr);
    
    // chunkSize
    fread(littleEndian4, sizeof(format.chunkSize), 1, wavptr);     // Getting data in Little Endian
    format.chunkSize = lil_e_to_big_e_4(littleEndian4);
    
    // formatTag: '1' for LPCM
    fread(littleEndian2, sizeof(format.formatTag), 1, wavptr);     // Getting data in Little Endian
    format.formatTag = lil_e_to_big_e_2(littleEndian2);
    
    // numChannels
    fread(littleEndian2, sizeof(format.numChannels), 1, wavptr);     // Getting data in Little Endian
    format.numChannels = lil_e_to_big_e_2(littleEndian2);
    
    // sampleRate
    fread(littleEndian4, sizeof(format.sampleRate), 1, wavptr);     // Getting data in Little Endian
    format.sampleRate = lil_e_to_big_e_4(littleEndian4);
//    printf("Sample Rate Chars: %02X %02X %02X %02X\n", littleEndian4[0], littleEndian4[1], littleEndian4[2], littleEndian4[3]);
    
    // byteRate
    fread(littleEndian4, sizeof(format.avgBytesPerSec), 1, wavptr);     // Getting data in Little Endian
    format.avgBytesPerSec = lil_e_to_big_e_4(littleEndian4);
//    printf("Byte Rate Chars: %02X %02X %02X %02X\n", littleEndian4[0], littleEndian4[1], littleEndian4[2], littleEndian4[3]);
    
    // blockAlign
    fread(littleEndian2, sizeof(format.blockAlign), 1, wavptr);     // Getting data in Little Endian
    format.blockAlign = lil_e_to_big_e_2(littleEndian2);
    
    // bitsPerSample
    fread(littleEndian2, sizeof(format.bitsPerSample), 1, wavptr);     // Getting data in Little Endian
    format.bitsPerSample = lil_e_to_big_e_2(littleEndian2);
    
    printf("Channels: %d\n", format.numChannels);
    printf("Sample Rate: %dHz\n", format.sampleRate);
    printf("Resolution: %d-bits\n", format.bitsPerSample);
}

/// Fills the DataChunk Structure
void fillData(){
    int i, j;
    byte buffer[4];
    
    fseek(wavptr, 36, SEEK_SET);
    fread(data.groupID, sizeof(data.groupID), 1, wavptr);
    fread(buffer, sizeof(buffer), 1, wavptr);
    
    data.chunkSize = lil_e_to_big_e_4(buffer);
}

///Cria o arquivo PGM.
void createPGM(){
    pgmptr = fopen("waveform.pgm", "w+");
    fprintf(pgmptr, "P2 %d %d 255", LINE_HEIGHT, data.chunkSize/(format.sampleRate/PPS));
}

/// Captura os pontos da onda WAV para ser plotado no PGM.
void getSamplesVector(){
    int i = 0, step = format.avgBytesPerSec/PPS;
//    printf("\nStep: %d\nChunk Size: %d\n\n", step, data.chunkSize);
    byte sampleByte, biggest = 0, smallest = 255;
    
    printf("\nGenerating PGM file...");
    
    while (i < data.chunkSize/step){
        if (format.bitsPerSample == 8) {
            fread(&sampleByte, sizeof(char), 1, wavptr);
            fseek(wavptr, step, SEEK_CUR);
            
            if(sampleByte > biggest) biggest = sampleByte;
            if(sampleByte < smallest) smallest = sampleByte;
            
//            printf("Plotting %d %02X", sampleByte, sampleByte);
            plotRow(sampleByte - 127);
        }
        i++;
    }
    printf("\nPlotted %d samples\n", i);
    printf("Biggest: %d, Smallest: %d\n", biggest, smallest);
}

///Funcao para plotar a forma da onda no PGM
void plotRow(short int value) {
    value = abs(value);
    
    short int i=0;
    byte padding =(LINE_HEIGHT - ((value *2) + 1))/2;
//    printf("\nVALUE: %d\nPADDING: %d\n", value, padding);
    
    ///Plota espacos brancos
    for(i=0; i < padding; i++) {
        fprintf(pgmptr, " 255");
    }
//    printf("Ploted %d whitespaces", i);
    
    ///Plota espaÁos pretos
    for(i=0; i < (value*2)+1; i++) {
        fprintf(pgmptr, " 110");
    }
//    printf("\nPloted %d blank", i);
    
    ///Plota espacos brancos
    for(i=0; i < padding; i++) {
        fprintf(pgmptr, " 255");
    }
    fprintf(pgmptr, " ");
//    printf("\nPloted %d whitespaces\n\n", i);
}

/// Returns the byte-inverted version of the input (4 bytes version)
char* lil_e_to_big_e_4(byte *littleEndian){
    return littleEndian[0] | (littleEndian[1]<<8) | (littleEndian[2]<<16) | (littleEndian[3]<<24);
}

/// Returns the byte-inverted version of the input (2 bytes version)
char* lil_e_to_big_e_2(const char *littleEndian){
    return littleEndian[0] | (littleEndian[1] << 8);
}
