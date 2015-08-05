#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int main(int argc, char *argv[])
{
	int i;
	char *filename=(char *)malloc(1000);
	char str1[10]="/proc/\0";
	char str2[10]="/status\0";
	if(argc==1)
	{
		printf("Error: PID not given\n");
		exit(0);
	}
	filename=strcat(str1,argv[1]);
	filename=strcat(filename,str2);
	FILE *fp=fopen(filename,"r");
	char *tmp=(char *)malloc(1000);
	char c;
	int count,val,tmpi,flag;

	if(fp==NULL)
	{
		printf("Error: Process not found\n");
		exit(0);
	}
	else
	{
		tmpi=0;flag=0;count=0;
		while(fscanf(fp,"%s",tmp)!=EOF)
		{
			if(flag)
			{
				printf("%s ",tmp);
				count++;
			}
			if(count==2)
			{
				printf("\n");
				break;
			}
			if(strcmp(tmp,"State:")==0)
			{
				printf("State: ");
				flag=1;
			}
		}
		flag=0;
		while(fscanf(fp,"%s",tmp)!=EOF)
		{
			if(flag)
			{
				printf("Parent ID: %s\n",tmp);
				break;
			}
			if(strcmp(tmp,"PPid:"))
				flag=1;
		}
		flag=0;
		while(fscanf(fp,"%s",tmp)!=EOF)
		{
			if(flag)
			{
				printf("Threads: %s\n",tmp);
				break;
			}
			if(strcmp(tmp,"Threads:")==0)
				flag=1;
		}
	}
	return 0;
}
