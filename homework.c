#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


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
image* readInput(const char * fileName) {
	FILE* f;
	f = fopen(fileName,"r");
	if(!f)
	{
		exit(1);
	}
	image* img = (image*)malloc(sizeof(image));
	char version[3];
	fgets(img->version,sizeof(img->version),f);
	fscanf(f,"%d",&img->width);
	fscanf(f,"%d",&img->height);
	fscanf(f,"%d",&img->maxValue);
	fscanf(f," ");
	
	img->data = (unsigned char**)malloc(img->height* sizeof(unsigned char*));
	for(int i = 0;i < img->height;i ++)
	{
		img->data[i] =(unsigned char*)malloc(3 * img->width * sizeof(unsigned char));
	}
	
	if(!strcmp(img->version,"P5"))
	{
		for(int i = 0;i < img->height;i ++)
		{
			for(int j = 0;j < 3 * img->width;j +=3)
			{
				
					fread(&img->data[i][j],sizeof(unsigned char),1,f);
				
		
			}
		}
	}
	else
	{
		for(int i = 0;i < img->height;i ++)
		{
			fread(img->data[i],sizeof(unsigned char),3 * img->width,f);
		}
	}
	
	return img;
	
	fclose(f);
}

void writeData(const char * fileName, image *img) {
	FILE* f;
	f = fopen(fileName,"w");
	if(!f)
	{	
		exit(1);
	}

	
	fprintf(f,"%s\n",img->version);
	fprintf(f,"%d %d\n",img->width,img->height);
	fprintf(f,"%d\n",img->maxValue);
	if(!strcmp(img->version,"P5"))
	{
		for(int i = 0;i < img->height;i ++)
		{
			for(int j = 0;j < 3 * img->width;j +=3)
			{
				
					fwrite(&img->data[i][j],sizeof(unsigned char),1,f);
				
		
			}
		}
	}
	else
	{
		for(int i = 0;i < img->height;i ++)
		{
			fwrite(img->data[i],sizeof(unsigned char),3 * img->width,f);
		}
	}
	
	fclose(f);
}
int main(int argc,char *argv[])
{
	int rank;
	int nProcesses;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&nProcesses);
	image *in;
	image *out;
	image *aux;
	image *temp;	
	char version[3];
	int width;
	int height;
	int maxValue;
	float suma = 0;
	if(rank == 0)
	{	
		in = readInput(argv[1]);
	}
	
	
		
	
	float matrix[3][3];
	for(int num = 3;num < argc;num ++)
	{
								if(!strcmp(argv[num],"smooth"))
								{
									for(int i = 0;i < 3;i ++)	
									{
										for(int j = 0;j < 3;j ++)
										{	
											matrix[i][j] = smooth[i][j];
										}	
									}			
								}
								else if(!strcmp(argv[num],"blur"))
								{
									for(int i = 0;i < 3;i ++)	
									{
										for(int j = 0;j < 3;j ++)
										{	
											matrix[i][j] = blur[i][j];
										}	
									}
								}
								else if(!strcmp(argv[num],"sharpen"))
								{
									for(int i = 0;i < 3;i ++)	
									{
										for(int j = 0;j < 3;j ++)
										{	
											matrix[i][j] = sharpen[i][j];
										}	
									}
								}
								else if(!strcmp(argv[num],"mean"))
								{
									for(int i = 0;i < 3;i ++)	
									{
										for(int j = 0;j < 3;j ++)
										{	
											matrix[i][j] = mean[i][j];
										}	
									}
								}
								else if(!strcmp(argv[num],"emboss"))
								{
									for(int i = 0;i < 3;i ++)	
									{
										for(int j = 0;j < 3;j ++)
										{	
											matrix[i][j] = emboss[i][j];
										}	
									}
								}
	if(rank == 0)
	{	
		if(num == 3)
		{
			
			out =(image*)malloc(sizeof(image));
			out->data =(unsigned char**)malloc(in->height * sizeof(unsigned char*));
			for(int i = 0;i < in->height;i ++)
			{
				out->data[i] = (unsigned char*)malloc(3 * in->width * sizeof(unsigned char));
			}
			strcpy(out->version,in->version);	
			out->width = in->width;
			out->height = in->height;
			out->maxValue =in->maxValue;
			
		
		for(int i = 1;i < nProcesses;i ++)
		{
			MPI_Send(in->version,3,MPI_CHAR,i,0,MPI_COMM_WORLD);
			MPI_Send(&in->width,1,MPI_INT,i,0,MPI_COMM_WORLD);
			MPI_Send(&in->height,1,MPI_INT,i,0,MPI_COMM_WORLD);
			MPI_Send(&in->maxValue,1,MPI_INT,i,0,MPI_COMM_WORLD);
		}
		}
		for(int i = 0;i < in->height;i ++)
			{	
				for(int j = 0;j < 3 * in->width;j ++)
				{
					out->data[i][j] = in->data[i][j];
				}
			}
		for(int i = 1;i < nProcesses;i ++)
		{
			if(i != nProcesses - 1)
			{	MPI_Send(in->data[(i * in->height/nProcesses) - 1],3 * in->width,MPI_UNSIGNED_CHAR,i,0,MPI_COMM_WORLD);
				for(int j = i * in->height/nProcesses;j < (i + 1) * in->height/nProcesses;j ++)
				{
					MPI_Send(in->data[j],3 * in->width,MPI_UNSIGNED_CHAR,i,0,MPI_COMM_WORLD);		
				}
				MPI_Send(in->data[(i + 1) * in->height/nProcesses],3 * in->width,MPI_UNSIGNED_CHAR,i,0,MPI_COMM_WORLD);
			}
			else
			{
				MPI_Send(in->data[(i * in->height/nProcesses) - 1],3 * in->width,MPI_UNSIGNED_CHAR,i,0,MPI_COMM_WORLD);
				for(int j = i * in->height/nProcesses;j < in->height;j++)
				{
					MPI_Send(in->data[j],3 * in->width,MPI_UNSIGNED_CHAR,i,0,MPI_COMM_WORLD);
				}
			}
		}
	}	
	
		if(rank == 0)
		{				
			
			int high = in->height/nProcesses;
			if(nProcesses == 1)
			{
				high --;
			}
			for(int i = 1;i < high;i ++)
			{
				for(int j = 3;j < 3 * (in->width - 1);j ++)
				{	suma = 0;
					
						for(int k = i - 1;k <= i + 1;k ++)
						{
							for(int l = j - 3;l <= j + 3;l+=3)
							{
								suma += matrix[(k - i + 1)][(l - j + 3)/3] * in->data[k][l];
							}
						}
					
					out->data[i][j] = suma;
				}
			} 
		
			for(int i = 1;i < nProcesses;i ++)
			{
				if(i != nProcesses - 1)
				{	
					for(int j = i * in->height/nProcesses;j < (i + 1) * in->height/nProcesses;j ++)
					{
						MPI_Recv(out->data[j],3 * in->width,MPI_UNSIGNED_CHAR,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);		
					}
					
				}
				else
				{	
					for(int j = i * in->height/nProcesses;j < in->height - 1;j++)
					{	
						MPI_Recv(out->data[j],3 * in->width,MPI_UNSIGNED_CHAR,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);	
					}
				}
			}
			if(num == argc - 1)
			{
				writeData(argv[2],out);
			}
			in = out;
		}
		else
		{	
			
			if(num == 3)
			{
			aux =(image*)malloc(sizeof(image));
			MPI_Recv(aux->version,3,MPI_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			MPI_Recv(&aux->width,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			MPI_Recv(&aux->height,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			MPI_Recv(&aux->maxValue,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			
			aux->data = (unsigned char**)malloc(aux->height* sizeof(unsigned char*));
			for(int i = 0;i < aux->height;i ++)
			{
				aux->data[i] =(unsigned char*)malloc(3 * aux->width * sizeof(unsigned char));
			}
			
			temp =(image*)malloc(sizeof(image));
			temp->data = (unsigned char**)malloc( aux->height* sizeof(unsigned char*));
			for(int i = 0;i <  aux->height;i ++)
			{
				temp->data[i] =(unsigned char*)malloc(3 * aux->width * sizeof(unsigned char));
			}
			strcpy(temp->version,aux->version);
			temp->width = aux->width;
			temp->height = aux->height;
			temp->maxValue = aux->maxValue;
			}	
			if(rank != nProcesses - 1)
			{	MPI_Recv(aux->data[(rank * aux->height/nProcesses) - 1],3 * aux->width,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				for(int j = rank * aux->height/nProcesses;j < (rank + 1) * aux->height/nProcesses;j ++)
				{
					MPI_Recv(aux->data[j],3 * aux->width,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);	
					for(int k = 0;k < 3;k ++)
					{
						temp->data[j][k] = aux->data[j][k];
					}
					for(int k = 3 * (aux->width - 1);k < 3 * aux->width;k ++)
					{
						temp->data[j][k] = aux->data[j][k];
					}	
				}
				
				MPI_Recv(aux->data[(rank + 1) * aux->height/nProcesses],3 * aux->width,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);	
				for(int i = rank * aux->height/nProcesses;i < (rank + 1) * aux->height/nProcesses ;i ++)
				{
					for(int j = 3;j < 3 * (aux->width - 1);j ++)
					{	suma = 0;
					
							for(int k = i - 1;k <= i + 1;k ++)
							{
								for(int l = j - 3;l <= j + 3;l+=3)
								{
								
									suma += matrix[(k - i + 1)][(l - j + 3)/3] * aux->data[k][l];
								
								}
							}
					
						temp->data[i][j] = suma;
					}
				}
				for(int j = rank * aux->height/nProcesses;j < (rank + 1) * aux->height/nProcesses;j ++)
				{
					MPI_Send(temp->data[j],3 * aux->width,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD);		
				}	 
			}
			else
			{
				MPI_Recv(aux->data[(rank * aux->height/nProcesses) - 1],3 * aux->width,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				for(int i = rank * aux->height/nProcesses;i < aux->height;i++)
				{
					MPI_Recv(aux->data[i],3 * aux->width,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
					for(int k = 0;k < 3;k ++)
					{
						temp->data[i][k] = aux->data[i][k];
					}
					for(int k = 3 * (aux->width - 1);k < 3 * aux->width;k ++)
					{
						temp->data[i][k] = aux->data[i][k];
					}	
				}
				
				for(int i = rank * aux->height/nProcesses;i < aux->height - 1;i ++)
				{
					for(int j = 3;j < 3 * (aux->width - 1);j ++)
					{	suma = 0;
					
						for(int k = i - 1;k <= i + 1;k ++)
						{
							for(int l = j - 3;l <= j + 3;l+=3)
							{
								suma += matrix[(k - i + 1)][(l - j + 3)/3] * aux->data[k][l];
							}
						}
					
					temp->data[i][j] = suma;
					}
				}	 
				for(int j = rank * aux->height/nProcesses;j < aux->height - 1;j ++)
				{
					MPI_Send(temp->data[j],3 * aux->width,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD);		
				}
						
			}
		
			
		}
	}
	MPI_Finalize();
	return 0;
}
