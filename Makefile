all: server client

client: client.cpp
	g++ -o client client.cpp

server: server.cpp
	g++ -pthread -o server server.cpp

clean:
	-rm -f *.o server client