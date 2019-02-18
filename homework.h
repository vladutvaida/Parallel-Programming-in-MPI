
#ifndef HOMEWORK_H
#define HOMEWORK_H


typedef struct {	
	char version[3];
	int width;
	int height;
	int maxValue;
	unsigned char** data;
}image;

float smooth[3][3] = {{((float)1)/9,((float)1)/9,((float)1)/9},{((float)1)/9,((float)1)/9,((float)1)/9},{((float)1)/9,((float)1)/9,((float)1)/9}};
float blur[3][3] = {{((float)1)/16,((float)2)/16,((float)1)/16},{((float)2)/16,((float)4)/16,((float)2)/16},{((float)1)/16,((float)2)/16,((float)1)/16}};
float sharpen[3][3] = {{((float)0)/3,((float)-2)/3,((float)0)/3},{((float)-2)/3,((float)11)/3,((float)-2)/3},{((float)0)/3,((float)-2)/3,((float)0)/3}};
float mean[3][3] = {{-1,-1,-1},{-1,9,-1},{-1,-1,-1}};
float emboss[3][3] = {{0,1,0},{0,0,0},{0,-1,0}};
image* readInput(const char * fileName);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */
