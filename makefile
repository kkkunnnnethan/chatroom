target:server.o client.o

server.o:server.cpp
	g++ server.cpp -o server.out -pthread
client.o:client.cpp
	g++ client.cpp -o client.out -pthread

clean:
	rm server.out client.out
