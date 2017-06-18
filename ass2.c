/**************************************************************************************
Filename:          ass2.c
Version:           1.0
Author:            Tim Osse
Student No:        040585009
Lab Sect:          310
Assignment #:      2
Assignment name:   CST8233 Assignment 2
Due Date:          November 11, 2012
Submission Date:   November 11, 2012
Professor:         Andrew Tyler
Purpose:           Fit data using linear regression least squares.
					Fit to a straight line and a power law function.
**************************************************************************************/
#include <math.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

int numPts = 0;
float m, c, c2, sigm, sigc, sigma;
char* fileName;//file name to open
FILE* fp;//file pointer
int running = 1;
char response;
float offSetX = 1960;//adjusts year to a value relative to 1960
float offSetY = 316.5F;//adjust Co2 value relative to what it was in 1960
float* x;//array of year values
float* y;//array of Co2 values
float* x_log;//array of log'ed Co2 values
float* y_log;//array of log'ed year values

/********************************************************************
Function name:	main
Purpose:		Program driver
In parameters:	void
Out parameters: int
Version:		1.0
Author:		    Tim Osse
*****************************e*****************************************/
int main(void){

	while(running == 1){

		printf("\n***********************************************************\n");
		printf("Linear Regression of Data - Function Type Menu\n");
		printf("1. Linear:	y = m*x + c\n");
		printf("2. Power Law:	y = m*x^c\n");
		printf("3. Quit\n");
		printf("\n***********************************************************\n");
		printf("Select an option: ");
		fflush(stdin);
		scanf("%c",&response);

		switch(response)
		{
		case'1':
			ReadFile();
			linear();//extrap menu in linear func
			break;
		case '2':
			ReadFile();
			powerLaw();//extrap menu in power func
			break;
		case '3':
			running = 0;
			break;
		default: printf("Invalid choice\n");
		}

	}
	free(x);
	free(y);
	free(x_log);
	free(y_log);
	return 0;
}

/********************************************************************
Function name:	ReadFile
Purpose:		read Co2 records from file into memory
In parameters:	none
Out parameters: void
Version:		1.0
Author:		    Tim Osse
**********************************************************************/
//Read from selected file and store in memory
void ReadFile(void){
	
	float temp_x = 0.0f;
	float temp_y = 0.0f;
	int i,j = 0;
	int k;
	fileName = (char*)malloc(sizeof(char)*25);
	printf("\nPlease enter the name of the file to open: ");
	scanf("%s",fileName);

	fp = fopen(fileName,"r");

	if(fp == NULL){
		printf("Empty file\n\n");
	}
	else {
		numPts=+1;
		while((k = fgetc(fp)) != EOF){
			if(k == '\n'){
				numPts++;
			}
		}
		//allocate memory for arrays
		x = (float*) malloc(sizeof(float)*numPts);
		y = (float*) malloc(sizeof(float)*numPts);

		printf("\n\nFILE OPENED FOR READING\n");
		printf("There are %d records.\n\n",numPts);
		rewind(fp);
		while(fscanf(fp,"%f %f",&temp_x,&temp_y) != EOF){
			x[j] = temp_x;
			y[j] = temp_y;
			j++;
		}
		printf("File read into memory\n\n");
	}//end else
	
	free(fileName);
}//end func

/********************************************************************
Function name:	LinearRegression
Purpose:		Calculate least squares fitting of data 
In parameters:	float* x, float* y, int ndata, float* m, float* c, float* sigm, float*
				sigc, float* sigma
Out parameters: void
Version:		1.0
Author:		    Tim Osse
**********************************************************************/
void linearRegression(float* x, float* y, int ndata, float* m, float* c, float* sigm, float*
					  sigc, float* sigma)
{
	int i;
	float t,sxoss,sx=0.0,sy=0.0,st2=0.0,ss,sigdat,sumSquares=0.0,sigma2,residual;
	*m=0.0;

	for(i=0;i<ndata;i++){
		sx += x[i];
		sy += y[i];
	}
	ss=ndata;
	sxoss=sx/ss;
	for(i=0;i<ndata;i++){
		t=x[i]-sxoss;
		st2 += t*t;
		*m += t*y[i];
	}
	*m /= st2;				//Solve for m
	*c=(sy-sx*(*m))/ss;		//c

	//Calculate the standard deviation
	for(i=0;i<ndata;i++){
		residual=(y[i]-(*m)*x[i]-(*c));
		sumSquares+=(residual*residual);
	}
	sigma2=sumSquares/(ndata-2);
	*sigma=sqrt(sigma2);
	*sigc=sqrt(sigma2*((1.0+sx*sx/(ss*st2))/ss));		//sigc
	*sigm=sqrt(sigma2/st2);								//sigm
}

/********************************************************************
Function name:	linear
Purpose:		Fit to a straight line: y = mx + c
In parameters:	void
Out parameters: void
Version:		1.0
Author:		    Tim Osse
**********************************************************************/
void linear(void){

	char ext_opt;//extrapolation menu option
	int year;
	float estimate;
	int running=1;
	linearRegression(x,y,numPts,&m,&c,&sigm,&sigc,&sigma);
	if(m < 0.01 ){
		printf("Linear:	Y = (%.2e+-%.1e)*x + (%.1e+-%.1e)\n",m,sigm,c,sigc);		
	}else
		printf("Linear:	Y = (%3.3f+-%3.2f)*x + (%3.2f+-%3.2f)\n",m,sigm,c,sigc);

	while(running == 1){

		printf("\n***********************************************************\n");
		printf("Extrapolation of data Menu\n");
		printf("1.	Do an extrapolation\n");
		printf("2.	Return to main menu\n");
		printf("\n***********************************************************\n");
		printf("Select an option: ");
		fflush(stdin);
		scanf("%c",&ext_opt);
		switch(ext_opt){
		case'1'://extrapolate
			printf("Input a year (e.g. 2020) to extrapolate to: ");
			scanf("%d",&year);
			//calculate y
			estimate = (m*(year-offSetX)+c)+ offSetY;
			printf("Y(%d) = %3.1f\n",year, estimate);
			break;
		case'2'://return to main menu
			running=0;
		}
	}//end while

}//end linear

/********************************************************************
Function name:	powerLaw
Purpose:		Fit to a power law y = mx^c
In parameters:	void
Out parameters: void
Version:		1.0
Author:		Tim Osse
**********************************************************************/
void powerLaw(void){

	char ext_opt;//extrapolation menu option
	float year;
	float estimate;
	int running=1;
	int k;
	x_log = (float*) malloc(sizeof(float)*numPts);
	y_log = (float*) malloc(sizeof(float)*numPts);
	for(k=0;k<numPts;k++){
		x_log[k] = log(x[k]);
		y_log[k] = log(y[k]);
	}
	linearRegression(x_log,y_log,numPts,&m,&c,&sigm,&sigc,&sigma);
	c2 = m;
	m = exp(c);

	if(m < 0.01 ){
		printf("Power Law: Y = (%.2e)*x^(%.1e)\n",m,c2);
	}
	else
		printf("Power Law: Y = (%3.3f)*x^(%3.2f)\n",m,c2);

	while(running == 1){

		printf("\n***********************************************************\n");
		printf("Extrapolation of data Menu\n");
		printf("1.	Do an extrapolation\n");
		printf("2.	Return to main menu\n");
		printf("\n***********************************************************\n");
		printf("Select an option: ");
		fflush(stdin);
		scanf("%c",&ext_opt);
		switch(ext_opt){
		case'1'://extrapolate
			printf("Input a year (e.g. 2020) to extrapolate to: ");
			scanf("%f",&year);
			//calculate y
			estimate = (m*pow((year-offSetX),c2))+ offSetY;
			printf("Y(%.2e) = %3.1f\n",year, estimate);
			break;
		case'2'://return to main menu
			running=0;
		}
	}//end while
}//end powerLaw
