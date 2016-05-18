/**
 Parser de Arquivos WAV
 Autores: Gustavo Pinheiro e Bruno Pessoa
 Data: Abril de 2016
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "wavefile.h"

#define PPS             100
#define LINE_HEIGHT     257
#define BLACK           " 110 "
#define WHITE           " 255 "
#define TESTING         1
#define MEASURING       0
#define DEFAULT_FILE    "/Users/gustavo/Developer/SEMB/8k8bitpcm.wav"

/// Global variables
FILE *pgmptr = NULL;
FILE *wavptr = NULL;
WaveHeader header = {0};
FormatChunk format = {0};
DataChunk data = {0};
char *filename;
byte allSamples[150000];
int allSamplesCount = 0;
byte chosenSamples[10000][2];
unsigned samplesCount = 0;
clock_t timePassed;

/* Function Prototypes */

/// Fills the WaveHeader Structure
void fillHeader();

/// Fills the FormatChunk Structure
void fillFormat();

/// Fills the DataChunk Structure
void fillData();

/// Puts all samples
void fillSamples();

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

/// Prints memory usage information
void printMemoryInfo();

/** Implementations */

int main(int argc, char** argv) {
    
    printMemoryInfo();
    
    //Checking given filepath
    short required = TESTING ? 1 : 2;
    
    if (argc != required) { //argc != 2
        char *progname = strrchr(argv[0], '/');
        printf("Uso correto: %s \"arquivo\"\n", &progname[1]);
        
        return 400;
    } else {
        filename = argv[1];
        filename = TESTING ? DEFAULT_FILE : filename;
        
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
        
        
        fillData();
        fillSamples();
        
        timePassed = clock();

        createPGM();
        chooseSamples();
        fillPGM();
        
        timePassed = clock() - timePassed;
        
        printf("Took %lu ticks (%f seconds)\n", timePassed, ((float) timePassed)/ CLOCKS_PER_SEC);
        
        fclose(pgmptr);
        fclose(wavptr);
        
        printMemoryInfo();
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

void fillSamples(){
    fseek(wavptr, 44, SEEK_SET);
    int i = 0;
    byte buffer;
    while (!feof(wavptr)) {
        fread(&buffer, sizeof(byte), 1, wavptr);
        allSamples[i++] = buffer;
    }
    allSamplesCount = i;
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
    if(!MEASURING) fprintf(pgmptr, "P2 %d %d 255", LINE_HEIGHT, data.chunkSize/(format.sampleRate/PPS));
}

void chooseSamples(){
    
    int i = 0, j = 0, step = format.avgBytesPerSec/PPS, index = 0;
    byte buffer, max = 128, min = 127;
    
    while (i < data.chunkSize/step){
        if (format.bitsPerSample == 8) {
            for (j = 0; j < step; j++) {
//                fread(&buffer, sizeof(buffer), 1, wavptr);
                buffer = allSamples[index++];
                if(buffer > max) max = buffer;
                if(buffer < min) min = buffer;
            }
            chosenSamples[i][0] = min;
            chosenSamples[i][1] = max;
            max = 128;
            min = 127;
        }
        i++;
    }
    samplesCount = i;
    if(!MEASURING) printf("\nPlotted %d samples\n", i);
}

void fillPGM(){
    int i;
    for (i = 0; i < samplesCount; i++) {
        plotValue(chosenSamples[i][0], chosenSamples[i][1]);
    }
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
        if(!MEASURING) fprintf(pgmptr, WHITE);
    }
    
    // Plot bottom value
    for (i = 0; i < down_mod; i++) {
        if(!MEASURING) fprintf(pgmptr, BLACK);
    }
    
    //Center point
    if(!MEASURING) fprintf(pgmptr, BLACK);
    
    // Plot bottom value
    for (i = 0; i < up_mod; i++){
        if(!MEASURING) fprintf(pgmptr, BLACK);
    }
    
    //Plot Top Paddding
    for (i = 0; i < padTop; i++) {
        if(!MEASURING) fprintf(pgmptr, WHITE);
    }
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

void printMemoryInfo(){
    int pageSize = getPageSize();
    
    FILE *fptr;
    if((fptr = fopen("/proc/self/statm", "r")) == NULL){
        printf("Failed to open the proc file\n");
        exit(1);
    }
    
    char ret[50];
    fscanf(fptr, "%[^=]=%[^;];", ret);
    fclose(fptr);
    printf("\n[size][RSS][share][text][data][dp]\n%s", ret);
    printf("pages size: %d", pageSize);
}









































