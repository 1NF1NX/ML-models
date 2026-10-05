#include <stdio.h>

int main(){
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
		      printf("%X", buffer);
	      }

        fclose(fp);
        printf("Image read successfully.\n");
        return 0;
}
