/**
 Parser de Arquivos WAV
 Autores: Gustavo Pinheiro e Bruno Pessoa
 Data: Abril de 2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wavefile.h"

/// Pixels per second
#define PPS 100
#define LINE_HEIGHT 257
#define BLACK " 110 "
#define WHITE " 255 "
#define TESTING 0

/// Global variables
FILE *pgmptr = NULL;
FILE *wavptr = NULL;
WaveHeader header = {0};
FormatChunk format = {0};
DataChunk data = {0};
char *filename;
byte *chosenSamples;
unsigned samplesCount = 0;

/* Function Prototypes */

/// Fills the WaveHeader Structure
void fillHeader();

/// Fills the FormatChunk Structure
void fillFormat();

/// Fills the DataChunk Structure
void fillData();

///Creates the PGM file
void createPGM();

/// Captura os pontos da onda WAV para ser plotado no PGM.
void chooseSamples();

/// Fills the PGM file with the chosen samples
void fillPGM();

///Plots a row with a top and a bottom value
void plotValue(short int min, short int max);

/// Returns the byte-inverted version of the input (2 bytes version)
char* lil_e_to_big_e_2(byte *input);

/// Returns the byte-inverted version of the input (4 bytes version)
char* lil_e_to_big_e_4(byte *input);

/// Removes the file extension
char *removeFileExt(char* mystr);

/** Implementations */

int main(int argc, char** argv) {
    
    //Checking given filepath
    short required = TESTING ? 1 : 2;
    
    if (argc != required) { //argc != 2
        char *progname = strrchr(argv[0], '/');
        printf("Uso correto: %s \"arquivo\"\n", &progname[1]);
        
        return 400;
    } else {
        filename = argv[1];
//        filename = "C:\\Users\\Bruno Pessoa\\Downloads\\wav-parser-master\\animals.wav";
//        filename = "C:\\Users\\gustavo\\sample.wav";
//        filename = "/Users/gustavo/Downloads/Silent8.wav";
        filename = TESTING ? "/Users/gustavo/Developer/SEMB/8k8bitpcm.wav" : filename;
//        filename = "/Users/gustavo/Developer/SEMB/JumpMono.wav";
        
        wavptr = fopen(filename, "r");
        
        /// Invalid file
        if (wavptr == NULL) {
            printf("ERROR: File not found.\n");
            return 404;
        }
        
        fillHeader();
        fillFormat();
        
        /// Unsupported sampleRate / resolution
        if (format.numChannels > 1 || format.bitsPerSample != 8) {
            printf("ERROR: Only works with 8-bit mono files\n");
            fclose(wavptr);
            return 403;
        }
        
        // Filling DataChunk
        fillData();
        
        // Allocating array for getting samples
        byte sampley[data.chunkSize/(format.sampleRate/PPS)][2];
        chosenSamples = sampley[0];
        
        // File stuff
        createPGM();
        chooseSamples();
        fillPGM();
        
        fclose(pgmptr);
        fclose(wavptr);
    }
    
    return 0;
}

void fillHeader(){
    byte littleEndian[4];
    
    rewind(wavptr);
    
    fread(header.groupID, sizeof(header.groupID), 1, wavptr);
    
    fread(littleEndian, sizeof(header.fileLength), 1, wavptr);
    header.fileLength = (unsigned) lil_e_to_big_e_4(littleEndian);
    
    fread(header.sRiffType, 4, 1, wavptr);
}

void fillFormat(){
    
    byte littleEndian4[4];
    byte littleEndian2[2];
    
    // Going to format-chunk position on file, just for making sure
    fseek(wavptr, 12, SEEK_SET);
    
    // groupId
    fread(format.groupID, sizeof(format.groupID), 1, wavptr);
    
    // chunkSize
    fread(littleEndian4, sizeof(format.chunkSize), 1, wavptr);     // Getting data in Little Endian
    format.chunkSize = (unsigned) lil_e_to_big_e_4(littleEndian4);
    
    // formatTag: '1' for LPCM
    fread(littleEndian2, sizeof(format.formatTag), 1, wavptr);     // Getting data in Little Endian
    format.formatTag = (unsigned short) lil_e_to_big_e_2(littleEndian2);
    
    // numChannels
    fread(littleEndian2, sizeof(format.numChannels), 1, wavptr);     // Getting data in Little Endian
    format.numChannels = (unsigned short) lil_e_to_big_e_2(littleEndian2);
    
    // sampleRate
    fread(littleEndian4, sizeof(format.sampleRate), 1, wavptr);     // Getting data in Little Endian
    format.sampleRate = (unsigned) lil_e_to_big_e_4(littleEndian4);
//    printf("Sample Rate Chars: %02X %02X %02X %02X\n", littleEndian4[0], littleEndian4[1], littleEndian4[2], littleEndian4[3]);
    
    // byteRate
    fread(littleEndian4, sizeof(format.avgBytesPerSec), 1, wavptr);     // Getting data in Little Endian
    format.avgBytesPerSec = (unsigned) lil_e_to_big_e_4(littleEndian4);
//    printf("Byte Rate Chars: %02X %02X %02X %02X\n", littleEndian4[0], littleEndian4[1], littleEndian4[2], littleEndian4[3]);
    
    // blockAlign
    fread(littleEndian2, sizeof(format.blockAlign), 1, wavptr);     // Getting data in Little Endian
    format.blockAlign = (unsigned short) lil_e_to_big_e_2(littleEndian2);
    
    // bitsPerSample
    fread(littleEndian2, sizeof(format.bitsPerSample), 1, wavptr);     // Getting data in Little Endian
    format.bitsPerSample = (unsigned) lil_e_to_big_e_2(littleEndian2);
    
    printf("Channels   : %5d\n", format.numChannels);
    printf("Sample Rate: %5dHz\n", format.sampleRate);
    printf("Resolution : %5d-bits\n", format.bitsPerSample);
}

void fillData(){
    byte buffer[4];
    
    fseek(wavptr, 36, SEEK_SET);
    fread(data.groupID, sizeof(data.groupID), 1, wavptr);
    fread(buffer, sizeof(buffer), 1, wavptr);
    
    data.chunkSize = (unsigned) lil_e_to_big_e_4(buffer);
}

void createPGM(){
    char *cropped = strrchr(filename, '/');
    char *namext, *namenoxt;
    if (cropped != NULL) {
        namext = cropped;
        namenoxt = removeFileExt(&namext[1]);
    } else {
        namext = filename;
        namenoxt = removeFileExt(namext);
    }
    
    pgmptr = fopen(strcat(namenoxt, ".pgm"), "w");
    fprintf(pgmptr, "P2 %d %d 255", LINE_HEIGHT, data.chunkSize/(format.sampleRate/PPS));
}

void chooseSamples(){
    
    printf("\nGenerating PGM file...");
    
    int i = 0, j = 0, step = format.avgBytesPerSec/PPS;
    byte buffer, max = 128, min = 127;
    
    while (i < data.chunkSize/step){
        if (format.bitsPerSample == 8) {
            for (j = 0; j < step; j++) {
                fread(&buffer, sizeof(buffer), 1, wavptr);
                if(buffer > max) max = buffer;
                if(buffer < min) min = buffer;
            }
            chosenSamples[2*(i+1)-1] = min;
            chosenSamples[2*(i+1)] = max;
            max = 128;
            min = 127;
        }
        i++;
    }
    samplesCount = i;
}

void fillPGM(){
    int i;
    for (i = 0; i < samplesCount; i++) {
        plotValue(chosenSamples[2*(i+1)-1], chosenSamples[2*(i+1)]);
    }
    printf("\nPlotted %d samples\n", i);
}

void plotValue(short int min, short int max){
    int range = (LINE_HEIGHT - 1)/2, i = 0;
    
    /// Normalizing between 1 and 128
    int down = 128 - min, up = max - 127;
    
    /// Normalizing between 1 and range
    int down_mod = down * range/128;
    int up_mod = up * range/128;
    
    // Paddings
    int padBottom = range - down_mod;
    int padTop = range - up_mod;
    
    //Plot Bottom Paddding
    for (i = 0; i < padBottom; i++) {
        fprintf(pgmptr, WHITE);
    }
    
    // Plot bottom value
    for (i = 0; i < down_mod; i++) {
        fprintf(pgmptr, BLACK);
    }
    
    //Center point
    fprintf(pgmptr, BLACK);
    
    // Plot bottom value
    for (i = 0; i < up_mod; i++){
        fprintf(pgmptr, BLACK);
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
    
//    printf("\n");
}

char* lil_e_to_big_e_2(byte *littleEndian){
    return littleEndian[0] | (littleEndian[1] << 8);
}

char* lil_e_to_big_e_4(byte *littleEndian){
    return littleEndian[0] | (littleEndian[1]<<8) | (littleEndian[2]<<16) | (littleEndian[3]<<24);
}

char *removeFileExt(char* mystr) {
    char *retstr;
    char *lastdot;
    if (mystr == NULL)
        return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    return retstr;
}