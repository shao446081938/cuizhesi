.PHONY : clean server

#server: client server.c
#   gcc server.c -o server -lpthread
#client: client.c
#   gcc client.c -o client -lpthread
#clean:
#   pkill -9 server
#   pkill -9 client
#   rm server client
    
GCCTV = arm-linux-androideabi-gcc
GCC = gcc
object = server_func.o server.o client_func.o client.o
LFLAG = -lpthread
#LFLAG = -pie -fPIE

server :    server.o server_func.o client
	$(GCC) -o server server.o server_func.o $(LFLAG)
server.o :  server.c
	$(GCC) -c server.c $(LFLAG)
server_func.o : server_func.c
	$(GCC) -c server_func.c $(LFLAG)

client :    client.o client_func.o
	$(GCC) -o client client.o client_func.o -lpthread
client.o :  client.c
	$(GCC) -c client.c -lpthread
client_func.o : client_func.c
	$(GCC) -c client_func.c -lpthread
    
clean:
	rm $(object)
