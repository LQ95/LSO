#include "scan_int.h"
int *convert(int min,int max){
	//Riceve in input una stringa e la converte a codice.
	char input[10];
	int *input_finale;
	input_finale=malloc(sizeof (int));
	*input_finale=0;
	if(!input_finale){
		printf("allocazione non riuscita");
		return NULL;
	}
	int pointer_controllo=0;
	scanf("%s",input);
		for(pointer_controllo=0;input[pointer_controllo]!=0;pointer_controllo++){
			if (input[pointer_controllo]==0) break;
			if(input[pointer_controllo]<48||input[pointer_controllo]>57) { 
				printf("input scorretto,riprovare: ");
				free(input_finale);
				input_finale=NULL;
				return input_finale;
				}
			else {
				*input_finale=(*input_finale*10);
				*input_finale=*input_finale+(input[pointer_controllo]-48);    //conversione ASCII->cifra
				if (*input_finale>max){
					printf("input troppo grande,riprovare: ");
					free(input_finale);
					input_finale=NULL;
					return input_finale;
					}
				}
		}
	if (*input_finale<min) {
				printf("input troppo piccolo,riprovare: ");
				free(input_finale);
				input_finale=NULL;
				return input_finale;
			}
	return input_finale;
}
int scan_int (int min, int max){
	int *input_finale=NULL;
	int out;
	do{
		input_finale=convert(min,max);
	}
	while (input_finale==NULL);
	if (input_finale!=NULL){
		out=*input_finale;
	}
	return out;
	}
