/**
    Parser de Arquivos WAV
    Autor: Gustavo Pinheiro Correia
    Data: Abril de 2016
*/

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
    unsigned chunkSize;                         ///
    void *sampleData;                           /// byte/char for 8-bit, short for 16-bit, float for 32-bit
} DataChunk;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fileptr = NULL;
WaveHeader header;
FormatChunk format;

//Function Prototypes
void fillHeader();
void fillFormat();
char* lil_e_to_big_e_2(const char *input);
char* lil_e_to_big_e_4(const char *input);

int main(int argc, char** argv)
{
    //Checking given filepath
    if (0) { //argv != 2
        char *progname = strrchr(argv[0], '\\');
        printf("Uso correto: %s \"nomedoarquivo\"\n", &progname[1]);

        return 0;
    } else {
        char *filename = "C:\\Users\\gustavo\\sample.wav";

        printf("Your file is %s\n", filename);

        fileptr = fopen(filename, "r");
        fillHeader();
        fillFormat();

        fclose(fileptr);
    }

    return 0;

}


/// Fills the WaveHeader Structure
void fillHeader()
{
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
}

/// Returns the byte-inverted version of the input (4 bytes version)
char* lil_e_to_big_e_4(const char *littleEndian){
    return littleEndian[0] | (littleEndian[1]<<8) | (littleEndian[2]<<16) | (littleEndian[3]<<24);
}

/// Returns the byte-inverted version of the input (2 bytes version)
char* lil_e_to_big_e_2(const char *littleEndian){
    return littleEndian[0] | (littleEndian[1] << 8);
}
