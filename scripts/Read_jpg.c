#include <stdio.h>
#include <stdbool.h>

#define LENGTH(fp) do{ \
unsigned char b1; \
unsigned char b2; \
fread(&b1,1,1,fp); \
fread(&b2,1,1,fp); \
length=(b1<<8)|b2; \
printf("length = %d\n",length); \
}while(0)
typedef struct {
unsigned int segment_length;
unsigned char precision;
unsigned char height[2];
unsigned char width[2];
}FI;
void frame(FILE* fp, int length) {
	FI image;
	image.segment_length = length;
	fread(&image.precision,1,1,fp);
	fread(&image.height,1,2,fp);
	fread(&image.width,1,2,fp);
}
bool read_marker(FILE *fp){
	unsigned char byte;
	fread(&byte, 1, 1, fp);
	printf("%X\n", byte);
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
			frame(fp,length);
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
			return true;
	}
	return false;
}
int main(){
	bool t = false;
        FILE *fp;
        unsigned char buffer;
        size_t bytesRead;
	bool end = false;
        fp = fopen("/usr/share/backgrounds/canvas_by_roytanck.jpg","rb");
        if (fp == NULL){
                printf("Unable to open file.\n");
                return 1;
        }
       for (;;) {
		fread(&buffer, 1, 1, fp);
		printf("%X", buffer);
		if(buffer == 0xFF){
			printf("\nmarker found\n");
			end = read_marker(fp);
			if(end == true) {
				break;
			}
		}
	}
	
        fclose(fp);
        printf("Image read successfully.\n");
        return 0;
}
