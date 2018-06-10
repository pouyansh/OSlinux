#include <stdio.h>
#include <string.h>

#define MAXCHAR 1000
int main()
{
	while (1){
		printf("menu:\n>>> length\n>>> protocol\n>>> hash\n>>> proctime\n\n>>> exit\n");
		
		char str[MAXCHAR];
   		gets(str);
   		if(strcmp(str, "exit") == 0) break;
   	
   		int write = 0;
  		if(strcmp(str, "length") == 0 ) write = 1;
    		else if(strcmp(str, "protocol") == 0 ) write = 2;
		else if(strcmp(str, "hash") == 0 ) write = 3;
		else if(strcmp(str, "proctime") == 0 ) write = 4;	
   	
		FILE * fp;
		printf("resid\n");
		fp = fopen ("/proc/sockinfo/in","w");
	  	fprintf (fp, "%d\n", write);
	   	fclose (fp);


		//khurujie module mire too out:
		//FILE * fpOut;
		//printf("resid\n");
		//fpOut = fopen ("/proc/sockinfo/out","w");
	  	//fprintf (fpOut, "result is %d\n", write * write);
	   	//fclose (fpOut);
		//khurujie module rafte too out.
	

		printf("resid\n");
		char* filename = "/proc/sockinfo/out";
		fp = fopen(filename, "r");
	    	if (fp == NULL){
			printf("Could not open file %s",filename);
			return 1;
	    	}
	    	while (fgets(str, MAXCHAR, fp) != NULL)
			printf("%s", str);
	    	fclose(fp);
	}
	
	return 0;
}
