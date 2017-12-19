    /*client.c*/  
      
    #include "client_func.h"   
    int main(int argc,char **argv)  
    {  
        int ret_send;  
      
        pthread_t id1;  
          
        int sfd = tcp_connect("192.168.20.217", 1234);  
      
        int *p_sfd = &sfd;  
        ret_send = pthread_create(&id1, NULL, (void*)pthread_send, (void*)p_sfd);  
        if(ret_send != 0)  
        {  
            printf("create pthread error!\n");  
        }  
        else  
        {  
            pthread_join(id1, NULL);  
        }  
      
        close(sfd);  
      
        return 0;  
    }  