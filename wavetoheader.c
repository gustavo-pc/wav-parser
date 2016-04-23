#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uint8_t;

FILE *wavptr = NULL;
FILE *headerptr = NULL;

void writeHeader();
void readAndPrint();

int main() {

    wavptr = fopen("C:\\Users\\Bruno Pessoa\\Desktop\\parser\\wav-parser\\jump8.wav", "r");

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

void writeHeader(){
    fprintf(headerptr, "typedef unsigned char byte;\n");
    fprintf(headerptr, "byte chNumber = 1;\nshort sampleRate = 8000;\nbyte resolution = 8;\nbyte data[] = {");
}

void readAndPrint(){

    uint8_t buffer = 0;
    int i = 0;

    fseek(wavptr, 44, SEEK_SET);

    while(!(feof(wavptr))) {
        fread(&buffer, sizeof(uint8_t), 1, wavptr);
        fprintf(headerptr, "%d,", buffer);
        i++;
    }

    fseek(headerptr, -1, SEEK_CUR);
    fprintf(headerptr, "};\n");
    fprintf(headerptr, "short dataSize = %d;\n", i);
}
