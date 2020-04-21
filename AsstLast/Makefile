make:
	make server
	make client
server:
	gcc server.c -pthread -o s
client:
	gcc client.c -pthread -o c
hash:
	gcc -g -Wall -o hash hash.c -lssl -lcrypto
