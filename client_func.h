    /*client_func.h*/  
      
    #ifndef CLIENT_H  
    #define CLIENT_H  
      
    #include <stdio.h>  
    #include <stdlib.h>  
    #include <string.h>  
    #include <sys/types.h>  
    #include <sys/socket.h>  
    #include <netinet/in.h>  
    #include <arpa/inet.h>  
    #include <unistd.h>  
    #include <signal.h>  
    #include <pthread.h>  
    #include <errno.h>  
    #include <libgen.h>  
      
    #define MAX_SIZE 1024  
    #define CMD_EXIT "exit"  
    #define CMD_DOWNLOAD_FILE "df"  
      
    typedef enum  tagCmdID  
    {  
        CMD_INVALID = -1,  
        CMD_FILE_EXIST,  
        CMD_FILE_NOT_EXIST  
    }E_CMD_ID;  
      
    typedef struct tagClientCom  
    {  
        E_CMD_ID cmd_id;  
        long length;  
    }T_CLIENT_COM_HEADER;  
      
    typedef struct  
    {  
        int cfd;  
        char filename[128];  
          
    }file_information;  
      
    int tcp_connect(const char* ip, int port);  
    void* recv_file(void *arg);  
    void* pthread_send(void *arg);  
      
    #endif  