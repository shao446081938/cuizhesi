#include<stdio.h>
#include"unp.h"
int
main(int argc,char **argv)
{
	int maxfd,maxi,i;
	int listenfd,connfd,sockfd,wsockfd;
	int client[FD_SETSIZE];
	ssize_t n;
	fd_set rset,wset,allset;
	char buf[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;
	listenfd=Socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(SERV_PORT);

	Bind(listenfd,(SA*)&servaddr,sizeof(servaddr));
	Listen(listenfd,LISTENQ);

	maxfd=listenfd;
	maxi=-1;
	for(int i=0;i<FD_SETSIZE;i++)
		client[i]=-1;
	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);

	for(;;){
		rset=allset;
		wset=allset;
		Select(maxfd+1,&rset,&wset,NULL,NULL);
		if(FD_ISSET(listenfd,&rset)){
			clilen=sizeof(cliaddr);
			connfd=Accept(listenfd,(SA*)&cliaddr,&clilen);
			for(i=0;i<FD_SETSIZE;i++)
				if(client[i]<0){
					client[i]=connfd;
					break;
				}
			FD_SET(connfd,&allset);
			if(connfd>maxfd)
				maxfd=connfd;
			if(i>maxi)
				maxi=i;
		}
		for(i=0;i<=maxi;i++){
			if((sockfd=client[i])<0)
				continue;
			if(FD_ISSET(sockfd,&rset)){
				if((n=Read(sockfd,buf,MAXLINE))==0){
					Close(sockfd);
					FD_CLR(sockfd,&allset);
					client[i]=-1;
				}else
					for(int i=0;i<=maxi;i++){
						if((wsockfd=client[i])!=sockfd)
							Writen(wsockfd,buf,n);
					}
			}
		}
	}
}
