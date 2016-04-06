/**
    Parser de Arquivos WAV
    Autor: Gustavo Pinheiro Correia
    Data: Abril de 2016
*/

///Struct representing the header of a WAV file
typedef struct wave_header {
    char sGroupID[4];                           //"RIFF"
    unsigned dwFileLength;                      //
    char sRiffType[4];                          //"WAVE"
} WaveHeader;

///Struct for keeping the metadata of the file
typedef struct format_chunk {
    char sGroupID[4];                           //"fmt " - yes, with the whitespace
    unsigned dwChunkSize;                       //
    unsigned short wFormatTag;                  //
    unsigned short wChannels;                   //
    unsigned dwSamplesPerSec;                   //
    unsigned dwAvgBytesPerSec;                  //
    unsigned short wBlockAlign;                 //
    unsigned dwBitsPerSample;                   //
} FormatChunk;

///Struct for containing the raw audio samples and some metadata
typedef struct data_chunk {
    char sGroupID[4];
    unsigned dwChunkSize;
    void *sampleData;       //byte/char for 8-bit, short for 16-bit, float for 32-bit
} DataChunk;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fileptr = NULL;

int main(int argc, char** argv)
{
    //Checking given filepath
    if (argc != 2) {
        char *progname = strrchr(argv[0], '\\');
        //printf("Uso correto: %s \"nomedoarquivo\"\n", &progname[1]);

        int hs = sizeof(WaveHeader), fs = sizeof(FormatChunk);
        printf("Tamanho do header:  %d bytes\n", hs);
        printf("Tamanho do fchunk:  %d bytes\n", fs);
        printf("Total            :  %d bytes\n", hs+fs);

        return 0;
    } else {
        printf("Your file is %s\n", argv[1]);
        fileptr = fopen(argv[1], "r");
        fclose(fileptr);
    }

    return 0;
}
