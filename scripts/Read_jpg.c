#include <stdio.h>
#include <stdbool.h>
#define LENGTH(fp) do{ \
unsigned char b1; \
unsigned char b2; \
fread(&b1,1,1,fp); \
fread(&b2,1,1,fp); \
int length=(b1<<8)|b2; \
printf("length= %d\n",length); \
}while(0)

void read_marker(FILE *fp){
	unsigned char byte ;
	fread(&byte, 1, 1, fp);
	int length;
	unsigned char b1, b2;
	switch(byte){
		case 0xD8:
			printf("SOI(Start of image)\n");
			
			break;

       		case 0xE0:
       			printf("APPO(JFIF)\n");
       			LENGTH(fp);
       			break;
        
       		case 0xE1:
       			printf("APP1(EXIF)\n");
			LENGTH(fp);
       			break;
       			
       		case 0xDB:
       			printf("DQT(Define Quantization Table)\n");
			LENGTH(fp);
       			break;
       			
       		case 0xC0:
       			printf("SOF0(Start OF Frame)\n");
			LENGTH(fp);
       			break;
       		
       		case 0xC4:
       			printf("DHT(Define Huffman Table)\n");
			LENGTH(fp);
       			break;
       			
       		case 0xDA:
       			printf("SOS(Start of Scan)\n");
			LENGTH(fp);
       			break;
       			
       		case 0xD9:
       			printf("EOI(End Of Image)\n");
}}
int main(){
	bool t = false;
        FILE *fp;
        unsigned char buffer;
        size_t bytesRead;

        fp = fopen("/usr/share/backgrounds/canvas_by_roytanck.jpg","rb");
        if (fp == NULL){
                printf("Unable to open file.\n");
                return 1;
        }
       for (int i = 0; i < 30 ; i++) {
		fread(&buffer, 1, 1, fp);
		if(buffer == 0xFF){
			printf("\nmarker found\n");
			read_marker(fp);
		}
		printf("%X", buffer);
	}
	
        fclose(fp);
        printf("Image read successfully.\n");
        return 0;
}
