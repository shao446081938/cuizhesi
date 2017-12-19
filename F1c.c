void recvfile(int client_socket_fd, char Down_Filename[])       //客户端从服务器下载文件  
{      
    char file_name[FILE_NAME];  
    char buffer[BUFFER_SIZE];  
              
    strcpy(file_name, Down_Filename);           
    // 向服务器发送buffer中的数据      
  
    FILEFILE *fp = fopen(file_name, "w");      
    if(NULL == fp)      
    {        
        printf("File:\t%s Can Not Open To Write\n", file_name);        
        exit(1);      
    }         
    // 从服务器接收数据到buffer中      
    // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止      
    bzero(buffer, BUFFER_SIZE);      
    int length = 0;    
      
    while((length = recv(client_socket_fd, buffer, BUFFER_SIZE, 0)) > 0)      
    {        
        printf("length = %d\n", length);  
        if(strcmp(buffer, "OK") == 0)  
        {  
            break;  
        }  
        if(fwrite(buffer, sizeof(char), length, fp) < length)        
        {          
            printf("File:\t%s Write Failed\n", file_name);          
            break;        
        }        
        bzero(buffer, BUFFER_SIZE);    //在这里注意使用清空函数，否则会发生意想不到的错误     
    }       // 接收成功后，关闭文件，关闭socket      
    printf("Receive File:\t%s From Server IP Successful!\n", file_name);      
    fclose(fp);    
    printf("client_socket_fd = %d\n", client_socket_fd);    
}  
  
void sendfile(int client_socket_fd, char Send_Filename[])    //上传文件到服务器  
{     
    char file_name[FILE_NAME];   
    char buffer[BUFFER_SIZE];     
         
    strcpy(file_name, Send_Filename);   
      
    // 打开文件，准备写入        
    FILEFILE *fp = fopen(file_name, "r");     
  
    if(NULL == fp)        
    {          
        printf("File:%s Not Found\n", file_name);        
    }        
    else       
    {          
        bzero(buffer, BUFFER_SIZE);        //在这里注意使用清空函数，否则会发生意想不到的错误     
        int length = 0;        // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止   
        while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)          
        {            
            printf("length = %d\n", length);  
            if(send(client_socket_fd, buffer, length, 0) < 0)            
            {              
                printf("Send File:%s Failed.\n", file_name);              
                break;            
            }                    
            bzero(buffer, BUFFER_SIZE);     //在这里注意使用清空函数，否则会发生意想不到的错误     
        }  sleep(1);        // 关闭文件       //睡眠函数延迟，这个必须有，因为是非阻塞   
        fclose(fp);   
        send(client_socket_fd, "OK", 2, 0);         
        printf("File:%s Transfer Successful!\n", file_name);      
    }      // 关闭与客户端的连接        
}  