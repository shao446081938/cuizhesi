    /*server_func.c*/  
      
    #include "server_func.h"  
    int tcp_init(const char* ip, int port)  //用于初始化  
    {  
        int on = 1;  
        int sfd = socket(AF_INET, SOCK_STREAM, 0); //首先建立一个Socket，向系统申请  
        if(sfd == -1)  
        {  
            printf("socket error: %s\n", strerror(errno));  
      
            return -1;  
        }  
      
        int ret = setsockopt( sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );  
        if(ret < 0)  
        {  
            printf("setsockopt error: %s", strerror(errno));  
        }  
      
        struct sockaddr_in serveraddr;  
        memset( &serveraddr, 0, sizeof(struct sockaddr) );   
        serveraddr.sin_family = AF_INET;  
        serveraddr.sin_port = htons(port);   
        serveraddr.sin_addr.s_addr = INADDR_ANY; //或者INADDR_ANY  
          
        if(bind(sfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr)) == -1)  
        {  
            printf("Bind error: %s\n", strerror(errno));  
            close(sfd);  
              
            return -1;  
        }  
          
        if(listen(sfd, MAX_CON) == -1)  //监听它 最大连接数为10  
        {  
            printf("Listen error: %s\n", strerror(errno));  
            close(sfd);  
              
            return -1;  
        }  
          
        return sfd;  
    }  
      
    int tcp_accept(int sfd)  
    {  
        struct sockaddr_in clientaddr;  
        int addrlen = sizeof(struct sockaddr);  
        int new_fd = accept(sfd, (struct sockaddr*)&clientaddr, &addrlen);  
          
        memset(&clientaddr, 0, addrlen);  
          
        if(new_fd == -1)  
        {  
            printf("accept error: %s\n", strerror(errno));  
            sleep(1);  
            return -1;  
        }  
        printf("Client%d(%s %d) success connect...\n", new_fd, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));  
          
        return new_fd;  
    }  
      
    void * send_file(void * arg)  
    {  
        int needSend = sizeof(T_CLIENT_COM_HEADER);  
        char *head = (char*)malloc(needSend);  
        char buffer[MAX_SIZE];  
        T_CLIENT_COM_HEADER *com = (T_CLIENT_COM_HEADER*)malloc(sizeof(T_CLIENT_COM_HEADER));   
        tcp_info *client_info;  
          
        client_info = ( tcp_info *)arg;  
        com->length = needSend;  
        memset(buffer, 0, sizeof(buffer));  
          
        if(access(client_info->filename, F_OK) >= 0)  
        {  
            FILE *fp = fopen(client_info->filename, "r");  
            fseek(fp, 0L, SEEK_END);    
            long file_size = ftell(fp);  
              
            rewind(fp);  
              
            com->length += file_size;  
            com->cmd_id = CMD_FILE_EXIST;  
              
            memcpy(head, com, needSend);  
              
            int sendlength = send(client_info->cfd, head, needSend, 0);  
            printf("head->cmd_id = %d, head->length = %ld\n", com->cmd_id, com->length);  
            do  
            {  
                int file_block_length = fread(buffer, sizeof(char), MAX_SIZE, fp);  
                  
                int len = send(client_info->cfd, buffer, file_block_length, 0);  
      
                if(file_block_length <= 0)  
                {  
                    break;  
                }  
                bzero(buffer, sizeof(buffer));    
            }while(1);  
      
            fclose(fp);  
            printf("File:\t%s \nTransfer Finished!\n", client_info->filename);    
              
        }  
        else  
        {  
            com->cmd_id = CMD_FILE_NOT_EXIST;  
            memcpy(head, com, needSend);  
            int sendlength = send(client_info->cfd, head, needSend, 0);  
        }  
    }  
      
      
      
    void* pthread_recv(void *arg)  
    {  
        char buf[128] = {0};  
        tcp_info *client_info;  
          
        client_info = ( tcp_info *)arg;  
        memset(buf, 0, sizeof(buf));  
      
        while(1)  
        {  
            char commond[64];  
            char filename[128];  
            int ret = recv(client_info->cfd, buf, sizeof(buf), 0 );  
      
            if (ret < 0)  
            {  
                printf("receive error: %s\n", strerror(errno));  
                  
                break;  
            }  
            else if(ret == 0)  
            {  
                printf("Client%d(Exception) exit!\n", client_info->cfd);  
                break;  
            }  
      
            if(0 == strcmp(buf, CMD_EXIT))  
            {  
                printf("Client%d exit!\n", client_info->cfd);  
                break;  
            }  
              
            if((sscanf(buf, "%s %s", commond, filename) != 0) && (0 == strcmp(commond, CMD_DOWNLOAD_FILE)))  
            {  
                pthread_t pid;  
                int ret;  
                int length = 0;  
                  
                strcpy(client_info->filename, filename);  
                  
                ret = pthread_create(&pid, NULL, send_file, (void*)client_info);  
                  
                continue;  
            }  
              
            printf("Client%d: %s\n", client_info->cfd, buf);  
        }  
        close(client_info->cfd);  
        free(client_info);  
        pthread_exit(NULL);  
    }  
      
    void signalhandler(void)    
    {    
        sigset_t sigSet;    
        sigemptyset(&sigSet);    
        sigaddset(&sigSet,SIGINT);    
        sigaddset(&sigSet,SIGQUIT);    
        sigaddset(&sigSet,SIGPIPE);    
        sigprocmask(SIG_BLOCK,&sigSet,NULL);        
    }    