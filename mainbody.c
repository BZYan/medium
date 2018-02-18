#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int 
main (int argc, char *argv[]){

typedef struct node node;

struct node{
		char* name;
		int ocr;
};

int scomparator (const void* pa, const void* pb){
	char* a= (const char*) pa;
	char* b=(const char* ) pb;
	int leng;
	if(strlen(a)>strlen(b)) leng=strlen(b); else leng=strlen(a);
	for (int i=0;i<leng;i++){
		if (a[i]!=b[i]) return b[i]-a[i];
	}
	if(strlen(a)>strlen(b)) return 1; else return -1;
}
	
int ncomparator (const void *pa, const void *pb){
	node* a=(node*) pa;
	node* b=(node*) pb;
	return (b->ocr - a->ocr);
}



	FILE *input= fopen(argv[1],"r");
	
	char word[1000][100];
	char ch;
	int wordcnt=0;
	char buffer[100];
	
	buffer[0]='\0';

	for( int i=0, j=0; ((ch=getc(input))!=EOF);){	
		
		if (! isalnum(ch)) {
			if(j!=0){
				
				//word[i]=malloc(sizeof(char)*strlen(buffer));
				strcpy(word[i],buffer);
				i++;  
				wordcnt++;
			}
			j=0;
			buffer[0]='\0';
		}else{
			buffer[j]=tolower(ch);
			j++;
		} 
		
			buffer[j]='\0'; 
			
	}

	if(buffer[0]!='\0') {
		strcpy(word[wordcnt],buffer);
		wordcnt++;
	}
	
	
	
	
	
	
	
	for(int i=0; i<wordcnt; i++){
		printf("%s\n",word[i]);
		
	}
	
	
	qsort((void*)word, wordcnt, sizeof(char[100]),scomparator);
	
	for(int i=0; i<wordcnt; i++){
		printf("%s\n",word[i]);
		
	}




	node table[wordcnt];	
	int pop=0;

	int occurcnt=1;
	int checker=0;

	for (int i=1; i<wordcnt; i++){
		
		if (!strcmp(word[checker], word[i])&&(i!=(wordcnt-1))) {
			occurcnt++;
		}else{
			node new= {.name=word[checker], .ocr=occurcnt};
			
			table[pop]=new;
			pop++;
			checker=i;
			occurcnt=1;
		}
		
	}
	qsort(table, pop, sizeof(node),ncomparator);

	for(int i=9; i>=0; i--) {
		printf ("%d, %s\n",table[i].ocr, table[i].name);
	}
	return 0;
}