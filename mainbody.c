#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int storefield(char* line, int column, int** output)
{
    const char* cursor;
    for (cursor = strtok(line, ";"); cursor; cursor = strtok(NULL, ";\n"))
    {
        int row=0;
        int temp=atoi(cursor);
	output[row][column]=temp; 
	row++;
    }
    return 0;
}

int 
main (int argc, char *argv[]){
	FILE *input= fopen("k.csv","r");
	
	char line[128];
	int value[128][128];
	double out[128];
	int cofv,rofv=0;
	int** ptr=value;
	while(fgets(line,128,input)!=NULL){

		char* buffer= strdup(line);
		storefield(buffer,cofv,ptr);		
		free(buffer);
		cofv++;
	}
	rofv=sizeof(value)/(cofv*sizeof(int));
	for(int i=0;i<rofv;i++){
		for(int j=0;j<cofv;j++){
			out[i]+=(double)value[i][j];
		}
		out[i]=out[i]/(double)cofv;
		printf("avg of row %d is %f\n",i,out[i]);
	}
		
	
	return 0;
}
