#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_QTABLES 4

typedef struct {
    uint16_t values[MAX_QTABLES][64];
    bool defined[MAX_QTABLES];
} QuantTables;

typedef struct {
     unsigned char component_id;
     unsigned char sampling_factor;
     unsigned char qt_id;
} Component;

typedef struct{
      unsigned char precision;
      uint16_t width;
      uint16_t height;
      unsigned char num_components;
      Component components[4];
} FrameInfo;

bool read_sof0(FILE *fp,int length,FrameInfo *frame)
{
      if(length < 8)
         return false;

      unsigned char h1,h2,w1,w2;

      if(fread(&frame ->precision,1,1,fp) !=1)
        return false;

      if(fread(&h1, 1, 1, fp) != 1 ||
         fread(&h2, 1, 1, fp) != 1 ||
         fread(&w1, 1, 1, fp) != 1 ||
         fread(&w2, 1, 1, fp) != 1)
         return false;

      frame->height = ((uint16_t)h1 <<8) | h2;
      frame->width =((uint16_t)w1 << 8) | w2;

      if (fread(&frame->num_components, 1, 1, fp) != 1)
         return false;

      if (frame->num_components > 4 ||
          length != 8+3 * frame->num_components)
          return false;
     
      for(int i = 0; i < frame->num_components; i++)
      {
          Component *c = &frame->components[i];

          if(fread(&c->component_id, 1, 1,fp) != 1 ||
             fread(&c->sampling_factor, 1, 1, fp) != 1 ||
             fread(&c->qt_id, 1, 1, fp) != 1)
             return false;
       }
       
       printf("\nSOF0 Information\n");
       printf("Precision:%u\n",frame->precision);
       printf("Width: %u\n", frame->width);
       printf("Height: %u\n", frame->height);
       printf("Components: %u\n", frame->num_components);

       for(int i=0; i<frame->num_components;i++)
       {
           printf("\nComponents %d\n",i+1);
           printf("Component ID :%u\n",frame->components[i].component_id);
           printf("Sampling factor: %X\n",frame->components[i].sampling_factor);
           printf("Quantization table ID: %u\n",frame->components[i].qt_id);
        }
        return true;
 }


bool read_dqt (FILE *fp, int length, QuantTables *qt) {
	int remaining = length - 2;
	
	while (remaining > 0) {
		unsigned char info;
		
		if (fread(&info, 1, 1, fp) != 1)
			return false;
			
       		remaining --;
       		
        	int precision = info >> 4;
        	int table_id = info & 0x0F;
        	
        	if (table_id >= MAX_QTABLES || precision > 1) {
            		printf("Invalid quantization table\n");
            		return false;
        	}
        	
        	int bytes_per_value = (precision == 0) ? 1 : 2;

        	if (remaining < 64 * bytes_per_value) {
            		printf("Invalid DQT segment length\n");
            		return false;
        	}
        	
        	for (int i = 0; i < 64; i++) {
            		unsigned char b1, b2 = 0;
            		
            		if (fread(&b1, 1, 1, fp) != 1)
               			return false;
               			
          		uint16_t value;
          		
        		if(precision == 0){
        			value = b1;
       	 		} else {
            			if (fread(&b2, 1, 1, fp) != 1)
            			return false;
            		value = ((uint16_t)b1 << 8) | b2;
        	}
        	
        	qt->values[table_id][i] = value;
        }
        
        remaining -= 64 * bytes_per_value;
        qt->defined[table_id] = true;
        
        printf("Quantization Table %d:\n", table_id);
        
        for (int i = 0; i <64; i++) {
        	printf("%u",(unsigned int)qt->values[table_id][i]);
        	if((i+1) % 8 == 0)
            		printf("\n");
        }
    }
    
    return true;
}

#define LENGTH(fp) do{ \
unsigned char b1; \
unsigned char b2; \
fread(&b1,1,1,fp); \
fread(&b2,1,1,fp); \
length=(b1<<8)|b2; \
printf("length = %d\n",length); \
}while(0)

bool read_marker(FILE *fp, QuantTables *qt,FrameInfo *frame){
	unsigned char byte;
	
	fread(&byte, 1, 1, fp);
	printf("%X\n", byte);
	
	int length;
	unsigned char b1, b2;
	
	switch(byte){
		case 0xD8:
			printf("SOI (Start of image)\n");
			break;

       		case 0xE0:
       			printf("APPO (JFIF)\n");
       			LENGTH(fp);
       			break;
        
       		case 0xE1:
       			printf("APP1 (EXIF)\n");
			LENGTH(fp);
       			break;
       			
      	 	case 0xDB:
    			printf("DQT (Define Quantization Table)\n");
    			LENGTH(fp);

    			if (read_dqt(fp, length, qt)) {
        			printf("DQT table read successfully!\n");
    			} else {
        			printf("DQT table reading failed!\n");
        			return false;
    			}
    			break;
       			
     	  	case 0xC0:
       			printf("SOF0 (Start OF Frame)\n");
			LENGTH(fp);

                        if(!read_sof0(fp,length,frame))
                           return false;
       			break;

       		
      	 	case 0xC4:
       			printf("DHT (Define Huffman Table)\n");
			LENGTH(fp);
       			break;
       			
       		case 0xDA:
       			printf("SOS (Start of Scan)\n");
			LENGTH(fp);
       			break;
       			
       		case 0xD9:
       			printf("EOI (End Of Image)\n");
			return true;
	}
	return false;
}
int main(){
        QuantTables qt = {0};
        FrameInfo frame ={0};
	bool t = false;
        FILE *fp;
        unsigned char buffer;
        size_t bytesRead;
	bool end = false;
        fp = fopen("jpeg_parser_test.jpg","rb");
        if (fp == NULL){
                printf("Unable to open file.\n");
                return 1;
        }
        for (;;) {
		fread(&buffer, 1, 1, fp);
		printf("%X", buffer);
		if(buffer == 0xFF){
			printf("\nmarker found\n");
			end = read_marker(fp,&qt,&frame);
			if(end == true) {
				break;
			}
		}
	}
	for (int i = 0; i < MAX_QTABLES; i++) {
		if (qt.defined[i]) {
			printf("Table %d is stored.\n", i);
		} else {
			printf("Table %d is not defined.\n", i);
		}
	}
        fclose(fp);
        printf("Image read successfully.\n");
        return 0;
}
