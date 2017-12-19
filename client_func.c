/*client_func.c*/  
  
#include "client_func.h"  
int tcp_connect(const char* ip, int port)  
{  
    int sfd = socket(AF_INET, SOCK_STREAM, 0);  
    if(sfd == -1)  
    {  
        printf("socket error: %s", strerror(errno));  
  
        return -1;  
    }  
      
    struct sockaddr_in serveraddr;  
    memset(&serveraddr, 0, sizeof(struct sockaddr) );  
      
    serveraddr.sin_family = AF_INET;  
    serveraddr.sin_port = htons(port);  
    serveraddr.sin_addr.s_addr = inet_addr(ip);  
  
    if(connect(sfd, (struct sockaddr*) &serveraddr, sizeof(struct sockaddr)) == -1 )  
    {  
        printf("connect error: %s", strerror(errno));  
        close(sfd);   
  
        return -1;  
    }  
  
    return sfd;  
}  
  
void* recv_file(void *arg)  
{  
    int needRecv = sizeof(T_CLIENT_COM_HEADER);  
    int length = 0;  
    long pos = 0;   
      
    char *head = (char*)malloc(needRecv);  
    char buffer[MAX_SIZE];  
      
    file_information *file_info;  
    T_CLIENT_COM_HEADER *myNode = (T_CLIENT_COM_HEADER*)malloc(sizeof(T_CLIENT_COM_HEADER));  
      
    file_info = ( file_information *)arg;  
    char *filename = basename(file_info->filename);  
    memset(&buffer, 0, MAX_SIZE);  
      
    while(pos < needRecv)  
    {  
        length = recv(file_info->cfd, head+pos, needRecv, 0);  
        if (length < 0)  
        {  
            printf("Server Recieve Data Failed!\n");  
            break;  
        }  
        pos += length;  
    }  
    memcpy(myNode,head,needRecv);  
    if(myNode->cmd_id == CMD_FILE_EXIST)  
    {  
        printf("File %s is sending...\n", filename);  
        filename = NULL;  
        FILE *fp = fopen(file_info->filename, "w");    
        length = 0;  
        pos = 0;  
          
        if (fp == NULL)    
        {  
            printf("File:\t%s Can Not Open To Write!\n", file_info->filename);    
            return;  
        }  
  
        while(pos < myNode->length - needRecv)  
        {  
            int write_length;  
            length = recv(file_info->cfd, buffer, MAX_SIZE, 0);  
            if (length < 0)  
            {  
                printf("Recieve Data From Server Failed!\n");    
                break;  
            }  
              
            pos += length;  
            write_length = fwrite(buffer, sizeof(char), length, fp);    
              
            if (write_length < length)  
            {  
                printf("File:\t%s Write Failed!\n", file_info->filename);    
                break;    
            }  
            bzero(buffer, MAX_SIZE);    
        }  
          
        printf("Recieve File: %s\tFrom Server Finished!\n", file_info->filename);    
    
        fclose(fp);  
    }  
    else if(myNode->cmd_id == CMD_FILE_NOT_EXIST)  
    {  
        printf("File %s is not exist!\n", file_info->filename);  
    }  
    free(head);  
    free(myNode);  
}  
  
void* pthread_send(void *arg)  
{  
    int *temp;  
    char buf[128];  
  
    memset(buf, 0, sizeof(buf));  
    temp = (int *)arg;   
      
    while(1)  
    {  
        char commond[64];  
        char filename[128];  
        char save_path[128];  
        int arg_cont;  
        gets(buf);  
  
          
          
        if(-1 == send(*temp, buf, sizeof(buf), 0))  
        {  
            printf("send error: %s", strerror(errno));  
            close(*temp);  
  
            return;  
        }  
          
        if(0 == strcmp(buf, CMD_EXIT))  
        {  
            break;  
        }  
  
        if(((arg_cont = sscanf(buf, "%s %s %s", commond, filename, save_path)) != 0) && (0 == strcmp(commond, CMD_DOWNLOAD_FILE)))  
        {  
            if(arg_cont != 3)  
            {  
                printf("parameter error!\n");  
                continue;  
            }  
            file_information *file_info;  
            pthread_t pid;  
            int ret;  
              
            file_info = (file_information *)malloc(sizeof(file_information));  
            memset(file_info, 0, sizeof(file_information));  
              
            strcpy(file_info->filename, save_path);  
            file_info->cfd = *temp;  
              
            ret = pthread_create(&pid, NULL, (void*)recv_file, (void*)file_info);  
            if(ret != 0)  
            {  
                printf("create pthread error!\n");  
            }  
        }  
          
    }  
    close(*temp);  
    pthread_exit(NULL);  
}