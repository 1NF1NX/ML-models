#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_QTABLES 4

typedef struct{
    uint16_t values[MAX_QTABLES][64];
    bool defined[MAX_QTABLES];
}QuantTables;

void read_dqt(FILE *fp,int length,QuantTables *qt){
int remaining = length-2;
while(remaining > 0){
	unsigned char info;
	if(fread(&info,1,1,fp) !=1)
		return;
        remaining --;
        int precision = info >> 4;
        int table_id = info & 0x0F;
        if(table_id >=MAX_QTABLES || precision > 1){
            printf("Invalid quantization table\n");
            return;
        }
        for (int i=0;i<64;i++){
            unsigned char b1,b2 =0;
            if(fread(&b1,1,1,fp) != 1)
               return;
          uint16_t value;
        if(precision == 0){
        	value = b1;
        	remaining--;
        }
        else{
            if(fread(&b2,1,1,fp)!=1)
            	return;
            value = ((uint16_t)b1 << 8) | b2;
            remaining -=2;
        }
        qt->values[table_id][i] = value;
        }
        qt->defined[table_id] = true;
        printf("Quantization Table %d:\n", table_id);
        for(int i=0; i<64; i++){
        	printf("%u",qt->values[table_id][i]);
            if((i+1)%8 ==0)
            	printf("\n");
        }
    }
}

#define LENGTH(fp) do{ \
unsigned char b1; \
unsigned char b2; \
fread(&b1,1,1,fp); \
fread(&b2,1,1,fp); \
length=(b1<<8)|b2; \
printf("length = %d\n",length); \
}while(0)

bool read_marker(FILE *fp, QuantTables *qt){
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
            read_dqt(fp, length, qt);
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
			return true;
	}
	return false;
}
int main(){
        QuantTables qt = {0};
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
			end = read_marker(fp,&qt);
			if(end == true) {
				break;
			}
		}
	}
	
        fclose(fp);
        printf("Image read successfully.\n");
        return 0;
}


