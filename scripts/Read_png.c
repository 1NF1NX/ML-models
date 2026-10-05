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
#include <stdlib.h>
#include <zlib.h>

typedef struct IHDR {
	unsigned char signature[8];
	unsigned char length[4];
	unsigned char data[13];
	unsigned char crc[4];
}ihdr;

typedef struct IDAT {
	unsigned char length[4];
	unsigned char type[4];
	unsigned char *data;
	unsigned char crc[4];
	struct IDAT *next;
}idat;

typedef struct Pixel{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
}pix;

idat* Readpixel(FILE *fp) {
	bool c = false;
	idat *head;
	idat *ptr;

	while(1) {
		idat *chunk = (idat*)malloc(sizeof(idat));
		fread(chunk->length, 1, 4, fp);
		fread(chunk->type, 1, 4, fp);
			uint32_t val = ((uint32_t)chunk->length[0] << 24) |
				 ((uint32_t)chunk->length[1] << 16) |
				 ((uint32_t)chunk->length[2] << 8)  |
				 chunk->length[3];
		if(chunk->type[0] == 'I' && chunk->type[1] == 'D'
		   && chunk->type[2] == 'A' && chunk->type[3] == 'T') {
			chunk->data = malloc(val);
			fread(chunk->data, 1, val, fp);
			fread(chunk->crc, 1, 4, fp);
			chunk->next = NULL;
			if (c == false) {
				head = chunk;
				ptr = chunk;
				c = true;
				continue;
			}
			ptr->next = chunk;
			ptr = chunk;
		}
		else if(chunk->type[0] == 'I' && chunk->type[1] == 'E'
		   && chunk->type[2] == 'N' && chunk->type[3] == 'D')
			return head; //for now for testing
		else {
			fseek(fp, val + 4, SEEK_CUR);
			free(chunk);
		}
	}
}

int decompress(idat *chunk, ihdr p) {
	z_stream strm = {0};

	int ret = inflateInit(&strm);
	uint32_t width =
    ((uint32_t)p.data[0] << 24) |
    ((uint32_t)p.data[1] << 16) |
    ((uint32_t)p.data[2] << 8)  |
    p.data[3];
	uint32_t height =
    ((uint32_t)p.data[4] << 24) |
    ((uint32_t)p.data[5] << 16) |
    ((uint32_t)p.data[6] << 8)  |
    p.data[7];
uint8_t bit_depth = p.data[8];
uint8_t color_type = p.data[9];
int channel;

switch(color_type){
    case 0:
    //grayscale
    channels=1;
    break;

    case 2:
    //Truecolor RGB
    channel=3;
    break;

    case 3:
    //Indexed color
    channel=1;
    break;

    case 4:
    //Grayscale+alpha
    channel=2;
    break;

    case 6:
    //Truecolor+alpha
    channel=4;
    break;
}
printf("Width: %u\n", width);
printf("Height: %u\n", height);
printf("Bit depth: %u\n", bit_depth);
printf("Color type: %u\n", color_type);
	unsigned char output[height * (width * channel + 1)];
	while (chunk != NULL ) {
	uint32_t length =
    ((uint32_t)chunk->length[0] << 24) |
    ((uint32_t)chunk->length[1] << 16) |
    ((uint32_t)chunk->length[2] << 8)  |
    chunk->length[3];
	strm.next_in = chunk->data;
	strm.avail_in = length;
	strm.next_out = output + strm.total_out;
	strm.avail_out = sizeof(output) - strm.total_out;

	if (ret != Z_OK) {
        	printf("inflateInit failed\n");
        	return 1;
    	}
		
	    ret = inflate(&strm, Z_FINISH);
	    printf("avail_in: %u\n", strm.avail_in);
printf("avail_out: %u\n", strm.avail_out);
printf("total_out: %lu\n", strm.total_out);
	    printf("\ninflate returned: %d\n", ret);
	    printf("bytes produced: %lu\n",sizeof(output) - strm.avail_out);

	    chunk = chunk ->next;
	}
	    inflateEnd(&strm);
	//    printf("\nRAW OUTPUT:\n");

/*	for (int i = 0; i < 1000; i++) {
    	printf("%02X ", output[i]);
	}

	printf("\n");
	*/
	    pix *image = malloc(width * height * sizeof(pix));
	    for (uint32_t y = 0; y < height; y++) {

    	size_t row_start = y * (width * channel + 1);

    	unsigned char filter = output[row_start];

    	printf("Row %u, filter = %u\n", y, filter);

    	for (uint32_t x = 0; x < width; x++) {

        	size_t i = row_start + 1 + x * channel;
		size_t index = y * width + x;
		if(filter == 0) {
			image[index].r = output[i];
			image[index].g = output[i + 1];
			image[index].b = output[i + 2];
			image[index].a = output[i + 3];
		}

		else if(filter == 1) {
			if(x == 0) {
				image[index].r = output[i];
				image[index].g = output[i + 1];
				image[index].b = output[i + 2];
				image[index].a = output[i + 3];
			}
			else {
				image[index].r = output[i] + image[index-1].r;
				image[index].g = output[i + 1] + image[index-1].g;
				image[index].b = output[i + 2] + image[index-1].b;
				image[index].a = output[i + 3] + image[index-1].a;
			}
		}

		else if(filter == 2) {
			if(y == 0) {
				image[index].r = output[i];
				image[index].g = output[i + 1];
				image[index].b = output[i + 2];
				image[index].a = output[i + 3];
			}
			else {
				image[index].r = output[i] + image[index - width].r;
				image[index].g = output[i+1] + image[index - width].b;
				image[index].b = output[i+2] + image[index - width].g;
				image[index].a = output[i+3] + image[index - width].a;
			}
		}
        printf("(%u, %u, %u, %u) ", image[index].r, image[index].g, image[index].b,image[index].a);
    } 

    printf("\n");
}

	    return 0;
}

int main() {
	FILE *fp = fopen("cnn_rgb_example.png","rb");
	if (fp == NULL) {
		printf("Could not open file!");
		return 1;
	}
	
	ihdr p;
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

	idat* head = Readpixel(fp);
	decompress(head,p);
	fclose(fp);
	return 0;
}
