#include <iostream>
using namespace std; 

/*
void pooling() {

}
*/
int main() {
	int input[7][7]={
		{0,0,1,1,1,1,0},
		{0,0,0,0,0,1,0},
		{0,0,0,0,1,0,0},
		{0,0,0,1,0,0,0},
		{0,0,0,1,0,0,0},
		{0,0,0,1,0,0,0},
		{0,0,0,0,1,0,0},
	};

	int filter[3][3] = {
        	{1, 0, -1},
        	{1, 0, -1},
        	{1, 0, -1}
	    };
	int output[5][5] = {0};

	for (int i = 0; i < 5; i++) {
        	for (int j = 0; j < 5; j++) {
	
            		int sum = 0;
			for(int ki = 0; ki < 3; ki++) {
				for(int kj = 0; kj < 3; kj++) {
					sum += input[i+ki][j+kj]*filter[ki][kj];
				}
			}
			output[i][j] = sum;
		}
	}

	 cout << "Convolution Output:\n";

    	for (int i = 0; i < 5; i++) {
        	for (int j = 0; j < 5; j++) {
        	    	cout << output[i][j] << " ";
        	}
        	cout << '\n';
    	}
	printf("\n=====ReLu=====\n");
	for (int i = 0; i < 5; i++) {
        	for (int j = 0; j < 5; j++) {
            		if(output[i][j] < 0) {
				output[i][j] = 0;
			}
			cout << output[i][j] << " ";
        	}
         	cout << '\n';
    	}
	int poolsum = 0;
	cout << "\n=====Max_Pooling=====\n" << endl;
/*	for(int i = 0; i < n; i++) {
		for(int j = 0; j < n; j++) {	 
		int pooling[2][2];
			
		for(int si = 0; si < 2; si++) {	 
			for(int sj = 0; sj < 2; sj++) {
				poolsum = output[i+si][j+sj];	
				pooling[si][sj] = output[i+si]
			}
		}
		}
	} */
    return 0;
}
