/**
    Parser de Arquivos WAV
    Autor: Gustavo Pinheiro Correia
    Data: Abril de 2016
*/

typedef unsigned char byte;
/** Struct representing the header of a WAV file */
typedef struct wave_header {
    char groupID[4];                            /// "RIFF"
    unsigned fileLength;                        ///
    char sRiffType[4];                          /// "WAVE"
} WaveHeader;

/** Struct for keeping the metadata of the file */
typedef struct format_chunk {
    char groupID[4];                            /// "fmt " - yes, with the whitespace
    unsigned chunkSize;                         ///
    unsigned short formatTag;                   ///
    unsigned short numChannels;                 ///
    unsigned sampleRate;                        ///
    unsigned avgBytesPerSec;                    ///
    unsigned short blockAlign;                  ///
    unsigned bitsPerSample;                     ///
} FormatChunk;

/** Struct for containing the raw audio samples and some metadata */
typedef struct data_chunk {
    char groupID[4];                            /// 'data'
    unsigned int chunkSize;                         ///
    void *sampleData;                           /// byte/char for 8-bit, short for 16-bit, float for 32-bit
} DataChunk;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PPS 50
#define LINEHEIGHT 255

FILE *pgm = NULL;
FILE *fileptr = NULL;
WaveHeader header = {0};
FormatChunk format = {0};
DataChunk data = {0};

//Function Prototypes
void plotRow(short int value);
void fillHeader();
void fillFormat();
char* lil_e_to_big_e_2(const char *input);
char* lil_e_to_big_e_4(byte *input);
char* fixBuffer(char buffer[4]);

int main(int argc, char** argv) {
    //Checking given filepath
    if (0) { //argv != 2
        char *progname = strrchr(argv[0], '\\');
        printf("Uso correto: %s \"nomedoarquivo\"\n", &progname[1]);

        return 0;
    } else {
        //C:\Users\Bruno Pessoa\Downloads\wav-parser-master
        char *filename = "C:\\Users\\Bruno Pessoa\\Downloads\\wav-parser-master\\8k8bitpcm.wav";
        //char *filename = "C:\\Users\\gustavo\\sample.wav";

        printf("Your file is %s\n", filename);

        fileptr = fopen(filename, "r");
        fillHeader();
        fillFormat();
        fillData();
        createPGM();
        //getSamplesVector(PPS);
        //plotRow(128);

        fclose(pgm);
        fclose(fileptr);
    }

    return 0;
}


/// Fills the WaveHeader Structure
void fillHeader(){
    char littleEndian[4];

    rewind(fileptr);

    fread(header.groupID, sizeof(header.groupID), 1, fileptr);

    fread(littleEndian, sizeof(header.fileLength), 1, fileptr);
    header.fileLength = lil_e_to_big_e_4(littleEndian);//littleEndian[0] | (littleEndian[1]<<8) | (littleEndian[2]<<16) | (littleEndian[3]<<24);

    fread(header.sRiffType, 4, 1, fileptr);
}

/// Fills the FormatChunk Structure
void fillFormat(){

    char littleEndian4[4];
    char littleEndian2[2];

    // Going to format-chunk position on file, just for making sure
    fseek(fileptr, 12, SEEK_SET);

    // groupId
    fread(format.groupID, sizeof(format.groupID), 1, fileptr);

    // chunkSize
    fread(littleEndian4, sizeof(format.chunkSize), 1, fileptr);     // Getting data in Little Endian
    format.chunkSize = lil_e_to_big_e_4(littleEndian4);

    // formatTag: '1' for LPCM
    fread(littleEndian2, sizeof(format.formatTag), 1, fileptr);     // Getting data in Little Endian
    format.formatTag = lil_e_to_big_e_2(littleEndian2);

    // numChannels
    fread(littleEndian2, sizeof(format.numChannels), 1, fileptr);     // Getting data in Little Endian
    format.numChannels = lil_e_to_big_e_2(littleEndian2);

    size_t bytes_read = 0;

    // sampleRate
    bytes_read = fread(littleEndian4, sizeof(format.sampleRate), 1, fileptr);     // Getting data in Little Endian
    printf("Lidos %u byte(s)\n", bytes_read);
    format.sampleRate = lil_e_to_big_e_4(littleEndian4);
    printf("Sample Rate Chars: %02X %02X %02X %02X\n", littleEndian4[0], littleEndian4[1], littleEndian4[2], littleEndian4[3]);

    // byteRate
    bytes_read = fread(littleEndian4, sizeof(format.avgBytesPerSec), 1, fileptr);     // Getting data in Little Endian
    printf("Lidos %u byte(s)\n", bytes_read);
    format.avgBytesPerSec = lil_e_to_big_e_4(littleEndian4);
    printf("Byte Rate Chars: %02X %02X %02X %02X\n", littleEndian4[0], littleEndian4[1], littleEndian4[2], littleEndian4[3]);

     // blockAlign
    fread(littleEndian2, sizeof(format.blockAlign), 1, fileptr);     // Getting data in Little Endian
    format.blockAlign = lil_e_to_big_e_2(littleEndian2);

     // bitsPerSample
    fread(littleEndian2, sizeof(format.bitsPerSample), 1, fileptr);     // Getting data in Little Endian
    format.bitsPerSample = lil_e_to_big_e_2(littleEndian2);

    printf("Canais: %d\n", format.numChannels);
    printf("Sample Rate Dec: %d\n", format.sampleRate);
    printf("Resolution Dec: %d\n", format.bitsPerSample);
}

void fillData(){
    int i, j;
    byte buffer[4];

    fseek(fileptr, 36, SEEK_SET);
    fread(data.groupID, sizeof(data.groupID), 1, fileptr);

    fread(buffer, sizeof(buffer), 1, fileptr);

    //data.chunkSize = lil_e_to_big_e_4(buffer);

    printf("\nGroup ID from Data: %s\n", data.groupID);
    //printf("Buffer size: %02X %02X %02X %02X\n", buffer[0] & 0x000000FF, buffer[1] & 0x000000FF, buffer[2], buffer[3]);

    printf("Buffer size: %02X %02X %02X %02X\n", buffer[0], buffer[1], buffer[2], buffer[3]);
    data.chunkSize = lil_e_to_big_e_4(buffer);
}

/// Captura os pontos da onda WAV para ser plotado no PGM.
void getSamplesVector(int pixelsPerSec){
    int i=0, step = format.sampleRate/pixelsPerSec;
    printf("\nStep: %d\nChunk Size: %d\n", step, data.chunkSize);
    char sampleByte;

    while (i < data.chunkSize/step){
        fread(&sampleByte, sizeof(sampleByte), 1, fileptr);
        printf("%4d\t ", (int)sampleByte);
        fseek(fileptr, step, SEEK_CUR);

        plotRow(sampleByte);
        i++;
        if(i % 5 == 0) printf("\n");
   }
   printf("\n\nI = %d\n\n", i);
}

///Cria o arquivo PGM.
void createPGM(){
    pgm = fopen("waveform.pgm", "w+");
    //data.chunkSize/(format.sampleRate/PPS);
    fprintf(pgm, "P2 128 4 255");
}

///Função para plotar a forma da onda no PGM
void plotRow(short int value){
    if (value < 0) value *= -1;
    short int i=0;
    byte padding = LINEHEIGHT - ((value *2) - 1);

    printf("\nVALUE: %d\n\nPADDING: %d\n", value, padding);

    ///Plota espaços brancos
    for(i=0; i <= padding; i++){
        fprintf(pgm, " 255");
    }
    printf("\nFIRST FOR\n");
    ///Plota espaços pretos
    for(i=0; i <= (value*2)+1; i++) {
        fprintf(pgm, " 0");
    }
    printf("\nSECOND FOR\n");
    ///Plota espaços brancos
    for(i=0; i <= padding; i++){
        fprintf(pgm, " 255");
    }
    printf("\nTHIRD FOR\n");
}

/// Returns the byte-inverted version of the input (4 bytes version)
char* lil_e_to_big_e_4(byte *littleEndian){
    return littleEndian[0] | (littleEndian[1]<<8) | (littleEndian[2]<<16) | (littleEndian[3]<<24);
}

/// Returns the byte-inverted version of the input (2 bytes version)
char* lil_e_to_big_e_2(const char *littleEndian){
    return littleEndian[0] | (littleEndian[1] << 8);
}
