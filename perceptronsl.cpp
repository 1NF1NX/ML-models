#include <iostream>
using namespace std;

class Perceptron {
private:
	double w1 = 0;
	double w2 = 0;
	double bias = 0;
	const double lr = 0.1;

public:
	void train(double x1,double x2,double target) { 
		int output = pred(x1,x2);
		int error = target - output;
	       	w1 = w1 + lr * error * x1; 
		w2 = w2 + lr * error * x2; 
		bias = bias + lr * error;
	}
	int pred(double x1, double x2) {
		float z = x1*w1 + x2*w2 + bias;
	//	cout << "Activation: " << z << endl;
			return (z > 0);
	}
};

int main() {
	double x1[] = {0.233,0.355,0.247,0.343,0.46, 0.74, 0.932};
	double x2[] = {0.342,0.934,0.424,0.52,0.6,0.932,0.854};
	double target[] = {0,1,0,0,0,1,1};
	Perceptron perc;
	for(int epoch = 0; epoch < 500; epoch++){
		int counter = 0;
		for(int i = 0; i < 7; i++){
			int op = perc.pred(x1[i],x2[i]);
			if(op!=target[i]) {
				perc.train(x1[i],x2[i],target[i]);
			}
			else 
				counter++;

			printf("Output  %d\n",op);
		}
		if(counter == 7)
			break;
	}
	return 0;
}
