#include <stdio.h>
#include <stdbool.h>


void read_marker(FILE *fp){
	unsigned char byte = fread(&byte, 1, 1, fp);
	int length;
	unsigned char b1, b2;

	switch(byte){
		case 0xD8:
			printf("SOI(Start of image)\n");
			break;

       	case 0xE0:
       		printf("APPO(JFIF)\n");
       		break;
        
       	case 0xE1:
       		printf("APP1(EXIF)\n");
       		break;
       			
       	case 0xDB:
       		printf("DQT(Define Quantization Table)\n");
       		break;
       			
       	case 0xC0:
       		printf("SOF0(Start OF Frame)\n");
       		break;
       		
       	case 0xC4:
       		printf("DHT(Define Huffman Table)\n");
       		break;
       			
       	case 0xDA:
       		printf("SOS(Start of Scan)\n");
       		break;
       			
       	case 0xD9:
       		printf("EOI(End Of Image)\n");
       		break;
	}
}
   
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
	}
	printf("%X", buffer);
	}

    fclose(fp);
    printf("Image read successfully.\n");
    return 0;
}