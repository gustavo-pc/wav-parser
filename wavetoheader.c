#include <stdio.h>
#include <stdlib.h>

FILE *wavptr = NULL;
FILE *headerptr = NULL;

void readAndPrint();

int main() {
    
    wavptr = fopen("/Users/gustavo/Developer/SEMB/jumpmono.wav", "r");
    
    if (wavptr == NULL) printf("Could not load the file\n");
    
    headerptr = fopen("test.h", "w");
    
    // Doin the magic
    readAndPrint();
    
    // Closing Filestreams
    fclose(wavptr);
    fclose(headerptr);
    
    return 0;
}

void readAndPrint(){
    
    uint8_t buffer = 0;
    int i = 0;
    
    fseek(wavptr, 44, SEEK_SET);
    
    fprintf(headerptr, "uint8_t *file = [");
    while(!(feof(wavptr))) {
        fread(&buffer, sizeof(uint8_t), 1, wavptr);
        fprintf(headerptr, "0x%02X ", buffer);
        //printf("Reading %02X\n", buffer);
        i++;
    }
    fseek(headerptr, -1, SEEK_CUR);
    fprintf(headerptr, "];");
    fprintf(headerptr, "\nint size = %d;", i);
}