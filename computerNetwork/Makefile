TARGET = client myserver
normal: $(TARGET)
client: client.c
	gcc client.c -o client
myserver: server.c
	gcc server.c -o myserver
clean:
	$(RM) $(TARGET)