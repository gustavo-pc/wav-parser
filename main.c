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
    unsigned short wFormatTag;                  ///
    unsigned short wChannels;                   ///
    unsigned sampleRate;                        ///
    unsigned dwAvgBytesPerSec;                  ///
    unsigned short wBlockAlign;                 ///
    unsigned dwBitsPerSample;                   ///
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

WaveHeader header;
FILE *fileptr = NULL;

void fillHeader();

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

        fclose(fileptr);
    }

    return 0;

}

void fillHeader()
{
    char littleEndian[4];

    rewind(fileptr);

    fread(header.groupID, sizeof(header.groupID), 1, fileptr);

    fread(littleEndian, sizeof(header.fileLength), 1, fileptr);
    header.fileLength = littleEndian[0] | (littleEndian[1]<<8) | (littleEndian[2]<<16) | (littleEndian[3]<<24);

    fread(header.sRiffType, 4, 1, fileptr);
}
