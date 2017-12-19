voidvoid *recvmation(voidvoid *arg) //接收客户端发送过来的信息，线程  
{  
    int my_fd = *((intint *)arg);  
    int n;  
          
    while(1)  
    {     
        n = recv(my_fd, (struct regis *)&RecvReg, sizeof(RecvReg), 0);  
        if(n == -1)  
        {  
            perror("fail1 to receive!\n");  
            //exit(1);  
        }  
        else if(n == 0)  
        {  
            continue;   //如果客户端断开连接，服务器继续监听接收  
        }  
  
        printf("filename:%s\n", RecvReg.filename);  
        printf("flag = %s\n", RecvReg.flag);  
        printf("my_fd = %d\n", my_fd);  
  
        if(strncmp(RecvReg.flag, "sendfile", 8) == 0)   //客户端上传文件到服务器  
        {  
            char Send_Filename[FILE_NAME];  
            strcpy(Send_Filename, RecvReg.filename);  
            recvfile(my_fd, Send_Filename);   //接受客户端发来的文件  
        }  
        if(strncmp(RecvReg.flag, "lookfile", 8) == 0)   //客户端从服务器下载文件  
        {  
            char Down_Filename[FILE_NAME];  
            DIR *dp = NULL;  
            struct dirent *dirp;  
            if((dp = opendir("/root/server/dai")) == NULL)    //查找并输出当前文件夹下的所有文件 //此路径可以根据自己需要修改  
                printf("cannot open\n");  
            while((dirp = readdir(dp)) != NULL)  
            {  
                printf("%s\n",dirp->d_name);  
                strcpy(D_File.Dir_File, dirp->d_name);  
                if(send(my_fd, (struct DIR_FILE *)&D_File, sizeof(D_File), 0) < 0)  
                    break;  
                usleep(1000);   //睡眠函数延迟，这个必须有，因为是非阻塞  
                bzero((struct DIR_FILE *)&D_File, sizeof(D_File));  
            }  
            closedir(dp);  
            send(my_fd, "OK", 2, 0);  
        }  
        if(strncmp(RecvReg.flag, "downfile", 8) == 0)   //客户端从服务器下载文件  
        {  
            char Down_Filename[FILE_NAME];  
            strcpy(Down_Filename, RecvReg.filename);  
            sendfile(my_fd, Down_Filename);   //客户端从服务器下载文件  
        }  
    }  
      
    close(my_fd);           //完成所有操作后，关闭该文件描述符  
}  
  
void sendfile(int sfd, char Down_Filename[])   //向客户端发送文件  
{  
    char file_name[FILE_NAME];  
    char buffer[BUFFER_SIZE];        
    char Path[Path_SIZE];  
      
    strcpy(file_name, Down_Filename);  
     
    printf("%s\n", file_name);    
    sprintf(Path, "/root/server/dai/%s", file_name);       //此路径可以根据自己需要修改  
    // 打开文件并读取文件数据        
    FILEFILE *fp = fopen(Path, "r");     
  
    if(NULL == fp)        
    {          
        printf("File:%s Not Found\n", file_name);        
    }        
    else       
    {          
        bzero(buffer, BUFFER_SIZE);          
        int length = 0;        // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止   
  
        while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)          
        {            
            printf("length = %d\n", length);  
            if(send(sfd, buffer, length, 0) < 0)            
            {              
                printf("Send File:%s Failed.\n", file_name);              
                break;            
            }            
            bzero(buffer, BUFFER_SIZE);          
        }           // 关闭文件          
        fclose(fp);     
        sleep(1);//延迟  //睡眠函数延迟，这个必须有，因为是非阻塞  
        send(sfd, "OK", 2, 0);     
        printf("File:%s Transfer Successful!\n", file_name);        
    }      // 关闭与客户端的连接        
}  
  
void recvfile(int sfd, char Send_Filename[])   //接受客户端发来的文件  
{  
    char file_name[FILE_NAME];  
    char buffer[BUFFER_SIZE];  
    char Path[Path_SIZE];  
    strcpy(file_name, Send_Filename);  
    
    printf("%s\n", file_name);     
    sprintf(Path, "/root/server/dai/%s", file_name);      //此路径可以根据自己需要修改  
    // 打开文件并读取文件数据        
    FILEFILE *fp = fopen(Path, "w");     
  
    if(NULL == fp)        
    {          
        printf("File:%s Not Found\n", file_name);        
    }     
    bzero(buffer, BUFFER_SIZE);      
    int length = 0;    
    while((length = recv(sfd, buffer, BUFFER_SIZE, 0)) > 0)      
    {        
        printf("length = %d\n", length);  
        if(strcmp(buffer, "OK") == 0)  
        {  
            break;  
        }  
        if(fwrite(buffer, sizeof(char), length, fp) < length)        
        {          
            printf("File:\t%s Write Failed!\n", file_name);          
            break;        
        }  
     
        bzero(buffer, BUFFER_SIZE);      
    }       // 接收成功后，关闭文件，关闭socket      
    printf("Receive File:\t%s From Server IP Successful!\n", file_name);      
    fclose(fp);      
}  