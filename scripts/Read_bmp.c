/*
  * structure of BMP 
*/

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>

typedef struct BMPHeader{
    unsigned char signature[2];
    unsigned char file_size[4];
    unsigned char reserved[4];
    unsigned char pixel_data_offset[4];
}bmpheader;

typedef struct DIB{
    unsigned char header_size[4];
    unsigned char width[4];
    unsigned char height[4];
    unsigned char colour_planes[2];
    unsigned char bits_per_pixel[2];
    unsigned char compression [4];
}dib;

typedef struct Pixeldata{
    unsigned char *data;
}pixeldata;

void Readpixel(FILE *fp){
    bmpheader header;
    dib information ;
        fread(&header,1,14,fp);
        fread(&information,1,20,fp);
        
        int width=((int)information.width[0]) |
                  ((int)information.width[1]<< 8) |
                  ((int)information.width[2]<< 16)|
                  ((int)information.width[3]<< 24);

        int height=((int)information.height[0])|
                   ((int)information.height[1]<<8)|
                   ((int)information.height[2]<<16)|
                   ((int)information.height[3]<<24);

        int bits = ((int)information.bits_per_pixel[0])|
                   ((int)information.bits_per_pixel[1]<<8);

        int offset =((int)header.pixel_data_offset[0]) |
                    ((int)header.pixel_data_offset[1]<<8)|
                    ((int)header.pixel_data_offset[2]<<16)|
                    ((int)header.pixel_data_offset[3]<<24);

                printf("width= %d\n",width);
                printf("height= %d\n",height);
                printf("pixel offset = %d\n", offset);

                fseek(fp,offset,SEEK_SET);

                int row_size = ((width *3 +3)/4)*4;

               unsigned char **matrix;
               matrix = malloc(height*sizeof(unsigned char *));

               for(int i=0; i<height ; i++){
                matrix[i]= malloc(width*sizeof(unsigned char));
               }
               pixeldata pixel;
               pixel.data = malloc(row_size);

               for(int i=0 ; i<height ; i++){
                fread(pixel.data,1,row_size,fp);
               

               for(int j=0; j<width ; j++)
               {
                unsigned char blue;
                unsigned char green;
                unsigned char red;

                blue = pixel.data[j * 3];
                green = pixel.data[j * 3 +1];
                red = pixel.data[j* 3 + 2];

                unsigned char gray;
                gray=(unsigned char)0.299*red+ 0.587*green + 0.114*blue;

                matrix[ height-1-i][j]= gray ;
               }
            }
               for(int i=0; i<height ; i++)
               {
                for (int j=0 ; j<width ; j++){
                    printf("%3d " , matrix[i][j]);
                }
                printf("\n");
               }
               free(pixel.data);
               for(int i = 0; i<height; i++)
               {
                free (matrix[i]);
               }
               free(matrix);

        }
        int main()
        {
            FILE *fp;
            fp = fopen("image.bmp","rb");

            if (fp == NULL)
            {
                printf("unable to open bmp file \n");
                return 1;
            }
            Readpixel(fp);
            fclose(fp);
            return 0;
        }    
