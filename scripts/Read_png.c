/*
 * Structure of a PNG image
 

 first 33 bytes
┌───────────────┬───────────────┐
│ Offset        │ Meaning       │
├───────────────┼───────────────┤
│ 00 - 07       │ PNG signature │
│ 08 - 0B       │ IHDR length   │
│ 0C - 0F       │ "IHDR"        │
│ 10 - 1C       │ IHDR data     │
│ 1D - 20       │ IHDR CRC      │
└───────────────┴───────────────┘

after 33 bytes
┌──────────────┐
│ 4 bytes      │ length
├──────────────┤
│ 4 bytes      │ "IDAT"
├──────────────┤
│ N bytes      │ compressed data
├──────────────┤
│ 4 bytes      │ CRC
└──────────────┘

*/
#include <stdio.h>
#include <stdint.h>

typedef struct IDRH {
	unsigned char signature[8];
	unsigned char length[4];
	unsigned char data[13];
	unsigned char crc[4];
}idhr;

typedef struct IDAT {
	unsigned char length[4];
	unsigned char type[4];
	unsigned char *data;
	unsigned char crc[4];
}idat;

void Readpixel(FILE *fp) {
	idat chunk;
	fread(chunk.length, 1, 4, fp);
	fread(chunk.type, 1, 4, fp);
	uint32_t val = ((uint32_t)chunk.length[0] << 24) |
		 ((uint32_t)chunk.length[1] << 16) |
   		 ((uint32_t)chunk.length[2] << 8)  |
		 chunk.length[3];
}

int main() {
	FILE *fp = fopen("/home/ashansud/Downloads/cnn_grayscale_example.png","rb");
	if (fp == NULL) {
		printf("Could not open file!");
		return 1;
	}
	
	idhr p;
	unsigned char byte;
	for (int i = 0; i < 33; i++) {
		fread(&byte, 1, 1, fp);
		if (i < 8)
			p.signature[i] = byte;
		else if (i >= 8 && i < 12) 
			p.length[i-8] = byte;
		
		else if(i >= 16 && i < 29) 
			p.data[i-16] = byte;
		else if(i >= 29 && i < 33)
		       p.crc[i-29] = byte;
		printf("%X ", byte);
	}

	for(int i = 33; i < 100; i++) {
		
	}
	fclose(fp);
	return 0;
}
