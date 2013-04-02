#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<pthread.h>

#define QT_PROT 8001
#define BOARD 8000

char buff[6] = {0};
int size_byte=sizeof(struct sockaddr_in);

void server_recv_for_control()
{
	struct sockaddr_in server_addr,client_addr;
	int sockfd,clientfd,recv_byte;
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket error");
		exit(1);
	}
	server_addr.sin_family		=AF_INET;
	server_addr.sin_port		=htons(QT_PROT);
	server_addr.sin_addr.s_addr	=INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);

	int i;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&i,sizeof(i));

	if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr_in)))
	{
		perror("bind error for recv\n");
		exit(1);
	}
	
	if(listen(sockfd,5)==-1)
	{
		perror("listen error\n");
		exit(1);
	}

	for(;;)
	{
		if((clientfd=accept(sockfd,(struct sockaddr*)&client_addr,&size_byte))==-1)
		{
			perror("accept error\n");
			exit(1);
		}
		
		while(1)
		{
			recv_byte=recv(clientfd,buff,6,0);
			if(recv_byte==-1)
			{
				perror("recv error\n");
				sleep(1);
				break;
			}
			if(recv_byte==0)
			{
				printf("recv end\n");
				break;
			}
			if(recv_byte>0)
			{
				printf("recv %s\n",buff);
				sleep(1);
			}
		}
		close(clientfd);
	}
	close(clientfd);
	printf("recv pthread end\n");
}

void server_send_to_borad()
{
	struct sockaddr_in server_addr,client_addr;
	int sockfd2,clientfd2,send_byte;

	if((sockfd2=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket error\n");
		exit(1);
	}
	server_addr.sin_family		=AF_INET;
	server_addr.sin_port		=htons(BOARD);
	server_addr.sin_addr.s_addr	=INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);

	int i;
	setsockopt(sockfd2,SOL_SOCKET,SO_REUSEADDR,&i,sizeof(i));

	if(bind(sockfd2,(struct sockaddr*)&server_addr,sizeof(struct sockaddr_in)))
	{
		perror("bind error for send\n");
		exit(1);
	}

	if(listen(sockfd2,5)==-1)
	{
		perror("listen\n");
		exit(1);
	}

	for(;;)
	{
		if((clientfd2=accept(sockfd2,(struct sockaddr *)&client_addr,&size_byte))==-1)
		{
			perror("accept\n");
			exit(1);	
		}

		printf("create a new clientfd\n");

		while(1)
		{
			send_byte=send(clientfd2,buff,6,0);
			if(send_byte==-1)
			{
				perror("send\n");
				sleep(1);
			}
			if(send_byte==0)
			{
				printf("send end!\n");
			}
			if(send_byte>0)
			{
				printf("send %s\n",buff);
				printf("send successful\n");
				sleep(1);
				break;
			}
		}
		close(clientfd2);
	}
	close(sockfd2);
	printf("send pthread end");
}


int main()
{
	pthread_t recvid,sendid;
	int ret1,ret2;
	while(1)
	{
		ret1=pthread_create(&recvid,NULL,(void*)server_recv_for_control,(void*)NULL);
		if(ret1!=0)
		{
			printf("create recv error\n");
			sleep(1);
			break;
		}
		ret2=pthread_create(&sendid,NULL,(void*)server_send_to_borad,(void*)NULL);
		if(ret2!=0)
		{
			printf("create send error\n");
			sleep(1);
			break;
		}
		printf("in main\n");
		ret1=pthread_join(recvid,NULL);
		ret2=pthread_join(sendid,NULL);
	}
	return 0;
}
