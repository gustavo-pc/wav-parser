#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uint8_t;

FILE *wavptr = NULL;
FILE *headerptr = NULL;

void writeHeader();
void readAndPrint();

int main() {
    wavptr = fopen("C:\\Users\\gustavo\\Developer\\SEMB\\wav-parser\\jump8.wav", "r");

    if (wavptr == NULL) {
        printf("Could not load the file\n");
        return 404;
    }

    ///Before running


    //char groupID[5];
    //groupID[4] = '\0';
    //unsigned char buffer;
    //int i = 0;

    //while (!feof(wavptr)){
    //    fread(&buffer, sizeof(buffer), 1, wavptr);
    //    printf("%d: %c\t", i++, buffer);
    //}

    //fseek(wavptr, 96, SEEK_SET);
    //fread(groupID, sizeof(groupID), 1, wavptr);
    //printf(groupID);

    //rewind(wavptr);

    /// End Before running


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
    fprintf(headerptr, "byte chNumber = 1;\nshort sampleRate = 8000;\nbyte resolution = 8;\nrom byte data[] = {");
}

void readAndPrint(){

    uint8_t buffer = 0;
    int i = 0;

    fseek(wavptr, 96 + 8, SEEK_SET);

    while(!(feof(wavptr))) {
        fread(&buffer, sizeof(uint8_t), 1, wavptr);
        //if(i >= 0 && i <= 40 ) printf("data[%2d]: %c 0x%02X %3d \n", i, buffer, buffer, buffer);
        fprintf(headerptr, "%d,", buffer);
        i++;
    }

    fseek(headerptr, -1, SEEK_CUR);
    fprintf(headerptr, "};\n");
    fprintf(headerptr, "short dataSize = %d;\n", i);

    printf("\nFunfou ae!!!\n");
}
