#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <openssl/md5.h>
#include <openssl/hmac.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/wait.h>

pid_t pid;


void Indexget(char read_buffer[],int *newsocketfd);
void Filehash(char read_buffer[] , int *newsocketfd);
void FileDownload(char read_buffer[],int *newsocketfd);
//void FileUpload(char a[],int *socketfd);

void clientIndexget(char write_buffer[],int *socketfd);
void clientFilehash(char write_buffer[], int *socketfd);	
void clientFiledownload(char write_buffer[],int *socketfd);	
//void clientFileUpload(char a[],int *socketfd);


struct print_data{
    char filename[100]; //filename
    off_t size; //size
    time_t mtime; //last modified
    char type; //filetype
};
struct print_data pdata[2048];
void put_error(const char *msg)
{
    perror(msg);
    exit(0);
}

void host()
{
int socketfd, newsocketfd,rfd;
	socklen_t cli_len;
	char read_buffer[2048];
	struct sockaddr_in servi_addr, cli_addr;
	int n,i;
	char commands[1000][100];
	int flag[1000],h=0;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
		put_error("ERROR opening socket");
	else
		printf("[Server] Socket intialized \n");

	bzero((char *) &servi_addr, sizeof(servi_addr));
	servi_addr.sin_family = AF_INET;
	servi_addr.sin_addr.s_addr = INADDR_ANY;
	servi_addr.sin_port = htons(5005);

	if (bind(socketfd, (struct sockaddr *) &servi_addr, sizeof(servi_addr)) < 0)
		put_error("ERROR on binding");
	else
		printf("[Server] Socket Binded to the Server Address\n");

	if(listen(socketfd,5) < 0)
		put_error("ERROR in listening");

	printf("[Server] Server waiting for an client\n Press Enter to continue");
	fflush(stdout);

	cli_len = sizeof(cli_addr);
	newsocketfd = accept(socketfd,  (struct sockaddr *) &cli_addr, &cli_len);
	if (newsocketfd < 0)
		put_error("ERROR on accept");

while(1)
		{
			bzero(read_buffer,2048);
			rfd = read(newsocketfd, read_buffer, 2048);
			if(rfd>0)
			printf("Message Recieved : %s \n",read_buffer);
			if(strncmp(read_buffer, "FileHash", 8) == 0)
			{
				Filehash(read_buffer, &newsocketfd);
			}
			else if(strncmp(read_buffer , "IndexGet",8)==0)
			{

				Indexget(read_buffer,&newsocketfd);
			}
			else if(strncmp(read_buffer , "FileDownload",12)==0)
			{

				FileDownload(read_buffer,&newsocketfd);
			}
			/*else if(strncmp(read_buffer , "FileUpload",10)==0)
			{

				FileUpload(read_buffer,&newsocketfd);
			}*/
			
			else if(rfd<0)
			{
				break;
			}	

			fflush(stdout);

		}
close(newsocketfd);
		printf("\n Connection closed by peer\n");
//	}
		
	close(socketfd);
	exit(1);
	return ;
}

void Indexget(char read_buffer[],int *newsocketfd)
{
	char write_buffer[2048];
	bzero(write_buffer,2048);
	char l[10]="--longlist",r[7]="--regex",s[11]="--shortlist";
	int type=0,i;
	for(i=0;i<10;i++)
		if(l[i]!=read_buffer[9+i])
			type=1;
	if(type==1)
	{
		for(i=0;i<7;i++)
			if(r[i]!=read_buffer[9+i])
				type=2;
		//printf("aaa");
	}
	if(type==2)
	{
		for(i=0;i<11;i++)
			if(s[i]!=read_buffer[9+i])
				type=3;

	}
	if(type==0)
	{   
		DIR *dp;
    i = 0; 
    struct dirent *ep;
    dp = opendir ("./");
    struct stat fileStat;

    if (dp != NULL)
    {
        while (ep = readdir (dp))
        {
            if(stat(ep->d_name,&fileStat) < 0)
                return 1;
            else
            {
                strcpy(pdata[i].filename, ep->d_name);
                pdata[i].size = fileStat.st_size;
                pdata[i].mtime = fileStat.st_mtime;
                pdata[i].type = (S_ISDIR(fileStat.st_mode)) ? 'd' : '-';
                i++;

                sprintf(write_buffer,"%-20s %-10zd %-10jd %-5c\n ",ep->d_name,fileStat.st_size,fileStat.st_mtime,type);
                write(*newsocketfd, write_buffer, 2048);
            }
        }
        closedir (dp);
    }
    else
    {
        printf("Couldn't open the directory");
    }
    /*FILE *fs = fopen("Result", "r");
		if(fs == NULL)
		{
			put_error("ERROR: File not found");
			exit(1);
		}*/
		int j=0, flag=0;
		while(i > j)
		{

			
			
			bzero(write_buffer, 2048);
			flag=1;
			j++;
		}
		if(flag==1)
		{
		bzero(write_buffer, 2048);
		strcpy(write_buffer,"over");
		if(write(*newsocketfd, write_buffer, 2048) < 0)
		{
			put_error("\nERROR: Writing to socket");
			exit(1);
		}
		}
	
	}
	if(type==1)
	{
/*
		FILE *pipein_fp;
    char string[2048] = "ls ";
    char str[2048];
    memset(str, 0,sizeof(str));
    char line[2048];
    char readbuf[2048];
    int i = 0;
    int regex = 1;

    strncpy(str,re +1,strlen(regexp)-2);
    strcat(string,str);

    DIR *dp;
    int a;
    struct dirent *ep;
    dp = opendir ("./");
    struct stat fileStat;

    if (dp != NULL)
    {
        while (ep = readdir (dp))
        {
            if(stat(ep->d_name,&fileStat) < 0)
                return 1;
            else
            {
                if (( pipein_fp = popen(string, "r")) == NULL)
                {
                    perror("popen");
                    exit(1);
                }
                while(fgets(readbuf, 2048, pipein_fp))
                {
                    strncpy(line,readbuf,strlen(readbuf)-1);
                    if(strcmp(line,ep->d_name) == 0)
                    {
                        strcpy(pdata[i].filename , ep->d_name);
                        pdata[i].size = fileStat.st_size;
                        pdata[i].type = (S_ISDIR(fileStat.st_mode)) ? 'd' : '-';
                        i++;
                        sprintf(write_buffer,"%-20s %-10zd %-10jd %-5c\n ",ep->d_name,fileStat.st_size,type);
                		write(*newsocketfd, write_buffer, 2048);
                        break;
                    }
                    memset(line, 0,sizeof(line));
                }
            }
        }

        pclose(pipein_fp);
}
}
*/

		//printf("t is %d",t);
		char st[500],str[400];
		int k=0;
		i=17;
		while(i!=strlen(read_buffer))
		{
			st[k]=read_buffer[i];
			k++;
			i++;
		}
		st[k]='\0';

		strcpy(str,"find . -name \""); 
		strcat(str,st);
		strcat(str,"\" -exec ls -l {} \\; | awk '{print $9\"\\t\"$5}' | cut -c "" 3- > Result");
		system("touch Result");
		system(str);

		bzero(write_buffer, 2048); 
		int f=0,flag=0;
		FILE *fs = fopen("Result", "r");
		if(fs == NULL)
		{
			put_error("ERROR: File not found");
			exit(1);
		}
		while((f= fread(write_buffer, sizeof(char),2048, fs)) > 0)
		{
			if(write(*newsocketfd, write_buffer, 2048) < 0)
			{
				put_error("\nERROR: Writing to socket");
				exit(1);
				break;
			}
			bzero(write_buffer, 2048);
			flag=1;
		}
		if(flag==1)
		{
		bzero(write_buffer, 2048);
		strcpy(write_buffer,"over");
		if(write(*newsocketfd, write_buffer, 2048) < 0)
		{
			put_error("\nERROR: Writing to socket");
			exit(1);
		}
		system("rm -rf Result");
		}
	}
	
	if(type==2)
	{
		printf("type=2");
		char st[400],str[400];
		int k=0,flag=0;
		i=21;
		while(read_buffer[i]!=' ')
		{
			st[k]=read_buffer[i];
			k++;
			i++;
		}
		st[k]='\0';
		k=0;
		i++;
		while(read_buffer[i]!='\0')
		{
			str[k]=read_buffer[i];
			k++;
			i++;
		}
		str[k]='\0';
		//printf("a%sa b%sb\n",st,str);
		system("touch fd");
		system("ls --full-time|grep -v ^d | awk '{print $9\"\\t\"$5\"\\t\"$7}' | tail -n +2 | sort -n > fd");
		FILE *file = fopen ( "fd", "r" );
		FILE *file1 = fopen("result","w");
		if ( file != NULL )
		{   
			char line [400]; 
			while ( fgets ( line, sizeof(line), file ) != NULL ) 
			{ 
				char te[400];
				i=0;
				k=0;
				while(line[i]!=' ')
					i++;
				i++;
		//		printf("%c\n",line[i]);
				while(line[i]!=' ')
					i++;
				i++;
		//		printf("%c\n",line[i]);
				while(i!=strlen(line)-1)
				{
					te[k]=line[i];
					k++;
					i++;
				}
				if(strcmp(te,st)>=0&&strcmp(te,str)<=0)
				{
					fputs(line,file1);
				}
			}   
			fclose ( file );
			fclose(file1);
		} 
		else
			put_error("Error in file opening");
		file1=fopen("result","r");
		if(file1!=NULL)
		{
		//	printf("sover");
			int f=0;
		while((f= fread(write_buffer, sizeof(char),2048, file1)) > 0)
		{
			if(write(*newsocketfd, write_buffer, 2048) < 0)
			{
				put_error("\nERROR: Writing to socket");
				exit(1);
				break;
			}
			bzero(write_buffer, 2048);
			flag=1;
		}
		if(flag==1)
		{
		bzero(write_buffer, 2048);
		strcpy(write_buffer,"over");
		if(write(*newsocketfd, write_buffer, 2048) < 0)
		{
			put_error("\nERROR: Writing to socket");
			exit(1);
		}
		system("rm -rf fd");
		system("rm -rf result");
		}
		}
		else
			put_error("Error in file opening");


	}
}
void doHashing(char my[],int *newsocketfd)
{
	int i=0,k=0;
	char write_buffer[2048];
		char md[500],st[100];
		strcpy(md,"md5sum ");
		strcat(md,my);
		strcat(md," > result_md5");
		system("touch result");
		system("touch result_stat");
		strcpy(st,"stat ");
		strcat(st,my);
		strcat(st," | grep Modify | awk '{print $2 "  " $3}' > result_stat");
		system(md);
		system(st);
		FILE *f=fopen("result_md5","r");
		FILE *f1=fopen("result_stat","r");
		char x[300],r[300],a[300];
		fgets(x,sizeof(x),f);
		fgets(r,sizeof(r),f1);
		system("rm -rf result_stat");
		system("rm -rf result_md5");
		i=0,k=0;
		while(x[i]!=' ')
		{   
			a[k]=x[i];
			i++;
			k++;
		}   
		a[k]='\0';
		strcat(my,"\t");
		r[strlen(r)-1]='\0';
		strcat(my,r);
		strcat(my,"\t");
		strcat(my,a);
		strcpy(write_buffer,my);
		if(write(*newsocketfd, write_buffer, 2048) < 0)
		{
			put_error("\nERROR: Writing to socket");
			exit(1);
		}
}


void Filehash(char read_buffer[] , int *newsocketfd)
{
	char write_buffer[2048];
	bzero(write_buffer,2048);
	char v[8]="--verify",c[10]="--checkall";
	int type=0,i;
	for(i=0;i<8;i++)
		if(v[i]!=read_buffer[9+i])
			type=1;
	if(type==1)
	{
		for(i=0;i<10;i++)
			if(c[i]!=read_buffer[9+i])
				type=2;
	}
	if(type==0)
	{
		char my[500];
		int k=0;
		i=18;
		while(i!=strlen(read_buffer))
		{
			my[k]=read_buffer[i];
			k++;
			i++;
		}
		my[k]='\0';
		doHashing(my,newsocketfd);
		bzero(write_buffer,2048);
		strcpy(write_buffer,"over");
		if(write(*newsocketfd, write_buffer, 2048) < 0)
		{
			put_error("\nERROR: Writing to socket");
			exit(1);
		}

	}
	if(type==1)
	{
		int flag=0;
		system("ls -l |grep -v ^d | awk '{print $9}' | tail -n +2 > fd");
		FILE *file = fopen ( "fd", "r" );
		if ( file != NULL )
		{   
			char line [500]; 
			while ( fgets ( line, sizeof(line), file ) != NULL ) 
			{ 
				char te[400];
				int k=0;
				i=0;
				while(line[i]!='\n')
				{
					te[k]=line[i];
					k++;
					i++;
				}
				te[k]='\0';
				doHashing(te,newsocketfd);
			}
			flag=1;
		}
		else
			put_error("Error in file opening");
		if(flag==1)
		{
		bzero(write_buffer, 2048);
		strcpy(write_buffer,"over");
		if(write(*newsocketfd, write_buffer, 2048) < 0)
		{
			put_error("\nERROR: Writing to socket");
			exit(1);
		}
		}
		system("rm -rf fd");

	}

}

void FileDownload(char myarray[],int *socketfd)
{
	
	char write_buffer[2048];
	bzero(write_buffer,2048);
	char *token=strtok(myarray," ");
	token=strtok(NULL," ");
	char c[500];
	
	int n,i;

	FILE *fp=fopen(token,"r");
	 
	if(fp==NULL)
	{
		printf("File can't be opened\n");
		strcpy(write_buffer,"NoFile is there");
		n=write(*socketfd, write_buffer, 2048);
		return;
	} 
//	puts(a);
	sprintf(c,"stat %s | grep Modify | cut -d ' ' -f3 > result",token);
	system(c);
	char v[200];
	FILE *f1=fopen("result","r");
	i=0;
	char cc;
	while(1)
	{
		cc = fgetc(f1);
	    if( feof(f1) )
	    {
	        break ;
	    }
	    v[i++]=cc;
	}
	v[i]='\0';
	fclose(f1);
	sprintf(write_buffer,"timeStp: %s",v);
	puts(write_buffer);
	if(n=write(*socketfd,write_buffer,2048)<0)
	{
		perror("Error in writing in socket");
	}
	bzero(write_buffer,2048);
	system("rm -rf result");

	sprintf(c,"stat %s | grep Modify | cut -d ' ' -f2 > result",token);
	system(c);
	char u[200];
	FILE *f3=fopen("result","r");
	i=0;
	while(1)
	{
		cc = fgetc(f3);
	    if( feof(f3) )
	    {
	        break ;
	    }
	    v[i++]=cc;
	}
	v[i]='\0';
	fclose(f3);
	
	sprintf(write_buffer,"Size: %s",v);
	puts(write_buffer);
	if(n=write(*socketfd,write_buffer,2048)<0)
	{
		perror("Error in writing in socket");
	}
	system("rm -rf result");
	bzero(write_buffer,2048);
	sprintf(write_buffer,"Name: %s",token);
	puts(write_buffer);
	if(n=write(*socketfd,write_buffer,2048)<0)
	{
		perror("Error in writing in socket");
	}
	bzero(write_buffer,2048);
	sprintf(v,"md5sum %s > result",token);
	system(v);
	f3=fopen("result","r");
	i=0;
	while(1)
	{
		cc = fgetc(f3);
	    if( feof(f3) )
	    {
	        break ;
	    }
	    v[i++]=cc;
	}
	v[i]='\0';
	system("rm -rf result");
	i=0;
	while(v[i]!=' ')
	{
		u[i++]=v[i];
	}
	u[i]='\0';
	sprintf(write_buffer,"md5sum hash: %s",u);
	puts(write_buffer);
	write(*socketfd,write_buffer,2048);
	bzero(write_buffer,2048);
	
	char b[1000];
	strcpy(b,token);
	FILE *fd;
	fd=fopen(b,"rb");
	char msg[1025];
	bzero(msg,1025);
	
	while((i=fread(msg,sizeof(char),1025,fd))>0)
	{
		if(write(*socketfd,msg,i)<0)
		{
			printf("ERROR: can't write to socket\n");
			break;
		}
		bzero(msg,1025);
	}
	bzero(msg,1025);
	strcpy(msg,"FILE LIST");
	sleep(1);
	if(write(*socketfd,msg,9)<0)
	{
		printf("\nERROR: Writing to socket\n");
		exit(1);
	}
	printf("FILE-----SENT\n");
}

/*void FileUpload(char a[],int *socketfd)
{
	int i,n;
	char req[1000];
	sprintf(req,"Request from client: %s",a);
	puts(req);
	printf("Enter your response: ");
	scanf("%s",req);
	n=write(*socketfd,req,2048);
	if(n<0)
	{
		perror("Error while writing to socket");
	}
	if(strcmp(req,"Yes")==0)
	{
		char read_buffer[2048];
		
		
	    char *myhash,*timeStp,*size,*name;
		printf("[Server] File recieving from Client\n");

		char *token;
		token=strtok(a," ");
		token=strtok(NULL," ");
		
		bzero(read_buffer,2048); 
	
		bzero(read_buffer,2048);
		if((n= read(*socketfd, read_buffer,2048)) > 0)
		{
				if(strcmp(read_buffer,"FileError")==0)
				{
					printf("Error in file uploading\n");
					return;
				}
				if(strncmp(read_buffer,"timeStp:",10)==0)
				{
					timeStp=strtok(read_buffer," ");
					timeStp=strtok(NULL," ");
				}

		}
		else
			perror("Error in reading from socket");
		timeStp[strlen(timeStp)-1]='\0';
		
		printf("timeStp	%s\n",timeStp);
		
		bzero(read_buffer,2048);
		if(n= read(*socketfd, read_buffer,2048) > 0)
		{
				if(strncmp(read_buffer,"Size:",5)==0)
				{
					size=strtok(read_buffer," ");
					size=strtok(NULL," ");
				}
		}
		else
			perror("Error in reading from socket");
		size[strlen(size)-1]='\0';
		printf("Size		%s\n",size);
		bzero(read_buffer,2048);
		
		if(n= read(*socketfd, read_buffer,2048) > 0)
		{
				if(strncmp(read_buffer,"Name:",5)==0)
				{
					name=strtok(read_buffer," ");
					name=strtok(NULL," ");
				}
		}
		else
			perror("Error in reading from socket");
		char myarr[100];
		strcpy(myarr,name);
		printf("Name		%s\n",myarr);
		bzero(read_buffer,2048);
		

		if(n= read(*socketfd, read_buffer,2048) > 0)
		{
				if(strncmp(read_buffer,"md5sum hash:",12)==0)
				{
					myhash=strtok(read_buffer," ");
					myhash=strtok(NULL," ");
					myhash=strtok(NULL," ");
				}
		}
		else
			perror("Error in reading from socket");
		printf("Hash		%s\n",myhash);
		

//		printf("----------------------------------------------------------------------------------------\n");

		bzero(read_buffer,2048); 

		strcat(name,"UploadedFile");
		char buffer_Read[1025];
		bzero(buffer_Read,1025);

		FILE *fd;
		fd=fopen(name,"wb");
		if(fd==NULL)
		{
			perror("Could not open file");
			_exit(1);
		}
		int f=0,f1=0;
		while((f=read(*socketfd,buffer_Read,1025))>0)
		{
			if(strcmp(buffer_Read,"FILE LIST")==0)
				break;
			else
				fwrite(buffer_Read,sizeof(char),f,fd);
			bzero(buffer_Read,1025);
		}
		printf("Recieved\n");
		fclose(fd);
		return;
	}
}
*/
void peer()
{
	int socketfd, portno, wfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char read_buffer[2048],write_buffer[2048];

	bzero(write_buffer , 2048);
	
	gets(write_buffer);
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
        put_error("ERROR opening socket");
    else
	    printf("[Client] Socket created\n");

    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5002);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero),8);

    while(connect(socketfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
	    sleep(1);
    }
    while(1)
    {
    	
    	bzero(write_buffer , 2048);
    	printf("Enter the message : ");
		gets(write_buffer);
		if(strcmp(write_buffer , "Q")==0 || strcmp(write_buffer , "q")==0)
		{
			wfd = write(socketfd,write_buffer,2048);
			if (wfd < 0) 
    			put_error("ERROR writing to socket");
			printf("[Client] Connection closed\n");
			break;
		}
		else if(strncmp(write_buffer, "IndexGet", 8) == 0)
		{
			//printf("Inget");
			clientIndexget(write_buffer,&socketfd);
		}
		else if(strncmp(write_buffer, "FileHash", 8) == 0)
		{
			clientFilehash(write_buffer, &socketfd);
		}
		else if(strncmp(write_buffer, "FileDownload", 12) == 0)
		{
			clientFiledownload(write_buffer, &socketfd);
		}
		/*else if(strncmp(write_buffer, "FileUpload", 10) == 0)
		{
			clientFileUpload(write_buffer, &socketfd);
		}*/
	
		else
		{//printf("cli");
    		wfd = write(socketfd, write_buffer, 2048);
    	}
    }
close(socketfd);
   _exit(0);
    return ;
}

void clientIndexget(char write_buffer[],int *socketfd)
{
	//	printf("client...");
	int writefd;
	writefd = write(*socketfd,write_buffer,2048);
	if (writefd < 0)  
		put_error("ERROR writing to socket");
	char l[10]="--longlist",read_buffer[2048],r[7]="--regex",s[11]="--shortlist";
	int type=0,i;
	for(i=0;i<10;i++)
		if(l[i]!=write_buffer[9+i])
			type=1;
	if(type==1)
	{
		for(i=0;i<7;i++)
			if(r[i]!=write_buffer[9+i])
				type=2;
	}
	if(type==2)
	{
		for(i=0;i<11;i++)
			if(s[i]!=write_buffer[9+i])
				type=3;
	}
	if(type==0)
	{


		int f=0,flag=1;
		bzero(read_buffer, 2048);
		printf("\nRecieved data : %s\n",read_buffer);
		printf("FileName Size timeStp\n");
		while((f= read(*socketfd, read_buffer,2048)) > 0)
		{
			if(strcmp(read_buffer,"over")==0)
			{flag=0;
				break;
			}
			else
				printf("%s\n",read_buffer);
		}
		if(flag==1)
		exit(0);
	}
	if(type==1)
	{
		int f=0,flag=1;
		bzero(read_buffer, 2048);
		printf("\nRecieved data : %s\n",read_buffer);	
		printf("File-name Size\n");
		while((f= read(*socketfd, read_buffer,2048)) > 0)
		{
			if(strcmp(read_buffer,"over")==0)
			{flag=0;
				break;
			}
			else
				printf("%s\n",read_buffer);
		}
		if(flag==1)
		exit(0);
	}
	if(type==2)
	{
		int f=0,flag=1;
		bzero(read_buffer, 2048);
		printf("\nRecieved data : %s\n",read_buffer);
		printf("File-name Size Last-Modified timeStp\n");
		while((f= read(*socketfd, read_buffer,2048)) > 0)
		{
			if(strcmp(read_buffer,"over")==0)
			{flag=0;
				break;
			}
			else
				printf("%s\n",read_buffer);
		}
		if(flag==1)
		exit(0);
	}
}

void clientFilehash(char write_buffer[], int *socketfd)
{
	int readfd;
	char read_buffer[2048];
	readfd = write(*socketfd,write_buffer,2048);
	if (readfd < 0)  
		put_error("ERROR writing to socket");
	int type=0,i;
	char v[8]="--verify",c[10]="--checkall";
	for(i=0;i<8;i++)
		if(v[i]!=write_buffer[9+i])
			type=1;
	if(type==1)
	{
		for(i=0;i<10;i++)
			if(c[i]!=write_buffer[9+i])
				type=2;
	}
	if(type==0)
	{

		int f=0,flag=1;
		bzero(read_buffer, 2048);
		printf("\nRecieved data : %s\n",read_buffer);
		printf("File-name	Latest-timeStp	Md5hash\n");
		while((f= read(*socketfd, read_buffer,2048)) > 0)
		{
			if(strcmp(read_buffer,"over")==0)
			{flag=0;
				break;
			}
			else
				printf("%s\n",read_buffer);
		}
		if(flag==1)
		exit(0);
	}
	if(type==1)
	{
		int f=0,flag=1;
		bzero(read_buffer, 2048);
		printf("\nRecieved data : %s\n",read_buffer);
		printf("File-name	Latest-timeStp	Md5hash\n");
		while((f= read(*socketfd, read_buffer,2048)) > 0)
		{
			if(strcmp(read_buffer,"over")==0)
			{flag=0;
				break;
			}
			else
				printf("%s\n",read_buffer);
		}
		if(flag==1)
		exit(0);
	}
}

void clientFiledownload(char myarray[],int *socketfd)
{
	
	char read_buffer[2048];
	int i,n;
	n = write(*socketfd,myarray,2048);
    if (n < 0)
        perror("ERROR writing to socket");

    char *myhash,*timeStp,*size,*name;
	printf("[Client] File recieving from server\n");

	char *token;
	token=strtok(myarray," ");
	token=strtok(NULL," ");
	
	bzero(read_buffer,2048); 

	bzero(read_buffer,2048);
	if((n= read(*socketfd, read_buffer,2048)) > 0)
	{
			if(strcmp(read_buffer,"File not present")==0)
			{
				printf("Error: Cannot read file\n");
				return;
			}

			if(strncmp(read_buffer,"timeStp:",10)==0)
			{
				timeStp=strtok(read_buffer," ");
				timeStp=strtok(NULL," ");
			}

	}
	else
		perror("Error in reading from socket");

	timeStp[strlen(timeStp)-1]='\0';
	
	printf("timeStp	%s\n",timeStp);
	
	bzero(read_buffer,2048);
	if(n= read(*socketfd, read_buffer,2048) > 0)
	{
			if(strncmp(read_buffer,"Size:",5)==0)
			{
				size=strtok(read_buffer," ");
				size=strtok(NULL," ");
			}
	}
	else
		perror("Error in reading from socket");

	size[strlen(size)-1]='\0';
	printf("Size	%s\n",size);
	bzero(read_buffer,2048);
	
	if(n= read(*socketfd, read_buffer,2048) > 0)
	{
			if(strncmp(read_buffer,"Name:",5)==0)
			{
				name=strtok(read_buffer," ");
				name=strtok(NULL," ");
			}
	}
	else
		perror("Error in reading from socket");

	char myarr[100];
	char *arr;
	strcpy(myarr,name);
	printf("Name		%s\n",myarr);
	bzero(read_buffer,2048);
	

	if(n= read(*socketfd, read_buffer,2048) > 0)
	{
			if(strncmp(read_buffer,"md5sum hash:",12)==0)
			{
				myhash=strtok(read_buffer," ");
				myhash=strtok(NULL," ");
				myhash=strtok(NULL," ");
			}
	}
	else
		perror("Error in reading from socket");

	printf("Hash	%s\n",myhash);
	

	
	bzero(read_buffer,2048); 

	strcat(name,"MyFile");
	char buffer_Read[1025];
	bzero(buffer_Read,1025);

	FILE *fd;
	fd=fopen(name,"wb");
	if(fd==NULL)
	{
		perror("Error while opening file");
		_exit(1);
	}
	int f=0,f1=0;
	while((f=read(*socketfd,buffer_Read,1025))>0)
	{
		if(strcmp(buffer_Read,"FILE LIST")==0)
			break;
		else
			fwrite(buffer_Read,sizeof(char),f,fd);
		bzero(buffer_Read,1025);
	}
	printf("File----Recieved\n");
	fclose(fd);
	return;
}
/*
void clientFileUpload(char a[],int *socketfd)
{
	
	char read_buffer[2048];
	int i,n;
	n = write(*socketfd,a,2048);
    if (n < 0)
    {
        perror("ERROR writing to socket");
        _exit(1);
    }

    char perm[1000];
    n=read(*socketfd,perm,2048);
    if (n < 0)
    {
        perror("ERROR reading from socket");
    }

    if(strcmp(perm,"Yes")==0)
    {
	    char *myhash,*timeStp,*size,*name;
		printf("[Client] File Uploading\n");

		char write_buffer[2048];
		bzero(write_buffer,2048);
		char c[500];
		char *token=strtok(a," ");
		token=strtok(NULL," ");
	
		FILE *fx=fopen(token,"r");
		 
		if(fx==NULL)
		{
		//	printf("Here\n");
			strcpy(write_buffer,"FileError");
			n=write(*socketfd, write_buffer, 2048);
			return;
		} 
	
		sprintf(c,"stat %s | grep Modify | cut -d ' ' -f3 > result",token);
		system(c);
		char v[200];
		FILE *f1=fopen("result","r");
		if(f1==NULL)
		{
			perror("Error while opening file");
			_exit(1);
		}
		i=0;
		char cc;
		while(1)
		{
			cc = fgetc(f1);
		    if( feof(f1) )
		    {
		        break ;
		    }
		    v[i++]=cc;
		}
		v[i]='\0';
		fclose(f1);
		sprintf(write_buffer,"timeStp: %s",v);
	
		if(n=write(*socketfd,write_buffer,2048)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(write_buffer,2048);
		system("rm -rf result");

		sprintf(c,"stat %s | grep Modify | cut -d ' ' -f2 > result",token);
		system(c);
		char u[200];
		FILE *f3=fopen("result","r");
		i=0;
		while(1)
		{
			cc = fgetc(f3);
		    if( feof(f3) )
		    {
		        break ;
		    }
		    v[i++]=cc;
		}
		v[i]='\0';
		fclose(f3);
		system("rm -rf result");
		sprintf(write_buffer,"Size: %s",v);
	
		if(n=write(*socketfd,write_buffer,2048)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(write_buffer,2048);
		sprintf(write_buffer,"Name: %s",token);
	
		if(n=write(*socketfd,write_buffer,2048)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(write_buffer,2048);
		sprintf(v,"md5sum %s > result",token);
		system(v);
		f3=fopen("result","r");
		i=0;
		while(1)
		{
			cc = fgetc(f3);
		    if( feof(f3) )
		    {
		        break ;
		    }
		    v[i++]=cc;
		}
		v[i]='\0';
		system("rm -rf result");
		i=0;
		while(v[i]!=' ')
		{
			u[i++]=v[i];
		}
		u[i]='\0';
		sprintf(write_buffer,"md5sum hash: %s",u);
	//	puts(write_buffer);
		if(n=write(*socketfd,write_buffer,2048)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(write_buffer,2048);
		
	//	printf("-----------------------------------------------------------------------------\n");

		char b[1000];
		strcpy(b,token);
		FILE *fd;
		fd=fopen(b,"rb");
		char msg[1025];
		bzero(msg,1025);
		
		while((i=fread(msg,sizeof(char),1025,fd))>0)
		{
			if(write(*socketfd,msg,i)<0)
			{
				printf("ERROR: Writing to socket\n");
				break;
			}
			bzero(msg,1025);
		}
		bzero(msg,1025);
		strcpy(msg,"FILE LIST");
		sleep(1);
		if(write(*socketfd,msg,9)<0)
		{
			printf("\nERROR: Writing to socket\n");
			exit(1);
		}
		printf("SENT\n");
	}
	else
		printf("Permission Denied");
}*/

int main(int argc, char *argv[])
{
	pid=fork();
	if(pid==0)
	peer();
	else if(pid)
	host();
	
	return 0;
}

