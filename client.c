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

void Indexget(char BufferRead[],int *newsocketfd);
void Filehash(char BufferRead[] , int *newsocketfd);
void FileDownload(char BufferRead[],int *newsocketfd);
//void FileUpload(char a[],int *socketfd);

void clientIndexget(char BufferWrite[],int *socketfd);
void clientFilehash(char BufferWrite[], int *socketfd);	
void clientFiledownload(char BufferWrite[],int *socketfd);	
//void clientFileUpload(char a[],int *socketfd);


struct print_data{
    char filename[100]; //filename
    off_t size; //size
    time_t mtime; //last modified
    char type; //filetype
};
struct print_data pdata[1024];
void put_error(const char *msg)
{
    perror(msg);
    exit(0);
}

void host()
{
int socketfd, newsocketfd,readFD;
	socklen_t cli_len;
	char BufferRead[1024];
	struct sockaddr_in servi_addr, cli_addr;
	int n,i;
	char commands[1000][100];
	int flag[1000],h=0;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
		put_error("ERROR in opening socket");
	else
		printf("[Server] Socket got intialized \n");

	bzero((char *) &servi_addr, sizeof(servi_addr));
	servi_addr.sin_family = AF_INET;
	servi_addr.sin_addr.s_addr = INADDR_ANY;
	servi_addr.sin_port = htons(5002);

	if (bind(socketfd, (struct sockaddr *) &servi_addr, sizeof(servi_addr)) < 0)
		put_error("ERROR on binding");
	else
		printf("[Server] Socket Binded to the Server Address\n");

	if(listen(socketfd,5) < 0)
		put_error("ERROR in listening");

	printf("[Server] Server waiting for an client\nPress Enter to continue");
	fflush(stdout);

	cli_len = sizeof(cli_addr);
	newsocketfd = accept(socketfd,  (struct sockaddr *) &cli_addr, &cli_len);
	if (newsocketfd < 0)
		put_error("ERROR on accept");

while(1)
		{
			bzero(BufferRead,1024);
			readFD = read(newsocketfd, BufferRead, 1024);
			if(readFD>0)
			printf("Message Recieved : %s \n",BufferRead);
			if(strncmp(BufferRead, "FileHash", 8) == 0)
			{
				Filehash(BufferRead, &newsocketfd);
			}
			else if(strncmp(BufferRead , "IndexGet",8)==0)
			{

				Indexget(BufferRead,&newsocketfd);
			}
			else if(strncmp(BufferRead , "FileDownload",12)==0)
			{

				FileDownload(BufferRead,&newsocketfd);
			}
			
			
			else if(readFD<0)
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

void Indexget(char BufferRead[],int *newsocketfd)
{
	char BufferWrite[1024];
	bzero(BufferWrite,1024);
	char l[10]="--longlist",r[7]="--regex",s[11]="--shortlist";
	int type=0,i;
	for(i=0;i<10;i++)
		if(l[i]!=BufferRead[9+i])
			type=1;
	if(type==1)
	{
		for(i=0;i<7;i++)
			if(r[i]!=BufferRead[9+i])
				type=2;
		//printf("aaa");
	}
	if(type==2)
	{
		for(i=0;i<11;i++)
			if(s[i]!=BufferRead[9+i])
				type=3;

	}
	if(type==0)
	{   
		DIR *xx;
    i = 0; 
    struct dirent *ep;
    xx = opendir ("./");
    struct stat fileStat;

    if (xx != NULL)
    {
        while (ep = readdir (xx))
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

                sprintf(BufferWrite,"%-20s %-10zd %-10jd %-5c\n ",ep->d_name,fileStat.st_size,fileStat.st_mtime,type);
                write(*newsocketfd, BufferWrite, 1024);
            }
        }
        closedir (xx);
    }
    else
    {
        printf("directory can't be opened");
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

			
			
			bzero(BufferWrite, 1024);
			flag=1;
			j++;
		}
		if(flag==1)
		{
		bzero(BufferWrite, 1024);
		strcpy(BufferWrite,"over");
		if(write(*newsocketfd, BufferWrite, 1024) < 0)
		{
			put_error("\nERROR: Writing to socket");
			exit(1);
		}
		}
	
	}
	if(type==1)
	{

		//printf("t is %d",t);
		char st[500],str[400];
		int k=0;
		i=17;
		while(i!=strlen(BufferRead))
		{
			st[k]=BufferRead[i];
			k++;
			i++;
		}
		st[k]='\0';

		strcpy(str,"find . -name \""); 
		strcat(str,st);
		strcat(str,"\" -exec ls -l {} \\; | awk '{print $9\"\\t\"$5}' | cut -c "" 3- > Result");
		system("touch Result");
		system(str);

		bzero(BufferWrite, 1024); 
		int f=0,flag=0;
		FILE *fs = fopen("Result", "r");
		if(fs == NULL)
		{
			put_error("ERROR: File not found");
			exit(1);
		}
		while((f= fread(BufferWrite, sizeof(char),1024, fs)) > 0)
		{
			if(write(*newsocketfd, BufferWrite, 1024) < 0)
			{
				put_error("\nERROR: Writing to socket");
				exit(1);
				break;
			}
			bzero(BufferWrite, 1024);
			flag=1;
		}
		if(flag==1)
		{
		bzero(BufferWrite, 1024);
		strcpy(BufferWrite,"over");
		if(write(*newsocketfd, BufferWrite, 1024) < 0)
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
		while(BufferRead[i]!=' ')
		{
			st[k]=BufferRead[i];
			k++;
			i++;
		}
		st[k]='\0';
		k=0;
		i++;
		while(BufferRead[i]!='\0')
		{
			str[k]=BufferRead[i];
			k++;
			i++;
		}
		str[k]='\0';
		//printf("a%sa b%sb\n",st,str);
		system("touch fd");
		system("ls --full-time|grep -v ^d | awk '{print $9 "   " \"\\t\"$5\"\\t\"$7}' | tail -n +2 | sort -n > fd");
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
		while((f= fread(BufferWrite, sizeof(char),1024, file1)) > 0)
		{
			if(write(*newsocketfd, BufferWrite, 1024) < 0)
			{
				put_error("\nERROR: Writing to socket");
				exit(1);
				break;
			}
			bzero(BufferWrite, 1024);
			flag=1;
		}
		if(flag==1)
		{
		bzero(BufferWrite, 1024);
		strcpy(BufferWrite,"over");
		if(write(*newsocketfd, BufferWrite, 1024) < 0)
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
void FileDownload(char myarray[],int *socketfd)
{
	
	char BufferWrite[1024];
	char *tokvar=strtok(myarray," ");
	bzero(BufferWrite,1024);
	char c[500];
	tokvar=strtok(NULL," ");

	int n,i;

	FILE *fp=fopen(tokvar,"r");
	 
	if(fp==NULL)
	{
		printf("File can't be opened\n");
		strcpy(BufferWrite,"NoFile is there");
		n=write(*socketfd, BufferWrite, 1024);
		return;
	} 
//	puts(a);
	sprintf(c,"stat %s | grep Modify | cut -d ' ' -f3 > result",tokvar);
	system(c);
	char v[200];
	
	i=0;
	char x;
	FILE *filex=fopen("result","r");
	while(1)
	{
		x = fgetc(filex);
	    if( feof(filex) )
	    {
	        break ;
	    }
	    v[i++]=x;
	}
	v[i]='\0';
	fclose(filex);
	sprintf(BufferWrite,"TimeStamp: %s",v);
	puts(BufferWrite);
	if(n=write(*socketfd,BufferWrite,1024)<0)
	{
		perror("Error in writing in socket");
	}
	bzero(BufferWrite,1024);
	system("rm -rf result");

	sprintf(c,"stat %s | grep Modify | cut -d ' ' -f2 > result",tokvar);
	system(c);
	char u[200];
	FILE *f3=fopen("result","r");
	i=0;
	char temp;
	while(1)
	{
		temp = fgetc(f3);
	    if( feof(f3) )
	    {
	        break ;
	    }
	    v[i++]=temp;
	}
	v[i]='\0';
	fclose(f3);
	system("rm -rf result");
	sprintf(BufferWrite,"Size: %s",v);
	puts(BufferWrite);
	if(n=write(*socketfd,BufferWrite,1024)<0)
	{
		perror("Error in writing in socket");
	}
	bzero(BufferWrite,1024);
	sprintf(BufferWrite,"Name: %s",tokvar);
	puts(BufferWrite);
	if(n=write(*socketfd,BufferWrite,1024)<0)
	{
		perror("Error in writing in socket");
	}
	bzero(BufferWrite,1024);
	sprintf(v,"md5sum %s > result",tokvar);
	system(v);
	f3=fopen("result","r");
	i=0;
	while(1)
	{
		temp = fgetc(f3);
	    if( feof(f3) )
	    {
	        break ;
	    }
	    v[i++]=temp;
	}
	v[i]='\0';
	system("rm -rf result");
	/*i=0;
	while(v[i]!=' ')
	{
		u[i++]=v[i];
	}
	u[i]='\0';
	sprintf(BufferWrite,"md5sum hash: %s",u);
	puts(BufferWrite);
	write(*socketfd,BufferWrite,1024);
	bzero(BufferWrite,1024);
	
	char b[1000];
	strcpy(b,tokvar);
	FILE *fd;
	fd=fopen(b,"rb");
	char msg[1025];
	bzero(msg,1025);
	
	while((i=fread(msg,sizeof(char),1025,fd))>0)
	{
		write(*socketfd,msg,i);
		bzero(msg,1025);
	}
	bzero(msg,1025);
	strcpy(msg,"listFILE");
	sleep(1);
	if(write(*socketfd,msg,9)<0)
	{
		printf("\nERROR: Writing to socket\n");
		exit(1);
	}*/
	printf("---------SENT------------\n");
}

void doHashing(char my[],int *newsocketfd)
{
	int i;
}


void Filehash(char BufferRead[] , int *newsocketfd)
{
	char BufferWrite[1024];
	bzero(BufferWrite,1024);
	char v[8]="--verify",c[10]="--checkall";
	int type=0,i;
	for(i=0;i<8;i++)
		if(v[i]!=BufferRead[9+i])
			type=1;
	if(type==1)
	{
		for(i=0;i<10;i++)
			if(c[i]!=BufferRead[9+i])
				type=2;
	}
	if(type==0)
	{
		char my[500];
		int k=0;
		i=18;
		while(i!=strlen(BufferRead))
		{
			my[k]=BufferRead[i];
			k++;
			i++;
		}
		my[k]='\0';
		doHashing(my,newsocketfd);
		bzero(BufferWrite,1024);
		strcpy(BufferWrite,"over");
		if(write(*newsocketfd, BufferWrite, 1024) < 0)
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
		bzero(BufferWrite, 1024);
		strcpy(BufferWrite,"over");
		if(write(*newsocketfd, BufferWrite, 1024) < 0)
		{
			put_error("\nERROR: Writing to socket");
			exit(1);
		}
		}
		system("rm -rf fd");

	}

}

/*void FileUpload(char a[],int *socketfd)
{
	int i,n;
	char req[1000];
	sprintf(req,"Request from client: %s",a);
	puts(req);
	printf("Enter your response: ");
	scanf("%s",req);
	n=write(*socketfd,req,1024);
	if(n<0)
	{
		perror("Error while writing to socket");
	}
	if(strcmp(req,"Yes")==0)
	{
		char BufferRead[1024];
		
		
	    char *myHash,*timestamp,*size,*name;
		printf("[Server] File recieving from Client\n");

		char *tokvar;
		tokvar=strtok(a," ");
		tokvar=strtok(NULL," ");
		
		bzero(BufferRead,1024); 
	
		bzero(BufferRead,1024);
		if((n= read(*socketfd, BufferRead,1024)) > 0)
		{
				if(strcmp(BufferRead,"FileError")==0)
				{
					printf("Error in file uploading\n");
					return;
				}
				if(strncmp(BufferRead,"TimeStamp:",10)==0)
				{
					timestamp=strtok(BufferRead," ");
					timestamp=strtok(NULL," ");
				}

		}
		else
			perror("Error in reading from socket");
		timestamp[strlen(timestamp)-1]='\0';
		
		printf("Timestamp	%s\n",timestamp);
		
		bzero(BufferRead,1024);
		if(n= read(*socketfd, BufferRead,1024) > 0)
		{
				if(strncmp(BufferRead,"Size:",5)==0)
				{
					size=strtok(BufferRead," ");
					size=strtok(NULL," ");
				}
		}
		else
			perror("Error in reading from socket");
		size[strlen(size)-1]='\0';
		printf("Size		%s\n",size);
		bzero(BufferRead,1024);
		
		if(n= read(*socketfd, BufferRead,1024) > 0)
		{
				if(strncmp(BufferRead,"Name:",5)==0)
				{
					name=strtok(BufferRead," ");
					name=strtok(NULL," ");
				}
		}
		else
			perror("Error in reading from socket");
		char name2[100];
		strcpy(name2,name);
		printf("Name		%s\n",name2);
		bzero(BufferRead,1024);
		

		if(n= read(*socketfd, BufferRead,1024) > 0)
		{
				if(strncmp(BufferRead,"md5sum hash:",12)==0)
				{
					myHash=strtok(BufferRead," ");
					myHash=strtok(NULL," ");
					myHash=strtok(NULL," ");
				}
		}
		else
			perror("Error in reading from socket");
		printf("Hash		%s\n",myHash);
		

//		printf("----------------------------------------------------------------------------------------\n");

		bzero(BufferRead,1024); 

		strcat(name,"UploadedFile");
		char myReadBuffer[1025];
		bzero(myReadBuffer,1025);

		FILE *fd;
		fd=fopen(name,"wb");
		if(fd==NULL)
		{
			perror("Could not open file");
			_exit(1);
		}
		int f=0,filex=0;
		while((f=read(*socketfd,myReadBuffer,1025))>0)
		{
			if(strcmp(myReadBuffer,"listFILE")==0)
				break;
			else
				fwrite(myReadBuffer,sizeof(char),f,fd);
			bzero(myReadBuffer,1025);
		}
		printf("Recieved\n");
		fclose(fd);
		return;
	}
}
*/
void peer()
{
	int socketfd, portno, writeFD;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char BufferRead[1024],BufferWrite[1024];

	bzero(BufferWrite , 1024);
	
	gets(BufferWrite);
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
    serv_addr.sin_port = htons(5005);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero),8);

    while(connect(socketfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
	    sleep(1);
    }
    while(1)
    {
    	
    	bzero(BufferWrite , 1024);
    	printf("Enter the message : ");
		gets(BufferWrite);
		if(strcmp(BufferWrite , "quit")==0 )
		{
			writeFD = write(socketfd,BufferWrite,1024);
			if (writeFD < 0) 
    			put_error("ERROR writing to socket");
			printf("[Client] Connection closed\n");
			break;
		}
		else if(strncmp(BufferWrite, "IndexGet", 8) == 0)
		{
			//printf("Inget");
			clientIndexget(BufferWrite,&socketfd);
		}
		else if(strncmp(BufferWrite, "FileHash", 8) == 0)
		{
			clientFilehash(BufferWrite, &socketfd);
		}
		else if(strncmp(BufferWrite, "FileDownload", 12) == 0)
		{
			clientFiledownload(BufferWrite, &socketfd);
		}
		/*else if(strncmp(BufferWrite, "FileUpload", 10) == 0)
		{
			clientFileUpload(BufferWrite, &socketfd);
		}*/
	
		else
		{//printf("cli");
    		writeFD = write(socketfd, BufferWrite, 1024);
    	}
    }
close(socketfd);
   _exit(0);
    return ;
}

void clientIndexget(char BufferWrite[],int *socketfd)
{
	//	printf("client...");
	int writefd;
	writefd = write(*socketfd,BufferWrite,1024);
	if (writefd < 0)  
		put_error("ERROR writing to socket");
	char l[10]="--longlist",BufferRead[1024],r[7]="--regex",s[11]="--shortlist";
	int type=0,i;
	for(i=0;i<10;i++)
		if(l[i]!=BufferWrite[9+i])
			type=1;
	if(type==1)
	{
		for(i=0;i<7;i++)
			if(r[i]!=BufferWrite[9+i])
				type=2;
	}
	if(type==2)
	{
		for(i=0;i<11;i++)
			if(s[i]!=BufferWrite[9+i])
				type=3;
	}
	if(type==0)
	{


		int f=0,flag=1;
		bzero(BufferRead, 1024);
		printf("\nRecieved data : %s\n",BufferRead);
		printf("FileName Size Timestamp\n");
		while((f= read(*socketfd, BufferRead,1024)) > 0)
		{
			if(strcmp(BufferRead,"over")==0)
			{flag=0;
				break;
			}
			else
				printf("%s\n",BufferRead);
		}
		if(flag==1)
		exit(0);
	}
	if(type==1)
	{
		int f=0,flag=1;
		bzero(BufferRead, 1024);
		printf("\nRecieved data : %s\n",BufferRead);	
		printf("File-name Size\n");
		while((f= read(*socketfd, BufferRead,1024)) > 0)
		{
			if(strcmp(BufferRead,"over")==0)
			{flag=0;
				break;
			}
			else
				printf("%s\n",BufferRead);
		}
		if(flag==1)
		exit(0);
	}
}

void clientFilehash(char BufferWrite[], int *socketfd)
{
	int readfd;
	char BufferRead[1024];
	readfd = write(*socketfd,BufferWrite,1024);
	if (readfd < 0)  
		put_error("ERROR writing to socket");
	int type=0,i;
	char v[8]="--verify",c[10]="--checkall";
	for(i=0;i<8;i++)
		if(v[i]!=BufferWrite[9+i])
			type=1;
	if(type==1)
	{
		for(i=0;i<10;i++)
			if(c[i]!=BufferWrite[9+i])
				type=2;
	}
	if(type==0)
	{

		int f=0,flag=1;
		bzero(BufferRead, 1024);
		printf("\nRecieved data : %s\n",BufferRead);
		printf("File-name	Latest-Timestamp	Md5hash\n");
		while((f= read(*socketfd, BufferRead,1024)) > 0)
		{
			if(strcmp(BufferRead,"over")==0)
			{flag=0;
				break;
			}
			else
				printf("%s\n",BufferRead);
		}
		if(flag==1)
		exit(0);
	}
	if(type==1)
	{
		int f=0,flag=1;
		bzero(BufferRead, 1024);
		printf("\nRecieved data : %s\n",BufferRead);
		printf("File-name	Latest-Timestamp	Md5hash\n");
		while((f= read(*socketfd, BufferRead,1024)) > 0)
		{
			if(strcmp(BufferRead,"over")==0)
			{flag=0;
				break;
			}
			else
				printf("%s\n",BufferRead);
		}
		if(flag==1)
		exit(0);
	}
}

void clientFiledownload(char myarray[],int *socketfd)
{
	
	char BufferRead[1024];
	int i,n;
	n = write(*socketfd,myarray,1024);
    if (n < 0)
        perror("ERROR writing to socket");

    char *myHash,*timestamp,*size,*name;
	printf("[Client] File recieving from server\n");

	char *tokvar;
	tokvar=strtok(myarray," ");
	tokvar=strtok(NULL," ");
	
	bzero(BufferRead,1024); 

	bzero(BufferRead,1024);
	if((n= read(*socketfd, BufferRead,1024)) > 0)
	{
			if(strcmp(BufferRead,"NoFile")==0)
			{
				printf("Error: Cannot read file\n");
				return;
			}

			if(strncmp(BufferRead,"TimeStamp:",10)==0)
			{
				timestamp=strtok(BufferRead," ");
				timestamp=strtok(NULL," ");
			}

	}
	else
		perror("Error in reading from socket");

	
	bzero(BufferRead,1024);
	if(n= read(*socketfd, BufferRead,1024) > 0)
	{
			if(strncmp(BufferRead,"Size:",5)==0)
			{
				size=strtok(BufferRead," ");
				size=strtok(NULL," ");
			}
	}
	else
		perror("Error in reading from socket");

	size[strlen(size)-1]='\0';
	printf("Size		%s\n",size);
	bzero(BufferRead,1024);
	
	if(n= read(*socketfd, BufferRead,1024) > 0)
	{
			if(strncmp(BufferRead,"Name:",5)==0)
			{
				name=strtok(BufferRead," ");
				name=strtok(NULL," ");
			}
	}
	else
		perror("Error in reading from socket");

	char name2[100];
	strcpy(name2,name);
	printf("Name		%s\n",name2);
	bzero(BufferRead,1024);
	

	if(n= read(*socketfd, BufferRead,1024) > 0)
	{
			if(strncmp(BufferRead,"md5sum hash:",12)==0)
			{
				myHash=strtok(BufferRead," ");
				myHash=strtok(NULL," ");
				myHash=strtok(NULL," ");
			}
	}
	else
		perror("Error in reading from socket");

	printf("Hash		%s\n",myHash);
	

	
	bzero(BufferRead,1024); 

	strcat(name,"downloaded");
	char myReadBuffer[1025];
	bzero(myReadBuffer,1025);

	FILE *fd;
	fd=fopen(name,"wb");
	if(fd==NULL)
	{
		perror("Error while opening file");
		_exit(1);
	}
	int f=0,filex=0;
	while((f=read(*socketfd,myReadBuffer,1025))>0)
	{
		if(strcmp(myReadBuffer,"listFILE")==0)
			break;
		else
			fwrite(myReadBuffer,sizeof(char),f,fd);
		bzero(myReadBuffer,1025);
	}
	printf("File---Recieved---\n");
	fclose(fd);
	return;
}

/*void clientFileUpload(char a[],int *socketfd)
{
	
	char BufferRead[1024];
	int i,n;
	n = write(*socketfd,a,1024);
    if (n < 0)
    {
        perror("ERROR writing to socket");
        _exit(1);
    }

    char perm[1000];
    n=read(*socketfd,perm,1024);
    if (n < 0)
    {
        perror("ERROR reading from socket");
    }

    if(strcmp(perm,"Yes")==0)
    {
	    char *myHash,*timestamp,*size,*name;
		printf("[Client] File Uploading\n");

		char BufferWrite[1024];
		bzero(BufferWrite,1024);
		char c[500];
		char *tokvar=strtok(a," ");
		tokvar=strtok(NULL," ");
	
		FILE *fx=fopen(tokvar,"r");
		 
		if(fx==NULL)
		{
		//	printf("Here\n");
			strcpy(BufferWrite,"FileError");
			n=write(*socketfd, BufferWrite, 1024);
			return;
		} 
	
		sprintf(c,"stat %s | grep Modify | cut -d ' ' -f3 > result",tokvar);
		system(c);
		char v[200];
		FILE *filex=fopen("result","r");
		if(filex==NULL)
		{
			perror("Error while opening file");
			_exit(1);
		}
		i=0;
		char cc;
		while(1)
		{
			cc = fgetc(filex);
		    if( feof(filex) )
		    {
		        break ;
		    }
		    v[i++]=cc;
		}
		v[i]='\0';
		fclose(filex);
		sprintf(BufferWrite,"TimeStamp: %s",v);
	
		if(n=write(*socketfd,BufferWrite,1024)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(BufferWrite,1024);
		system("rm -rf result");

		sprintf(c,"stat %s | grep Modify | cut -d ' ' -f2 > result",tokvar);
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
		sprintf(BufferWrite,"Size: %s",v);
	
		if(n=write(*socketfd,BufferWrite,1024)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(BufferWrite,1024);
		sprintf(BufferWrite,"Name: %s",tokvar);
	
		if(n=write(*socketfd,BufferWrite,1024)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(BufferWrite,1024);
		sprintf(v,"md5sum %s > result",tokvar);
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
		sprintf(BufferWrite,"md5sum hash: %s",u);
		if(n=write(*socketfd,BufferWrite,1024)<0)
		{
			perror("Error in writing in socket");
		}
		bzero(BufferWrite,1024);
		
	//	printf("-----------------------------------------------------------------------------\n");

		char b[1000];
		strcpy(b,tokvar);
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
		strcpy(msg,"listFILE");
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
}
*/
int main(int argc, char *argv[])
{
	char sec[10];
	printf("Enter the protocol TCP/UDP :");
	//scanf("%s",sec);
	pid=fork();
	if(pid==0)
	peer();
	else if(pid)
	host();
	
	return 0;
}

