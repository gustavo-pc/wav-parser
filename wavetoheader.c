/**
    Wave to Header
    April 2016, IFCE - Bruno Pessoa and Gustavo Pinheiro

    Description: An auxiliar program for generating header files for being used in the 'WAVE to PGM' application for PIC18
                 It outputs the LPCM bytes from the WAVE file in a header file
    Input      : An 8-bit mono WAVE file
    Output     : A header file 'wave.h' containing an array of sampels
*/

#include <stdio.h>
#include <stdlib.h>

/// WAVE file location
#define WAV_LOCATION "C:\\Users\\gustavo\\Developer\\SEMB\\wav-parser\\jump8.wav"

typedef unsigned char uint8_t;

FILE *wavptr = NULL;
FILE *headerptr = NULL;

void writeHeader();
void readAndPrint();

int main() {
    wavptr = fopen(WAV_LOCATION, "r");

    if (wavptr == NULL) {
        printf("Could not load the file\n");
        return 404;
    }
    headerptr = fopen("wave.h", "w");

    writeHeader();

    // Doin the magic
    readAndPrint();

    // Closing Filestreams
    fclose(wavptr);
    fclose(headerptr);

    return 0;
}


/// Prints the default values on the header file
void writeHeader(){
    fprintf(headerptr, "typedef unsigned char byte;\n");
    fprintf(headerptr, "byte chNumber = 1;\nshort sampleRate = 8000;\nbyte resolution = 8;\nrom byte data[] = {");
}


/// Iterates on the Wave file and prints the data in integer format on the array of samples
void readAndPrint(){

    uint8_t buffer = 0;
    int i = 0;

    /// Jumping to the 'data' chunk of the WAVE file
    fseek(wavptr, 96 + 8, SEEK_SET);

    printf("Generating header file...\n");

    /// Reading and printing all the content on the .h
    while(!(feof(wavptr))) {
        fread(&buffer, sizeof(uint8_t), 1, wavptr);
        fprintf(headerptr, "%d,", buffer);
        i++;
    }

    fseek(headerptr, -1, SEEK_CUR);
    fprintf(headerptr, "};\n");
    fprintf(headerptr, "short dataSize = %d;\n", i);

    printf("Done!!!\n");
}
