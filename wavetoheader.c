#include <stdio.h>
#include <stdlib.h>

FILE *wavptr = NULL;
FILE *headerptr = NULL;

void writeHeader();
void readAndPrint();

int main() {
    
    wavptr = fopen("/Users/gustavo/Developer/SEMB/wave-parser/jump8.wav", "r");
    
    if (wavptr == NULL) {
        printf("Could not load the file\n");
        return 404;
    }
    
    headerptr = fopen("test.h", "w");
    
    writeHeader();
    
    // Doin the magic
    readAndPrint();
    
    // Closing Filestreams
    fclose(wavptr);
    fclose(headerptr);
    
    return 0;
}

void writeHeader(){
    fprintf(headerptr, "typedef struct wavefile {\n\tuint8_t chNumber,\n\tunsigned sampleRate,\n\tuint8_t resolution,\n\tuint8_t *data,\n\tunsigned size\n} WaveFile;\n\n");
    fprintf(headerptr, "WaveFile jump8 = {0};\n\n");
    fprintf(headerptr, "jump8.chNumber = 1;\n");
    fprintf(headerptr, "jump8.sampleRate = 1;\n");
    fprintf(headerptr, "jump8.resolution = 1;\n");
}

void readAndPrint(){
    
    uint8_t buffer = 0;
    int i = 0;
    
    fseek(wavptr, 44, SEEK_SET);
    
    fprintf(headerptr, "jump8.data = [");
    while(!(feof(wavptr))) {
        fread(&buffer, sizeof(uint8_t), 1, wavptr);
        fprintf(headerptr, "%d,", buffer);
        i++;
    }
    fseek(headerptr, -1, SEEK_CUR);
    fprintf(headerptr, "];\n");
//    fprintf(headerptr, "\nint size = %d;", i);
    fprintf(headerptr, "jump8.size = %d;\n", i);
}