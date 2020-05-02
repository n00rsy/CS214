make:
	make server
	make client
server:
	gcc server/server.c -pthread -o WTFServer
client:
	gcc client.c -lssl -lcrypto -pthread -o WTF
hash:
	gcc -g -Wall -o hash hash.c -lssl -lcrypto
