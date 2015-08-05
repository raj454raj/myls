#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int len(char *s)
{
	int i;
	for(i=0;s[i];i++)
		;
	return i;
}
int compare(char *s1, char *s2)
{
	int i;
	if(len(s1)!=len(s2))
		return 0;
	for(i=0;s1[i]&&s2[i];i++)
		if(s1[i]!=s2[i])
			return 0;
	return 1;
}
void concat(char *s1,char *s2)
{
	int j,i=0;
	for(i=0;s1[i];i++)
		;
	for(j=0;s2[j];j++)
	{
		s1[j+i]=s2[j];
	}
	s1[j+i]='\0';
}

int main(int argc, char *argv[])
{
	char filename[50];
	char buffer[3],c;
	filename[0]='\0';
	char str1[10]="/proc/\0";
	char str2[10]="/status\0";
	if(argc==1)
	{
		printf("Error: PID not given\n");
		exit(0);
	}
	concat(filename,str1);
	concat(filename,argv[1]);
	concat(filename,str2);
	//printf("%s",filename);
	//write(1,filename,len(filename));
	
	int ntemp,fp=open(filename,O_RDONLY);
	if(fp<0)
	{
		printf("Process ID not found");
		return 0;
	}
	char tmpstr[2],line[50],val[50];
	int tmpi=0,tmpflag=0,z=0;
	while(ntemp=read(fp, buffer, 1))
	{
		c=buffer[0];
		tmpstr[0]=c;
		tmpstr[1]='\0';
		if(tmpflag&&c=='\t')
			continue;
		if(c=='\n')
		{
			if(tmpflag)
			{
				val[z]='\0';
				if(tmpflag==1)
					write(1,"State: ",7);
				else if(tmpflag==2)
					write(1,"Parent ID: ",11);
				else if(tmpflag==3)
					write(1,"Threads: ",9);
				write(1,val,len(val));
				write(1,"\n",2);
				//write(1,line,len(line));
			}
			tmpi=0;
			z=0;
			tmpflag=0;
			line[0]='\0';
			val[0]='\0';
			continue;
		}
		if(tmpflag)
		{
			val[z++]=c;
		}
		if(c==':')
		{
			line[tmpi]='\0';
			if(compare(line,"State"))
				tmpflag=1;
			if(compare(line,"PPid"))
				tmpflag=2;
			if(compare(line,"Threads"))
				tmpflag=3;
		}
		else if(!tmpflag)
			line[tmpi++]=c;
	}
	return 0;
}
