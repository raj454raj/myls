#define _GNU_SOURCE       
#include <dirent.h>     
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
	long           d_ino;
	off_t          d_off;
	unsigned short d_reclen;
	char           d_name[];
};

#define BUF_SIZE 1024
int len(char *s)
{
	int i;
	for(i=0;s[i];i++)
		;
	return i;
}
int contains(char *s,char c)
{
	int i;
	for(i=0;s[i];i++)
	{
		if(s[i]==c)
			return 1;
	}
	return 0;
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
void assign(char *tmp, char *file)
{
	int i;
	for(i=0;file[i];i++)
		tmp[i]=file[i];
	tmp[i]='\0';
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
char str[50];
int k=0;
void tostring(int n)
{
	k=0;
	char str1[50];
	int z=0;
	while(n)
	{
		str1[z++]=n%10+'0';
		n/=10;
	}
	int i;
	for(i=z-1;i>=0;i--)
		str[k++]=str1[i];
	str[k]='\0';

}
void printinteger(int n)
{
	tostring(n);
	if(n==0)
	{
		str[0]='0';
		str[1]='\0';
	}
	write(1, str,len(str)+1);
}
void printdatemodified(long long int secs)
{
	long long int tempdays=24*60*60,days=0;
	secs+=5*60*60+30*60;
	int year=1970;
	while(secs>tempdays)
	{
		days++;
		if((days%365==0&&year%4)||(days%366==0))
		{
			year++;
			days=0;
		}
		secs-=24*60*60;
		//tempdays+=24*60*60;
	}
	int monthdays[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	char *months[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	int i;
	if(year%4==0)monthdays[2]++;
	for(i=1;i<=12;i++)
	{
		if(days<monthdays[i])
			break;
		else
			days-=monthdays[i];
	}
	int hours=0,mins=0;
	while(secs>60)
	{
		if(secs>3600)
		{
			secs-=3600;
			hours++;
		}
		else if(secs>60)
		{
			secs-=60;
			mins++;
		}

	}
	write(1,months[i-1],4);
	write(1," ",1);
	printinteger(days+1);
	write(1," ",1);
	if(hours/10==0)
		printinteger(0);
	printinteger(hours);
	write(1,":",1);
	if(mins/10==0)
		printinteger(0);
	printinteger(mins);
	write(1," ",1);
	//printf("%lld %d %d %d %lld\n",days,hours,mins,i,secs);
}
int main(int argc, char *argv[])
{
	int size,tmpdec,i,fd, nread, fp, ntemp, tmpi,colons, uid,tmpflag;
	double tmpsize;
	long long int giga,mega,kilo;
	giga=1024*1024*1024;
	mega=1024*1024;
	kilo=1024;
	char buf[BUF_SIZE];
	char user[50];
	char buffer[50000],linkval[500];
	char line[1000];
	struct linux_dirent *d;
	int bpos,isalink;
	int flag=0,hasfile=0;
	char d_type;
	int l=0,a=0,h=0;
	struct stat fileStat,tmpStat;
	char file[100],tmpfile[100];
	if(argc==2||argc==3)
	{
		if(compare(argv[1],"-l")||compare(argv[1],"-a")||compare(argv[1],"-h")||compare(argv[1],"-lah")||compare(argv[1],"-lha")||compare(argv[1],"-ahl")||compare(argv[1],"-alh")||compare(argv[1],"-hla")||compare(argv[1],"-hal")||compare(argv[1],"-la")||compare(argv[1],"-ah")||compare(argv[1],"-hl")||compare(argv[1],"-al")||compare(argv[1],"-ha")||compare(argv[1],"-lh"))
		{
			flag=1;
			if(argc>2)
				hasfile=1;
			if(contains(argv[1],'l'))
				l=1;
			if(contains(argv[1],'a'))
			{
				if(l==0)
					flag=0;
				if(argc==3)
					hasfile=1;
				a=1;
			}
			if(contains(argv[1],'h'))
			{
				h=1;
				if(l==0)
					flag=0;
				if(argc==3)
					hasfile=1;
			}
		}
		else if(argc==2)
			hasfile=1;
	}
	if(argc==3)
	{
		if(compare(argv[2],"-l")||compare(argv[2],"-a")||compare(argv[2],"-h")||compare(argv[2],"-lah")||compare(argv[2],"-lha")||compare(argv[2],"-ahl")||compare(argv[2],"-alh")||compare(argv[2],"-hla")||compare(argv[2],"-hal")||compare(argv[2],"-la")||compare(argv[2],"-ah")||compare(argv[2],"-hl")||compare(argv[2],"-al")||compare(argv[2],"-ha")||compare(argv[2],"-lh"))
		{
			flag=2;
			hasfile=1;
			if(contains(argv[2],'l'))
				l=1;
			if(contains(argv[2],'a'))
			{
				a=1;
				if(l==0)
					flag=0;
			}
			if(contains(argv[2],'h'))
			{
				h=1;
				if(l==0)
					flag=0;
			}
		}
	}
	//	printf("%d %d %d %d %d %d\n" ,l,a,h,hasfile,flag, argc);
	if((!flag&&a&!l)||(!flag&&h&!l)||!a&!l&!h||(!flag&&a&h))
	{
		if(argc>1)
		{
			file[0]='\0';
			if(!a&&!h)
			{	
				fd = open(argv[1], O_RDONLY | O_DIRECTORY);
				concat(file,argv[1]);
				//printf("%s",file);
			}
			else if(a|h)
			{
				if(hasfile)
				{
					if(compare(argv[1],"-a")||compare(argv[1],"-h")||compare(argv[1],"-ah")||compare(argv[1],"-ha"))
					{
						fd = open(argv[2], O_RDONLY | O_DIRECTORY);
						concat(file,argv[2]);
					}
					else if(compare(argv[2],"-a")||compare(argv[2],"-h")||compare(argv[2],"-ah")||compare(argv[2],"-ha"))
					{
						fd = open(argv[1], O_RDONLY | O_DIRECTORY);
						concat(file,argv[1]);
					}
					//fd = open(".", O_RDONLY | O_DIRECTORY);
				}
				else
				{
					fd = open(".", O_RDONLY | O_DIRECTORY);
					file[0]='.';
					file[1]='\0';
				}
			}
			

		}
		else
		{
			fd = open(".", O_RDONLY | O_DIRECTORY);
			file[0]='.';
			file[1]='\0';
		}
		if (fd == -1)
			handle_error("open");

		for ( ; ; ) 
		{

			nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
			if (nread == -1)
				handle_error("getdents");

			if (nread == 0)
				break;
			for (bpos = 0; bpos < nread;) {
				isalink=0;
				d = (struct linux_dirent *) (buf + bpos);
				d_type = *(buf + bpos + d->d_reclen - 1);
				if(d->d_name[0]=='.'&&!a)
				{
					bpos += d->d_reclen;
					continue;
				}
				tmpfile[0]='.';
				tmpfile[1]='\0';
				assign(tmpfile,file);
				concat(tmpfile,"/");
				//printf("");
				concat(tmpfile,d->d_name);
				if(lstat(tmpfile,&fileStat) < 0)    
					return -1;
				if(!a)
				{
					if(d->d_name[0]!='.')
					{	

						if(S_ISDIR(fileStat.st_mode))
							write(1,"\x1b[1;34m",8);
						else if(d_type==DT_LNK)
							write(1,"\x1b[1;36m",8);
						else if(fileStat.st_mode & S_IXUSR)
							write(1,"\x1b[1;32m",8);

						write(1,d->d_name,len(d->d_name)+1);	
						if(S_ISDIR(fileStat.st_mode))
							write(1,"\x1b[0m",5);
						else if(d_type==DT_LNK)
							write(1,"\x1b[0m",5);
						else if(fileStat.st_mode & S_IXUSR)
							write(1,"\x1b[0m",5);
						write(1,"\t",2);
					}
				}
				else
				{
					if(S_ISDIR(fileStat.st_mode))
						write(1,"\x1b[1;34m",8);
					else if(d_type==DT_LNK)
						write(1,"\x1b[1;36m",8);
					else if(fileStat.st_mode & S_IXUSR)
						write(1,"\x1b[1;32m",8);

					write(1,d->d_name,len(d->d_name)+1);
					if(S_ISDIR(fileStat.st_mode))
						write(1,"\x1b[0m",5);
					else if(d_type==DT_LNK)
						write(1,"\x1b[0m",5);
					else if(fileStat.st_mode & S_IXUSR)
						write(1,"\x1b[1;32m",8);
					write(1,"\t",2);

					//	write(1,d->d_name,len(d->d_name)+1);
					write(1,"\t",2);
				}
				bpos += d->d_reclen;
			}
		}
		write(1,"\n",2);
		exit(EXIT_SUCCESS);
	}
	else if(flag==1)
	{
		if(hasfile)
		{
			fd = open(argv[2], O_RDONLY | O_DIRECTORY);
			file[0]='\0';
			concat(file,argv[2]);
		}
		else
		{
			fd = open(".", O_RDONLY | O_DIRECTORY);
			file[0]='.';
			file[1]='\0';
		}
		if (fd == -1)
			handle_error("open");

	}
	else
	{
		fd = open(argv[1], O_RDONLY | O_DIRECTORY);
		file[0]='\0';
		concat(file,argv[1]);
	}

	if (fd == -1)
		handle_error("open");

	for ( ; ; ) 
	{
		nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
		if (nread == -1)
			handle_error("getdents");

		if (nread == 0)
			break;
		//printf("%d",nread);
		for (bpos = 0; bpos < nread;) 
		{
			k=0;
			isalink=0;
			d = (struct linux_dirent *) (buf + bpos);
			d_type = *(buf + bpos + d->d_reclen - 1);
			if(d->d_name[0]=='.' && !a)
			{
				bpos += d->d_reclen;
				continue;
			}
			//printf("%s\t",d->d_name);
			tmpfile[0]='\0';
			assign(tmpfile,file);
			concat(tmpfile,"/");
			concat(tmpfile,d->d_name);
			if(lstat(tmpfile,&fileStat) < 0)    
				return 1;
			if(S_ISDIR(fileStat.st_mode))
				write(1,"d",2);
			else 
			{
				if(d_type==DT_LNK)
				{	
					isalink=1;
					write(1,"l",2);
				}	
				else
					write(1,"-",2);

			}
			//				write(1, (S_ISDIR(fileStat.st_mode)) ? "d" : (S_ISLNK(fileStat.st_mode)?"l":"-"), 2);
			write(1, (fileStat.st_mode & S_IRUSR) ? "r" : "-", 2);
			write(1, (fileStat.st_mode & S_IWUSR) ? "w" : "-", 2);
			write(1, (fileStat.st_mode & S_IXUSR) ? "x" : "-", 2);
			write(1, (fileStat.st_mode & S_IRGRP) ? "r" : "-", 2);
			write(1, (fileStat.st_mode & S_IWGRP) ? "w" : "-", 2);
			write(1, (fileStat.st_mode & S_IXGRP) ? "x" : "-", 2);
			write(1, (fileStat.st_mode & S_IROTH) ? "r" : "-", 2);
			write(1, (fileStat.st_mode & S_IWOTH) ? "w" : "-", 2);
			write(1, (fileStat.st_mode & S_IXOTH) ? "x" : "-", 2);
			write(1," ",2);
			if(!isalink)
				printinteger(fileStat.st_nlink);
			else
				write(1,"1",2);
			write(1,"\t",2);
			//printinteger(fileStat.st_uid);
			//write(1,"\t",2);
			{
				fp=open("/etc/passwd",O_RDONLY);
				tmpi=0;colons=0;tmpflag=0;uid=0;
				while(ntemp=read(fp, buffer, 1))
				{
					if(buffer[0]=='\n')
					{
						colons=0;
						tmpflag=0;
						uid=0;
						tmpi=0;
						line[0]='\0';
						continue;
					}
					line[tmpi++]=buffer[0];
					if(buffer[0]==':')
						colons++;
					if(colons==3&&tmpflag)
					{
						tmpflag=0;
						if(uid==fileStat.st_uid)
							break;
					}
					if(tmpflag)
					{
						uid=uid*10+buffer[0]-'0';
					}
					if(colons==2)
						tmpflag=1;
				}
				line[tmpi]='\0';
				tmpi=0;
				for(i=0;line[i];i++)
				{
					if(line[i]==':')
						break;
					else
						user[tmpi++]=line[i];
				}
				user[tmpi]='\0';
				//printinteger(uid);
				write(1, user, len(user)+1);
			}
			write(1,"\t",2);
			{
				fp=open("/etc/group",O_RDONLY);
				tmpi=0;colons=0;tmpflag=0;uid=0;
				while(ntemp=read(fp, buffer, 1))
				{
					if(buffer[0]=='\n')
					{
						colons=0;
						tmpflag=0;
						uid=0;
						tmpi=0;
						line[0]='\0';
						continue;
					}
					line[tmpi++]=buffer[0];
					if(buffer[0]==':')
						colons++;
					if(colons==3&&tmpflag)
					{
						tmpflag=0;
						if(uid==fileStat.st_gid)
							break;
					}
					if(tmpflag)
					{
						uid=uid*10+buffer[0]-'0';
					}
					if(colons==2)
						tmpflag=1;
				}
				line[tmpi]='\0';
				tmpi=0;
				for(i=0;line[i];i++)
				{
					if(line[i]==':')
						break;
					else
						user[tmpi++]=line[i];
				}
				user[tmpi]='\0';
				write(1, user, len(user)+1);
			}
			write(1,"\t",2);
			if(h)
			{
				tmpsize=fileStat.st_size;
				size=(int)(tmpsize);
				if(size/giga)
				{
					if(size/giga>1024)
					{
						printinteger((int)((tmpsize/giga)/1024));
						tmpdec=((int)(10*((tmpsize/giga)/1024)))%10;
						write(1,".",2);
						//printf("*%d*",tmpdec);
						printinteger(tmpdec);
						write(1, "T", 2);
					}
					else
					{
						printinteger((int)(tmpsize/giga));
						tmpdec=((int)((10*(tmpsize/giga))))%10;
						write(1,".",2);
						//printf("*%d*",tmpdec);
						printinteger(tmpdec);
						write(1, "G", 2);
					}
				}
				else if(size/mega)
				{
					printinteger((int)(tmpsize/mega));
					tmpdec=((int)(10*(tmpsize/mega)))%10;
					write(1,".",2);
					//	printf("*%d*",tmpdec);
					printinteger(tmpdec);
					write(1, "M", 2);
				}
				else if(size/kilo)
				{
					printinteger((int)(tmpsize/kilo));
					tmpdec=((int)(10*(tmpsize/kilo)))%10;
					write(1,".",2);
					//printf("*%d*",tmpdec);
					printinteger(tmpdec);
					write(1, "K", 2);
				}
				else
				{
					printinteger(size);
				}

			}
			else
				printinteger(fileStat.st_size);

			write(1, "\t", 2);
			printdatemodified(fileStat.st_mtime);
			if(S_ISDIR(fileStat.st_mode))
				write(1,"\x1b[1;34m",8);
			else if(isalink)
				write(1,"\x1b[1;36m",8);
			else if(fileStat.st_mode & S_IXUSR)
				write(1,"\x1b[1;32m",8);
			write(1,d->d_name,len(d->d_name)+1);	
			if(S_ISDIR(fileStat.st_mode))
				write(1,"\x1b[0m",5);
			else if(isalink)
			{
				write(1,"\x1b[0m",5);
				write(1, " -> ",4);
				readlink(tmpfile,linkval,500);
				write(1,"\x1b[1;34m",8);
				write(1,linkval,len(linkval));
				write(1,"\x1b[0m",5);
			}
			else if(fileStat.st_mode & S_IXUSR)
				write(1,"\x1b[0m",5);
			write(1,"\n",2);
			bpos += d->d_reclen;
		}
	}

	exit(EXIT_SUCCESS);
}

